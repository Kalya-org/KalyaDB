cmake_minimum_required(VERSION 3.20)

project(KalyaDBNative
    VERSION 0.1.0
    DESCRIPTION "Native core for the KalyaDB database engine"
    LANGUAGES CXX
)

add_library(kalyadb_native STATIC
    src/version.cpp
    src/storage.cpp
)

target_include_directories(kalyadb_native
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(kalyadb_native PUBLIC cxx_std_17)

set_target_properties(kalyadb_native PROPERTIES
    CXX_EXTENSIONS OFF
)

include(CTest)

if(BUILD_TESTING)
    add_executable(kalyadb_version_test
        tests/version_test.cpp
    )
    target_link_libraries(kalyadb_version_test PRIVATE kalyadb_native)
    add_test(NAME kalyadb.version COMMAND kalyadb_version_test)

    add_executable(kalyadb_storage_test
        tests/storage_test.cpp
    )
    target_link_libraries(kalyadb_storage_test PRIVATE kalyadb_native)
    add_test(NAME kalyadb.storage COMMAND kalyadb_storage_test)
endif()
