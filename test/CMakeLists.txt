if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src)

add_executable(libxmp-test test.c ../src/md5.c)
set_property(TARGET libxmp-test PROPERTY C_STANDARD 90)

if(MSVC)
    target_compile_definitions(libxmp-test PRIVATE ${LIBXMP_MSVC_DEFINES})
endif()

if(BUILD_SHARED) # Make sure test.exe will find its dll
    set_property(TARGET libxmp-test PROPERTY RUNTIME_OUTPUT_DIRECTORY ${libxmp_BINARY_DIR})
endif()

target_link_libraries(libxmp-test XMP_IF)

add_test(NAME libxmp-test COMMAND libxmp-test WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
if(LIBXMP_DISABLE_DEPACKERS)
    target_compile_definitions(libxmp-test PRIVATE LIBXMP_NO_DEPACKERS)
endif()

if(BUILD_LITE)
    add_executable(libxmp-lite-test testlite.c ../src/md5.c)
    set_property(TARGET libxmp-lite-test PROPERTY C_STANDARD 90)
    if(MSVC)
        target_compile_definitions(libxmp-lite-test PRIVATE ${LIBXMP_MSVC_DEFINES})
    endif()
    if(BUILD_SHARED) # Make sure test.exe will find its dll
        set_property(TARGET libxmp-lite-test PROPERTY RUNTIME_OUTPUT_DIRECTORY ${libxmp_BINARY_DIR})
    endif()
    target_link_libraries(libxmp-lite-test XMPLITE_IF)
    add_test(NAME libxmp-lite-test COMMAND libxmp-lite-test WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
    target_compile_definitions(libxmp-lite-test PRIVATE LIBXMP_CORE_PLAYER)
    if(LIBXMP_DISABLE_IT)
        target_compile_definitions(libxmp-lite-test PRIVATE LIBXMP_CORE_DISABLE_IT)
    endif()
endif()
