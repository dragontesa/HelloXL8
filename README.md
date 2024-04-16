How to build
----------------
Step1. Install  grpc 1.54 into above directory
 or You can change SDK_INSTALL_DIR in xgate_e2e_cpp_example/cmake/grpc.cmake 

tar xJf grpc1.5.4 -C ..
Step2. make build directory 
mkdir xgate_e2e_cpp_example/build
cd xgate_e2e_cpp_example/build

 -- Windows --
$env:UE_THIRD_PARTY_DIR="C:\UnrealEngine\Engine\Source\ThirdParty"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DOPENSSL_INCLUDE_DIR="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\include\Win64\VS2015" -DLIB_EAY_LIBRARY_DEBUG="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Debug\libcrypto.lib" -DLIB_EAY_LIBRARY_RELEASE="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Release\libcrypto.lib" -DLIB_EAY_DEBUG="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Debug\libcrypto.lib" -DLIB_EAY_RELEASE="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Release\libcrypto.lib" -DSSL_EAY_DEBUG="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Debug\libssl.lib" -DSSL_EAY_RELEASE="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Release\libssl.lib" -DSSL_EAY_LIBRARY_DEBUG="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\Debug\libssl.lib" -DSSL_EAY_LIBRARY_RELEASE="$env:UE_THIRD_PARTY_DIR\OpenSSL\1.1.1n\lib\Win64\VS2015\RELEASE\libssl.lib" -DgRPC_ZLIB_PROVIDER=package -DZLIB_INCLUDE_DIR="$env:UE_THIRD_PARTY_DIR\zlib\v1.2.8\include\Win64\VS2015" -DZLIB_LIBRARY_DEBUG="$env:UE_THIRD_PARTY_DIR\zlib\v1.2.8\lib\Win64\VS2015\Debug\zlibstatic.lib" -DZLIB_LIBRARY_RELEASE="$env:UE_THIRD_PARTY_DIR\zlib\v1.2.8\lib\Win64\VS2015\Release\zlibstatic.lib" -G "Visual Studio 17 2022" ..

cmake --build . --target ALL_BUILD --config Release
