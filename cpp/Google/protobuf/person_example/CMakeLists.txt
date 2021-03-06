cmake_minimum_required(VERSION 3.9)

project(protobuf-examples CXX)

find_package(protobuf CONFIG REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# https://gitlab.kitware.com/cmake/community/wikis/FAQ#how-can-i-build-my-msvc-application-with-a-static-runtime
# https://github.com/kkos/oniguruma/blob/c0881d81467216ecdfa1452c54271e7f60c8e4a5/CMakeLists.txt#L74-L81
if(MSVC AND protobuf_MSVC_STATIC_RUNTIME)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach()
endif()

add_executable(person_cpp ${CMAKE_CURRENT_LIST_DIR}/person.cpp)

# Code generation
protobuf_generate(
    LANGUAGE cpp
    TARGET person_cpp
    PROTOS ${CMAKE_CURRENT_LIST_DIR}/person.proto
)

if (MSVC)
    #add_definitions(/wd4005 /wd4251) # Works fine! Protobuf's CMakeLists.txt uses this way.
    #add_compile_definitions(/wd4005 /wd4251) # Not works here!
    #add_compile_options(/wd4005 /wd4251) # Not Works here!
endif ()

#target_compile_definitions(person_cpp PRIVATE /wd4005 /wd4251) # Not works here!

target_compile_options(
    person_cpp
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /wd4005
            /wd4251
        >
)

target_link_libraries(person_cpp PRIVATE protobuf::libprotobuf)

add_custom_command(TARGET person_cpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_LIST_DIR}/person_with_email.json"
        $<TARGET_FILE_DIR:person_cpp>
)

add_custom_command(TARGET person_cpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_LIST_DIR}/person_without_email.json"
        $<TARGET_FILE_DIR:person_cpp>
)


# References:
# https://github.com/protocolbuffers/protobuf/blob/master/examples/CMakeLists.txt
# https://stackoverflow.com/questions/52533396/cmake-cant-find-protobuf-protobuf-generate-cpp
# https://stackoverflow.com/questions/16736819/protobuf-generate-cpp-during-creation-of-cmake-files
