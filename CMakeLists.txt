cmake_minimum_required(VERSION 3.2)
project(libxmp VERSION 4.6.0 LANGUAGES C)

set(LIBXMP_DEFINES)
set(LIBXMP_DEFINES_PUBLIC)
set(LIBXMP_CFLAGS)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libxmp-checks.cmake)
include(GNUInstallDirs)

# -fPIC thing
if(NOT WIN32 AND NOT EMSCRIPTEN AND NOT VITA)
    option(LIBXMP_PIC   "Build the static library as position-independent" OFF)
else()
    set(LIBXMP_PIC OFF)
endif()
# -----------------------------------

if(EMSCRIPTEN OR VITA OR PSP)
    set(BUILD_SHARED_DEFAULT OFF)
else()
    set(BUILD_SHARED_DEFAULT ON)
endif()

option(BUILD_STATIC   "Build libXMP static library" ON)
option(BUILD_SHARED   "Build libXMP shared library" ${BUILD_SHARED_DEFAULT})
if(NOT BUILD_STATIC AND NOT BUILD_SHARED)
    message(FATAL_ERROR "Both static and shared builds got disabled. You must enable at least one of them.")
endif()

option(LIBXMP_DISABLE_DEPACKERS     "Disable archive depackers" OFF)
option(LIBXMP_DISABLE_PROWIZARD     "Disable ProWizard format loaders" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libxmp-sources.cmake)

if(LIBXMP_DISABLE_DEPACKERS)
    list(APPEND LIBXMP_DEFINES_PUBLIC -DLIBXMP_NO_DEPACKERS)
else()
    list(APPEND LIBXMP_SRC_LIST ${LIBXMP_SRC_LIST_DEPACKERS})
endif()

if(LIBXMP_DISABLE_PROWIZARD)
    list(APPEND LIBXMP_DEFINES_PUBLIC -DLIBXMP_NO_PROWIZARD)
else()
    list(APPEND LIBXMP_SRC_LIST ${LIBXMP_SRC_LIST_PROWIZARD})
endif()

if(MSVC)
    set(LIBXMP_MSVC_DEFINES -D_USE_MATH_DEFINES)
    list(APPEND LIBXMP_DEFINES ${LIBXMP_MSVC_DEFINES})
endif()


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
    target_compile_definitions(xmp_static PUBLIC ${LIBXMP_DEFINES_PUBLIC} -DLIBXMP_STATIC)

    if(LIBXMP_PIC)
        target_compile_options(xmp_static PRIVATE -fPIC)
    endif()

    target_include_directories(xmp_static PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
    target_include_directories(xmp_static PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

    if(LIBM_REQUIRED)
        target_link_libraries(xmp_static PUBLIC ${LIBM_LIBRARY})
    endif()
endif()

if(BUILD_SHARED)
    add_library(xmp_shared SHARED ${LIBXMP_SRC_LIST})
    add_library(libxmp::xmp_shared ALIAS xmp_shared)
    list(APPEND XMP_INSTALLS xmp_shared)
    set_target_properties(xmp_shared PROPERTIES C_STANDARD 90)
    if(MSVC)
        set_target_properties(xmp_shared PROPERTIES OUTPUT_NAME libxmp)
    else()
        set_target_properties(xmp_shared PROPERTIES OUTPUT_NAME xmp)
    endif()

    if(HAVE_GCC OR HAVE_CLANG)
        if(APPLE)
            set_property(TARGET xmp_shared APPEND_STRING PROPERTY
                         LINK_FLAGS " -Wl,-undefined,error")
        elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
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
        target_compile_definitions(xmp_shared PRIVATE -DUSE_VERSIONED_SYMBOLS=1)
        set_property(TARGET xmp_shared APPEND_STRING PROPERTY
                     LINK_FLAGS " -Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/libxmp.map")
        message(STATUS "Versioned symbols: ENABLED.")
    else()
        message(STATUS "Versioned symbols: DISABLED")
    endif()

    target_compile_definitions(xmp_shared PRIVATE ${LIBXMP_DEFINES})
    if(WIN32)
        target_compile_definitions(xmp_shared PRIVATE -DBUILDING_DLL)
    endif()

    target_compile_options(xmp_shared PRIVATE ${LIBXMP_CFLAGS})
    target_compile_definitions(xmp_shared PUBLIC ${LIBXMP_DEFINES_PUBLIC})

    target_include_directories(xmp_shared PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
    target_include_directories(xmp_shared PUBLIC "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

    set_target_properties(xmp_shared PROPERTIES VERSION ${libxmp_VERSION} SOVERSION ${libxmp_VERSION_MAJOR})

    if(ANDROID AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
        target_link_libraries(xmp_shared PRIVATE log)
    endif()
    if(LIBM_REQUIRED)
        target_link_libraries(xmp_shared PUBLIC ${LIBM_LIBRARY})
    endif()
endif()


add_library(XMP_IF INTERFACE)

if(NOT BUILD_SHARED)
    target_link_libraries(XMP_IF INTERFACE xmp_static)
else()
    target_link_libraries(XMP_IF INTERFACE xmp_shared)
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
else()
    set(cmake_install_cmakdir "${CMAKE_INSTALL_LIBDIR}/cmake/libxmp")
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
