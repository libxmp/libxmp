
if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src)

add_executable(libxmp-test test.c ../src/md5.c)
set_property(TARGET libxmp-test PROPERTY C_STANDARD 90)
if(WIN32) # Make sure unit test will find its DLLs while running
    set_property(TARGET libxmp-test PROPERTY RUNTIME_OUTPUT_DIRECTORY ${libxmp_BINARY_DIR})
endif()

if(LIBXMP_DISABLE_DEPACKERS)
    target_compile_definitions(libxmp-test PRIVATE -DLIBXMP_NO_DEPACKERS)
endif()

target_link_libraries(libxmp-test XMP_IF)
if(WIN32)
    target_link_libraries(libxmp-test XMP_IF)
endif()

add_test(NAME libxmp-test COMMAND libxmp-test WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
