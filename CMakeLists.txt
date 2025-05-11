cmake_minimum_required(VERSION 3.4...3.10)
project(libxmp VERSION 4.6.3 LANGUAGES C)

set(LIBXMP_DEFINES)
set(LIBXMPLITE_DEFINES)
set(LIBXMP_CFLAGS)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libxmp-checks.cmake)
include(GNUInstallDirs)

# -fPIC thing
if(NOT WIN32 AND NOT EMSCRIPTEN AND NOT VITA)
    option(LIBXMP_PIC   "Build the static library as position-independent" OFF)
else()
    set(LIBXMP_PIC OFF)
endif()

if(EMSCRIPTEN OR VITA OR PSP)
    set(BUILD_SHARED_DEFAULT OFF)
else()
    set(BUILD_SHARED_DEFAULT ON)
endif()

option(BUILD_STATIC   "Build libXMP static library" ON)
option(BUILD_SHARED   "Build libXMP shared library" ${BUILD_SHARED_DEFAULT})
option(BUILD_LITE     "Build libXMP-lite library" OFF)
if(NOT BUILD_STATIC AND NOT BUILD_SHARED)
    message(FATAL_ERROR "Both static and shared builds got disabled. You must enable at least one of them.")
endif()

option(LIBXMP_DISABLE_DEPACKERS     "Disable archive depackers" OFF)
option(LIBXMP_DISABLE_PROWIZARD     "Disable ProWizard format loaders" OFF)
option(LIBXMP_DISABLE_IT            "Disable IT format in libXMP-lite" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libxmp-sources.cmake)

if(LIBXMP_DISABLE_DEPACKERS)
    list(APPEND LIBXMP_DEFINES LIBXMP_NO_DEPACKERS)
else()
    list(APPEND LIBXMP_SRC_LIST ${LIBXMP_SRC_LIST_DEPACKERS})
endif()
if(LIBXMP_DISABLE_PROWIZARD)
    list(APPEND LIBXMP_DEFINES LIBXMP_NO_PROWIZARD)
else()
    list(APPEND LIBXMP_SRC_LIST ${LIBXMP_SRC_LIST_PROWIZARD})
endif()

#lite-only defs
list(APPEND LIBXMPLITE_DEFINES LIBXMP_CORE_PLAYER)
if(LIBXMP_DISABLE_IT)
    list(APPEND LIBXMPLITE_DEFINES LIBXMP_CORE_DISABLE_IT)
endif()

if(MSVC)
    list(APPEND LIBXMP_DEFINES _USE_MATH_DEFINES)
    list(APPEND LIBXMPLITE_DEFINES _USE_MATH_DEFINES)
else()
    list(APPEND LIBXMP_DEFINES _REENTRANT)
    list(APPEND LIBXMPLITE_DEFINES _REENTRANT)
endif()

#for libxmp-lite.pc
set(libxmplite_VERSION ${libxmp_VERSION})


# extract exports:
set(XMP_EXPORTS)
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/libxmp.map _XMP_EXPORTS)
foreach(export_sym ${_XMP_EXPORTS})
  if(export_sym MATCHES "xmp_.*")
    string(REPLACE ";" "" export_sym ${export_sym})
    string(STRIP ${export_sym} export_sym)
    list(APPEND XMP_EXPORTS "${export_sym}")
  endif()
endforeach()
set(_XMP_EXPORTS)
list(REMOVE_DUPLICATES XMP_EXPORTS)


set(XMP_INSTALLS)

