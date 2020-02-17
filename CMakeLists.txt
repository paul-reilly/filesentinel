cmake_minimum_required(VERSION 3.13)

project(filesentinel)

set(SRCS
    src/filesentinel.cpp
    include/filesentinel.hpp
)

add_library(filesentinel STATIC ${SRCS})
add_executable(tests ${SRCS})

target_compile_features(filesentinel PRIVATE cxx_std_17)
target_compile_definitions(filesentinel PRIVATE -DDOCTEST_CONFIG_DISABLE)

target_compile_features(tests PRIVATE cxx_std_17)
set_target_properties(tests PROPERTIES EXCLUDE_FROM_ALL True)

if (UNIX)
    target_link_libraries(filesentinel PRIVATE pthread)
    target_link_libraries(tests PRIVATE pthread)
endif()

target_include_directories(filesentinel PRIVATE include)
target_include_directories(tests PRIVATE include)