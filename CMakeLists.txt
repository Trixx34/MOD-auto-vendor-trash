cmake_minimum_required(VERSION 3.20)
project(auto_vendor_trash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The SDK can come from a local checkout (FOREVER_MODULE_SDK_SOURCE) or from GitHub.
set(FOREVER_MODULE_SDK_SOURCE "" CACHE PATH "Path to a forever-module-sdk checkout (optional)")
set(FOREVER_MODULE_SDK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
if(FOREVER_MODULE_SDK_SOURCE)
    add_subdirectory("${FOREVER_MODULE_SDK_SOURCE}" "${CMAKE_BINARY_DIR}/forever-module-sdk")
else()
    include(FetchContent)
    FetchContent_Declare(forever_module_sdk
        GIT_REPOSITORY https://github.com/Trixx34/forever-module-sdk
        GIT_TAG main)
    FetchContent_MakeAvailable(forever_module_sdk)
endif()

add_library(auto_vendor_policy STATIC src/vendor_policy.cpp)
target_include_directories(auto_vendor_policy PUBLIC src)

add_executable(auto-vendor-trash src/main.cpp)
target_link_libraries(auto-vendor-trash PRIVATE auto_vendor_policy forever_module_sdk)

enable_testing()
add_executable(policy_test tests/policy_test.cpp)
target_link_libraries(policy_test PRIVATE auto_vendor_policy)
add_test(NAME policy COMMAND policy_test)
