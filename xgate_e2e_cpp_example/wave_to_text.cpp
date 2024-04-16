#include <string>
#include <iostream>
#include <fstream>
#include <ios>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "e2e_pipe/api/e2e_api_layer_proto.pb.h"
#include "e2e_pipe/api/e2e_api_layer_proto.grpc.pb.h"

using e2e_pipe::api::ApiType;
using e2e_pipe::api::E2eApiCloseRequest;
using e2e_pipe::api::E2eApiCloseResponse;
using e2e_pipe::api::E2eApiData;
using e2e_pipe::api::E2eApiInitRequest;
using e2e_pipe::api::E2eApiInitResponse;
using e2e_pipe::api::E2eApiResponseType;
using e2e_pipe::api::E2eApiService;
using e2e_pipe::api::E2eApiTransRequest;
using e2e_pipe::api::E2eApiTransResponse;
using e2e_pipe::api::Timeliness;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

const std::string SERVER_ADDRESS("xgate.worker.xl8.ai:17777");
const std::string CLIENT_ID("XGATE");
const std::string API_KEY("62db8e71cba04c2797609483080a8a9a");

class Xl8E2EClient
{
public:
    Xl8E2EClient(std::string server_address)
    {
        channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
        stub = e2e_pipe::api::E2eApiService::NewStub(channel);
    }

    bool initialize(std::string source_language, std::string target_language)
    {
        ClientContext context;
        E2eApiInitRequest initRequest;
        initRequest.set_api_type(ApiType::SPEECH_TO_TEXT);
        initRequest.set_api_key(API_KEY);
        initRequest.set_client_id(CLIENT_ID);
        initRequest.set_timeliness(Timeliness::INTERPRETING);
        initRequest.mutable_source_data_format()->mutable_audio_format()->set_sample_rate(16000);
        initRequest.mutable_source_data_format()->mutable_audio_format()->set_channels(1);
        initRequest.mutable_source_data_format()->set_language_code(source_language);
        initRequest.mutable_target_data_format()->set_language_code(target_language);
        E2eApiInitResponse initResponse;
        Status status = stub->InitE2e(&context, initRequest, &initResponse);
        if (!status.ok())
        {
            std::cout << "Initialization failed: " << status.error_message() << std::endl;
            return false;
        }
        std::cout << "Connected to the server: sessionId=" << initResponse.session_id() << std::endl;
        session_id = initResponse.session_id();
        return true;
    }

    void print_result(const E2eApiData &result)
    {
        if (result.text().length() > 0)
        {
            std::cout << (result.is_partial() ? "[     ] " : "[FINAL] ") << result.text() << " (" << result.original() << ")";
            std::cout << "  [" << result.time_start_msec() << "-" << result.time_end_msec() << "]" << std::endl;
        }
    }

    bool translate(char *audio, size_t size)
    {

        ClientContext context;
        E2eApiTransRequest transRequest;
        E2eApiTransResponse transResponse;
        transRequest.set_session_id(session_id);
        transRequest.mutable_data()->set_audio(audio, size);
        stub->TransE2e(&context, transRequest, &transResponse);
        if (transResponse.type() != E2eApiResponseType::E2E_API_RESPONSE_SUCCESS)
        {
            std::cerr << "ERROR: " << transResponse.error().error_message() << std::endl;
            return false;
        }
        print_result(transResponse.data());
        return true;
    }

    bool close()
    {

        ClientContext context;
        E2eApiCloseRequest closeRequest;
        E2eApiCloseResponse closeResponse;
        closeRequest.set_session_id(session_id);
        closeRequest.set_wait_to_drain(true);
        std::cout << "CLOSING" << std::endl;
        stub->CloseE2e(&context, closeRequest, &closeResponse);
        if (closeResponse.type() != E2eApiResponseType::E2E_API_RESPONSE_SUCCESS)
        {
            std::cerr << "ERROR: " << closeResponse.error().error_message() << std::endl;
            return false;
        }
        std::cout << "CLOSED" << std::endl;
        print_result(closeResponse.data());
        return true;
    }

private:
    std::unique_ptr<E2eApiService::Stub>
        stub;
    std::shared_ptr<Channel> channel;
    std::string session_id;
};

typedef struct CHUNK_HEADER
{
    uint32_t chunk_id;
    uint32_t chunk_size;
} chunk_header;

bool skip_headers(std::ifstream *wavfile)
{
    chunk_header header;
    std::cerr << "header-size=" << sizeof(chunk_header) << std::endl;

    wavfile->read((char *)&header, sizeof(chunk_header));
    if (header.chunk_id != 0x46464952)
    {
        return false;
    }
    #ifndef WIN32
    wavfile->seekg(4, std::ifstream::cur);
    while (*wavfile)
    {
        wavfile->read((char *)&header, sizeof(chunk_header));
        if (header.chunk_id == 0x61746164) // "DATA"
            return true;
        wavfile->seekg(header.chunk_size, std::ifstream::cur);
    }
    #else
    wavfile->seekg(36,std::ifstream::beg);
    wavfile->read((char *)&header, sizeof(chunk_header));
        if (header.chunk_id == 0x61746164) // "DATA"
            return true;
        wavfile->seekg(header.chunk_size, std::ifstream::cur);
    
    #endif
    return false;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << argv[0] << " [wav-file]" << std::endl;
        return 1;
    }
    std::ifstream wavfile(argv[1]);
    if (!skip_headers(&wavfile))
    {
        std::cerr << "Not a wave file: " << argv[1] << std::endl;
        return 1;
    }

    Xl8E2EClient client(SERVER_ADDRESS);
    if (!client.initialize("en", "ko"))
        return 1;

    uint64_t start_msec = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint64_t sent_bytes = 0;
    char buffer[4000];
    while (wavfile)
    {
        wavfile.read(buffer, 4000);
        std::streamsize read_bytes = wavfile.gcount();
        bool ok = client.translate(buffer, (size_t) read_bytes);
        sent_bytes += read_bytes;
        uint64_t sent_msec = sent_bytes * 1000 / 32000;
        while (start_msec + sent_msec > duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count())
        {
#ifndef WIN32
            usleep(10000);
#else
            Sleep(10000);
#endif

        }
    }
    client.close();
    return 0;
}