if(BUILD_STATIC)
    add_library(xmp_static STATIC ${LIBXMP_SRC_LIST})
    add_library(libxmp::xmp_static ALIAS xmp_static)
    list(APPEND XMP_INSTALLS xmp_static)
    set_target_properties(xmp_static PROPERTIES C_STANDARD 90)
    if(MSVC)
        set_target_properties(xmp_static PROPERTIES OUTPUT_NAME libxmp-static)
    else()
        set_target_properties(xmp_static PROPERTIES OUTPUT_NAME xmp)
    endif()

    target_compile_options(xmp_static PRIVATE ${LIBXMP_CFLAGS})
    target_compile_definitions(xmp_static PRIVATE ${LIBXMP_DEFINES})
    target_compile_definitions(xmp_static PUBLIC LIBXMP_STATIC)

    if(LIBXMP_PIC)
        target_compile_options(xmp_static PRIVATE -fPIC)
    endif()

    target_include_directories(xmp_static PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
    target_include_directories(xmp_static PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

    if(LIBM_REQUIRED)
        target_link_libraries(xmp_static PUBLIC ${LIBM_LIBRARY})
    endif()

    if(BUILD_LITE)
        add_library(xmp_lite_static STATIC ${LIBXMP_SRC_LIST_LITE})
        add_library(libxmp-lite::xmp_lite_static ALIAS xmp_lite_static)
        list(APPEND XMP_INSTALLS xmp_lite_static)
        set_target_properties(xmp_lite_static PROPERTIES C_STANDARD 90)
        if(MSVC)
            set_target_properties(xmp_lite_static PROPERTIES OUTPUT_NAME libxmp-lite-static)
        else()
            set_target_properties(xmp_lite_static PROPERTIES OUTPUT_NAME xmp-lite)
        endif()

        target_compile_options(xmp_lite_static PRIVATE ${LIBXMP_CFLAGS})
        target_compile_definitions(xmp_lite_static PRIVATE ${LIBXMPLITE_DEFINES})
        target_compile_definitions(xmp_lite_static PUBLIC LIBXMP_STATIC)

        if(LIBXMP_PIC)
            target_compile_options(xmp_lite_static PRIVATE -fPIC)
        endif()

        target_include_directories(xmp_lite_static PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
        target_include_directories(xmp_lite_static PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libxmp-lite>")

        if(LIBM_REQUIRED)
            target_link_libraries(xmp_lite_static PUBLIC ${LIBM_LIBRARY})
        endif()
    endif()
endif()

if(BUILD_SHARED)
    add_library(xmp_shared SHARED ${LIBXMP_SRC_LIST})
    add_library(libxmp::xmp_shared ALIAS xmp_shared)
    list(APPEND XMP_INSTALLS xmp_shared)
    set_target_properties(xmp_shared PROPERTIES C_STANDARD 90)
    if(MSVC)
        set_target_properties(xmp_shared PROPERTIES OUTPUT_NAME libxmp)
    elseif(OS2)
        set_target_properties(xmp_shared PROPERTIES OUTPUT_NAME libxmp)
        set(DEF_CONTENTS "LIBRARY LIBXMP INITINSTANCE TERMINSTANCE\n")
        list(APPEND DEF_CONTENTS "DATA MULTIPLE NONSHARED\n")
        list(APPEND DEF_CONTENTS "EXPORTS\n")
        foreach(export_sym ${XMP_EXPORTS})
            list(APPEND DEF_CONTENTS "    ${export_sym}\n")
        endforeach()
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/os2dll.def ${DEF_CONTENTS})
        target_sources(xmp_shared PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/os2dll.def)
    else()
        set_target_properties(xmp_shared PROPERTIES OUTPUT_NAME xmp)
    endif()

    if(HAVE_GCC OR HAVE_CLANG)
        if(APPLE)
            set_property(TARGET xmp_shared APPEND_STRING PROPERTY
                         LINK_FLAGS " -Wl,-undefined,error")
        elseif(NOT CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
            cmake_push_check_state()
            set(CMAKE_REQUIRED_FLAGS "-Wl,--no-undefined")
            check_c_compiler_flag("" HAVE_NO_UNDEFINED)
            cmake_pop_check_state()
            if(HAVE_NO_UNDEFINED)
                set_property(TARGET xmp_shared APPEND_STRING PROPERTY
                             LINK_FLAGS " -Wl,--no-undefined")
            endif()
        endif()
    endif()

    if(HAVE_VISIBILITY AND HAVE_GNU_LD AND UNIX)
        target_compile_definitions(xmp_shared PRIVATE USE_VERSIONED_SYMBOLS=1)
        set_property(TARGET xmp_shared APPEND_STRING PROPERTY
                     LINK_FLAGS " -Wl,--version-script,\"${CMAKE_CURRENT_SOURCE_DIR}/libxmp.map\"")
        message(STATUS "Versioned symbols: ENABLED.")
    else()
        message(STATUS "Versioned symbols: DISABLED")
    endif()

    target_compile_options(xmp_shared PRIVATE ${LIBXMP_CFLAGS})
    target_compile_definitions(xmp_shared PRIVATE ${LIBXMP_DEFINES})
    if(WIN32)
        target_compile_definitions(xmp_shared PRIVATE BUILDING_DLL)
    endif()

    target_include_directories(xmp_shared PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
    target_include_directories(xmp_shared PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

    set_target_properties(xmp_shared PROPERTIES VERSION ${libxmp_VERSION} SOVERSION ${libxmp_VERSION_MAJOR})

    if(ANDROID AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
        target_link_libraries(xmp_shared PRIVATE log)
    endif()
    if(LIBM_REQUIRED)
        target_link_libraries(xmp_shared PUBLIC ${LIBM_LIBRARY})
    endif()

    if(BUILD_LITE)
        add_library(xmp_lite_shared SHARED ${LIBXMP_SRC_LIST_LITE})
        add_library(libxmp-lite::xmp_lite_shared ALIAS xmp_lite_shared)
        list(APPEND XMP_INSTALLS xmp_lite_shared)
        set_target_properties(xmp_lite_shared PROPERTIES C_STANDARD 90)
        if(MSVC)
            set_target_properties(xmp_lite_shared PROPERTIES OUTPUT_NAME libxmp-lite)
        elseif(OS2)
            set_target_properties(xmp_lite_shared PROPERTIES OUTPUT_NAME libxmplt)
            set(DEF_CONTENTS "LIBRARY LIBXMPLT INITINSTANCE TERMINSTANCE\n")
            list(APPEND DEF_CONTENTS "DATA MULTIPLE NONSHARED\n")
            list(APPEND DEF_CONTENTS "EXPORTS\n")
            foreach(export_sym ${XMP_EXPORTS})
                list(APPEND DEF_CONTENTS "    ${export_sym}\n")
            endforeach()
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/os2lite.def ${DEF_CONTENTS})
            target_sources(xmp_lite_shared PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/os2lite.def)
        else()
            set_target_properties(xmp_lite_shared PROPERTIES OUTPUT_NAME xmp-lite)
        endif()

        if(HAVE_GCC OR HAVE_CLANG)
            if(APPLE)
                set_property(TARGET xmp_lite_shared APPEND_STRING PROPERTY
                             LINK_FLAGS " -Wl,-undefined,error")
            elseif(NOT CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
                cmake_push_check_state()
                set(CMAKE_REQUIRED_FLAGS "-Wl,--no-undefined")
                check_c_compiler_flag("" HAVE_NO_UNDEFINED)
                cmake_pop_check_state()
                if(HAVE_NO_UNDEFINED)
                    set_property(TARGET xmp_lite_shared APPEND_STRING PROPERTY
                                 LINK_FLAGS " -Wl,--no-undefined")
                endif()
            endif()
        endif()

        if(HAVE_VISIBILITY AND HAVE_GNU_LD AND UNIX)
            target_compile_definitions(xmp_lite_shared PRIVATE USE_VERSIONED_SYMBOLS=1)
            set_property(TARGET xmp_lite_shared APPEND_STRING PROPERTY
                         LINK_FLAGS " -Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/libxmp.map")
        endif()

        target_compile_options(xmp_lite_shared PRIVATE ${LIBXMP_CFLAGS})
        target_compile_definitions(xmp_lite_shared PRIVATE ${LIBXMPLITE_DEFINES})
        if(WIN32)
            target_compile_definitions(xmp_lite_shared PRIVATE BUILDING_DLL)
        endif()

        target_include_directories(xmp_lite_shared PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
        target_include_directories(xmp_lite_shared PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libxmp-lite>")

        set_target_properties(xmp_lite_shared PROPERTIES VERSION ${libxmp_VERSION} SOVERSION ${libxmp_VERSION_MAJOR})

        if(ANDROID AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
            target_link_libraries(xmp_lite_shared PRIVATE log)
        endif()
        if(LIBM_REQUIRED)
            target_link_libraries(xmp_lite_shared PUBLIC ${LIBM_LIBRARY})
        endif()
    endif()
endif()


add_library(XMP_IF INTERFACE)
if(NOT BUILD_SHARED)
    target_link_libraries(XMP_IF INTERFACE xmp_static)
else()
    target_link_libraries(XMP_IF INTERFACE xmp_shared)
endif()

if(BUILD_LITE)
    add_library(XMPLITE_IF INTERFACE)
    if(NOT BUILD_SHARED)
        target_link_libraries(XMPLITE_IF INTERFACE xmp_lite_static)
    else()
        target_link_libraries(XMPLITE_IF INTERFACE xmp_lite_shared)
    endif()
endif()

# === Unit tests ====
option(WITH_UNIT_TESTS "Enable unit testing" OFF)
if(WITH_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test-dev/")
        add_subdirectory(test-dev)
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory(examples)
endif()


# === Documentation ====
add_subdirectory(docs)


# === Install ====

include(CMakePackageConfigHelpers)

if(WIN32)
    set(cmake_install_cmakdir "cmake")
    set(cmake_install_cmakdir_lite "cmake")
else()
    set(cmake_install_cmakdir "${CMAKE_INSTALL_LIBDIR}/cmake/libxmp")
    set(cmake_install_cmakdir_lite "${CMAKE_INSTALL_LIBDIR}/cmake/libxmp-lite")
endif()

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
set(libdir     "${CMAKE_INSTALL_FULL_LIBDIR}")
set(bindir     "${CMAKE_INSTALL_FULL_BINDIR}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libxmp.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/libxmp.pc" @ONLY
)
write_basic_package_version_file(libxmp-config-version.cmake
    COMPATIBILITY AnyNewerVersion
)
if(TARGET xmp_shared)
    install(TARGETS xmp_shared EXPORT libxmp_shared_exports
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(EXPORT libxmp_shared_exports
        DESTINATION "${cmake_install_cmakdir}"
        FILE "libxmp-shared-targets.cmake"
        NAMESPACE "libxmp::"
    )
endif()
if(TARGET xmp_static)
    install(TARGETS xmp_static EXPORT libxmp_static_exports
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(EXPORT libxmp_static_exports
        DESTINATION "${cmake_install_cmakdir}"
        FILE "libxmp-static-targets.cmake"
        NAMESPACE "libxmp::"
    )
endif()
install(FILES
        libxmp-config.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/libxmp-config-version.cmake"
        DESTINATION "${cmake_install_cmakdir}"
)
install(FILES
        include/xmp.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libxmp.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
if(BUILD_LITE)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/lite/libxmp-lite.pc.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/libxmp-lite.pc" @ONLY
    )
    write_basic_package_version_file(libxmp-lite-config-version.cmake
            COMPATIBILITY AnyNewerVersion
    )
    if(TARGET xmp_lite_shared)
        install(TARGETS xmp_lite_shared EXPORT libxmp_lite_shared_exports
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
        install(EXPORT libxmp_lite_shared_exports
            DESTINATION "${cmake_install_cmakdir_lite}"
            FILE "libxmp-lite-shared-targets.cmake"
            NAMESPACE "libxmp-lite::"
        )
    endif()
    if(TARGET xmp_lite_static)
        install(TARGETS xmp_lite_static EXPORT libxmp_lite_static_exports
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
        install(EXPORT libxmp_lite_static_exports
            DESTINATION "${cmake_install_cmakdir_lite}"
            FILE "libxmp-lite-static-targets.cmake"
            NAMESPACE "libxmp-lite::"
        )
    endif()
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/lite/libxmp-lite-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libxmp-lite-config-version.cmake"
        DESTINATION "${cmake_install_cmakdir_lite}"
    )
    install(FILES
            include/xmp.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libxmp-lite
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libxmp-lite.pc"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()
