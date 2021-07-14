cmake_minimum_required(VERSION 3.2)
project(libxmp VERSION 4.5.0 LANGUAGES C)

include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(TestBigEndian)

# -----------------------------------
if(POLICY CMP0075) # Include file check macros honor CMAKE_REQUIRED_LIBRARIES, CMake >= 3.12
    cmake_policy(SET CMP0075 NEW)
endif()

if(POLICY CMP0077) # Cache variables override since 3.12
    cmake_policy(SET CMP0077 NEW)
endif()


# If platform is Emscripten
if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(EMSCRIPTEN 1)
endif()

# Strip garbage
if(APPLE)
    string(REGEX REPLACE "-O3" ""
        CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
    set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")

    # Unify visibility to meet llvm's default.
    include(CheckCCompilerFlag)
    check_c_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
    if(SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility-inlines-hidden")
    endif()
elseif(NOT MSVC)
    if(EMSCRIPTEN)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -fdata-sections -ffunction-sections")
        if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")
        else()
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
        endif()
    else()
        string(REGEX REPLACE "-O3" ""
            CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -fdata-sections -ffunction-sections")
        if(ANDROID)
            set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -funwind-tables")
        elseif(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
            set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -Wl,--gc-sections -Wl,-s")
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
        else()
            set(LINK_FLAGS_RELEASE  "${LINK_FLAGS_RELEASE} -dead_strip")
        endif()
    endif()
endif()

# Global optimization flags
if(NOT MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-omit-frame-pointer")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    add_definitions(-DNDEBUG)
endif()

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

# Disable bogus MSVC warnings
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# -fPIC thing
if(NOT WIN32)
    option(LIBXMP_PIC   "Build the static library as the position-independent" OFF)
endif()
# -----------------------------------

if(EMSCRIPTEN)
    set(BUILD_SHARED_DEFAULT OFF)
else()
    set(BUILD_SHARED_DEFAULT ON)
endif()

option(BUILD_STATIC   "Build static library of libXMP" ON)
option(BUILD_SHARED   "Build shared library of libXMP" ${BUILD_SHARED_DEFAULT})
if(NOT BUILD_STATIC AND NOT BUILD_SHARED)
    message(FATAL_ERROR "Both static and shared builds got disabled. You must enable at least one of them.")
endif()

option(LIBXMP_BUILD_LITE   "Build lite version of libXMP" ON)
option(LIBXMP_BUILD_FULL   "Build full version of libXMP" ON)
if(NOT LIBXMP_BUILD_LITE AND NOT LIBXMP_BUILD_FULL)
    message(FATAL_ERROR "Both Full and Lite versions got disabled. You must enable at least one of them.")
endif()

option(LIBXMP_DISABLE_IT   "Disable IT support at libXMP-Lite" OFF)
option(LIBXMP_DISABLE_DEPACKERS     "Disable archive depackers at libXMP" OFF)
option(LIBXMP_DISABLE_PROWIZARD     "Disable the ProWizard at libXMP" OFF)

TEST_BIG_ENDIAN(WORDS_BIGENDIAN)
if(WORDS_BIGENDIAN)
    add_definitions(-DWORDS_BIGENDIAN=1)
endif()

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    add_definitions(-DDEBUG -D_DEBUG -DDEBUG_NOCOLOR)
endif()

check_include_file(alloca.h HAVE_ALLOCA_H)
if(HAVE_ALLOCA_H)
    add_definitions(-DHAVE_ALLOCA_H=1)
endif()

check_symbol_exists(powf "math.h" HAVE_POWF_FUNCTION)
if(HAVE_POWF_FUNCTION)
    add_definitions(-DHAVE_POWF=1)
endif()

check_function_exists(popen HAVE_POPEN_F)
check_symbol_exists(popen "stdio.h" HAVE_POPEN)
if(HAVE_POPEN AND HAVE_POPEN_F)
    add_definitions(-DHAVE_POPEN=1)
endif()

check_function_exists(fnmatch HAVE_FNMATCH_F)
check_symbol_exists(fnmatch "fnmatch.h" HAVE_FNMATCH)
if(HAVE_FNMATCH AND HAVE_FNMATCH_F)
    add_definitions(-DHAVE_FNMATCH=1)
endif()

check_function_exists(umask HAVE_UMASK_F)
check_symbol_exists(umask "sys/stat.h" HAVE_UMASK)
if(HAVE_UMASK AND HAVE_UMASK_F)
    add_definitions(-DHAVE_UMASK=1)
endif()

check_function_exists(wait HAVE_WAIT_F)
check_symbol_exists(wait "sys/wait.h" HAVE_WAIT)
if(HAVE_WAIT AND HAVE_WAIT_F)
    add_definitions(-DHAVE_WAIT=1)
endif()

check_function_exists(pipe HAVE_PIPE_F)
check_symbol_exists(pipe "unistd.h" HAVE_PIPE)
if(HAVE_PIPE AND HAVE_PIPE_F)
    add_definitions(-DHAVE_PIPE=1)
endif()

check_function_exists(fork HAVE_FORK_F)
check_symbol_exists(fork "unistd.h" HAVE_FORK)
if(HAVE_FORK AND HAVE_FORK_F)
    add_definitions(-DHAVE_FORK=1)
endif()

check_function_exists(execvp HAVE_EXECVP_F)
check_symbol_exists(execvp "unistd.h" HAVE_EXECVP)
if(HAVE_EXECVP AND HAVE_EXECVP_F)
    add_definitions(-DHAVE_EXECVP=1)
endif()

check_function_exists(dup2 HAVE_DUP2_F)
check_symbol_exists(dup2 "unistd.h" HAVE_DUP2)
if(HAVE_DUP2 AND HAVE_DUP2_F)
    add_definitions(-DHAVE_DUP2=1)
endif()

check_function_exists(mkstemp HAVE_MKSTEMP_F)
check_symbol_exists(mkstemp "stdlib.h" HAVE_MKSTEMP)
if(HAVE_MKSTEMP AND HAVE_MKSTEMP_F)
    add_definitions(-DHAVE_MKSTEMP=1)
endif()

check_symbol_exists(opendir "dirent.h" HAVE_DIRENT)
check_function_exists(opendir HAVE_OPENDIR)
check_function_exists(readdir HAVE_READDIR)
if(HAVE_DIRENT AND HAVE_OPENDIR AND HAVE_READDIR)
    add_definitions(-DHAVE_DIRENT=1)
endif()

if(UNIX AND NOT (WIN32 OR APPLE OR HAIKU OR EMSCRIPTEN OR BEOS))
    find_library(LIBM_LIBRARY m)
    if(LIBM_LIBRARY) # No need to link it by an absolute path
        set(LIBM_REQUIRED 1)
        set(LIBM_LIBRARY m)
        list(APPEND CMAKE_REQUIRED_LIBRARIES m)
    endif()
    mark_as_advanced(LIBM_LIBRARY)
endif()

if("${CMAKE_C_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-shift-negative-value")
endif()

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4244 /wd4018 /wd4996 /wd4048 /wd4267")
    # set(CMAKE_LINK_LIBRARY_FLAG "${CMAKE_LINK_LIBRARY_FLAG} /wd4273")
else()
    if(NOT "${CMAKE_C_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable -Wno-stringop-truncation")
    endif()
endif()


set(LIBXMP_SRC_LITE)
set(LIBXMP_SRC_FULL)
set(LIBXMP_SRC_COMMON)

# Lite version
set(LIBXMP_SRC_LIST_LITE_ONLY
    lite/src/format.c
    lite/src/loaders/mod_load.c
)

set(LIBXMP_SRC_LIST_LITE
    src/control.c
    src/dataio.c
    src/effects.c
    src/filter.c
    src/hio.c
    src/lfo.c
    src/misc.c
    src/load.c
    src/loaders/common.c
    src/loaders/it_load.c
    src/loaders/itsex.c
    src/loaders/s3m_load.c
    src/loaders/sample.c
    src/loaders/xm_load.c
    src/load_helpers.c
    src/memio.c
    src/mix_all.c
    src/mixer.c
    src/period.c
    src/player.c
    src/read_event.c
    src/scan.c
    src/smix.c
    src/virtual.c
)

# Full version
set(LIBXMP_SRC_LIST_FULL_ONLY
    src/format.c
    src/loaders/mod_load.c
)

set(LIBXMP_SRC_LIST_FULL
    src/extras.c
    src/fmopl.c
    src/hmn_extras.c
    src/loaders/669_load.c
    src/loaders/abk_load.c
    src/loaders/amf_load.c
    src/loaders/arch_load.c
    src/loaders/asif.c
    src/loaders/asylum_load.c
    src/loaders/chip_load.c
    src/loaders/coco_load.c
    src/loaders/dbm_load.c
    src/loaders/digi_load.c
    src/loaders/dt_load.c
    src/loaders/emod_load.c
    src/loaders/far_load.c
    src/loaders/flt_load.c
    src/loaders/fnk_load.c
    src/loaders/gal4_load.c
    src/loaders/gal5_load.c
    src/loaders/gdm_load.c
    src/loaders/hmn_load.c
    src/loaders/ice_load.c
    src/loaders/iff.c
    src/loaders/imf_load.c
    src/loaders/ims_load.c
    src/loaders/liq_load.c
    src/loaders/masi_load.c
    src/loaders/mdl_load.c
    src/loaders/med2_load.c
    src/loaders/med3_load.c
    src/loaders/med4_load.c
    src/loaders/mfp_load.c
    src/loaders/mgt_load.c
    src/loaders/mmd1_load.c
    src/loaders/mmd3_load.c
    src/loaders/mmd_common.c
    src/loaders/mtm_load.c
    src/loaders/no_load.c
    src/loaders/okt_load.c
    src/loaders/psm_load.c
    src/loaders/pt3_load.c
    src/loaders/ptm_load.c
    src/loaders/pw_load.c
    src/loaders/rtm_load.c
    src/loaders/sfx_load.c
    src/loaders/stim_load.c
    src/loaders/st_load.c
    src/loaders/stm_load.c
    src/loaders/stx_load.c
    src/loaders/sym_load.c
    src/loaders/ult_load.c
    src/loaders/umx_load.c
    src/loaders/voltable.c
    src/md5.c
    src/med_extras.c
    src/mix_paula.c
    src/mkstemp.c
    src/tempfile.c
)

set(LIBXMP_SRC_LIST_PROWIZARD
    src/loaders/prowizard/ac1d.c
    src/loaders/prowizard/di.c
    src/loaders/prowizard/eureka.c
    src/loaders/prowizard/fc-m.c
    src/loaders/prowizard/fuchs.c
    src/loaders/prowizard/fuzzac.c
    src/loaders/prowizard/gmc.c
    src/loaders/prowizard/heatseek.c
    src/loaders/prowizard/hrt.c
    src/loaders/prowizard/ksm.c
    src/loaders/prowizard/mp.c
    src/loaders/prowizard/noiserun.c
    src/loaders/prowizard/novotrade.c
    src/loaders/prowizard/np1.c
    src/loaders/prowizard/np2.c
    src/loaders/prowizard/np3.c
    src/loaders/prowizard/p40.c
    src/loaders/prowizard/p61a.c
    src/loaders/prowizard/pha.c
    src/loaders/prowizard/pm10c.c
    src/loaders/prowizard/pm18a.c
    src/loaders/prowizard/pp10.c
    src/loaders/prowizard/pp21.c
    src/loaders/prowizard/prowiz.c
    src/loaders/prowizard/prun1.c
    src/loaders/prowizard/prun2.c
    src/loaders/prowizard/ptktable.c
    src/loaders/prowizard/skyt.c
    src/loaders/prowizard/starpack.c
    src/loaders/prowizard/tdd.c
    src/loaders/prowizard/theplayer.c
    src/loaders/prowizard/titanics.c
    src/loaders/prowizard/tp1.c
    src/loaders/prowizard/tp3.c
    src/loaders/prowizard/tuning.c
    src/loaders/prowizard/unic2.c
    src/loaders/prowizard/unic.c
    src/loaders/prowizard/wn.c
    src/loaders/prowizard/xann.c
    src/loaders/prowizard/zen.c
)

set(LIBXMP_SRC_LIST_DEPACKERS
    src/depackers/arcfs.c
    src/depackers/bunzip2.c
    src/depackers/crc32.c
    src/depackers/depacker.c
    src/depackers/gunzip.c
    src/depackers/inflate.c
    src/depackers/mmcmp.c
    src/depackers/muse.c
    src/depackers/oxm.c
    src/depackers/ppdepack.c
    src/depackers/readlzw.c
    src/depackers/readrle.c
    src/depackers/s404_dec.c
    src/depackers/unarc.c
    src/depackers/uncompress.c
    src/depackers/unlha.c
    src/depackers/unlzx.c
    src/depackers/unsqsh.c
    src/depackers/unxz.c
    src/depackers/unzip.c
    src/depackers/vorbis.c
    src/depackers/xfnmatch.c
    src/depackers/xz_dec_lzma2.c
    src/depackers/xz_dec_stream.c
)

if(AMIGA)
    list(APPEND LIBXMP_SRC_LIST_DEPACKERS
        src/depackers/xfd.c
        src/depackers/xfd_link.c
    )
endif()

if(WIN32)
    list(APPEND LIBXMP_SRC_COMMON
        src/win32.c
        src/win32/ptpopen.c
    )
    if(MSVC AND CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
        list(APPEND LIBXMP_SRC_COMMON
            src/win32/debug.c
        )
    endif()
endif()

set(LIBXMP_DEFINES)

if(DISABLE_IT)
    list(APPEND LIBXMP_DEFINES -DLIBXMP_CORE_DISABLE_IT)
endif()

if(LIBXMP_DISABLE_DEPACKERS)
    list(APPEND LIBXMP_DEFINES -DLIBXMP_NO_DEPACKERS)
else()
    list(APPEND LIBXMP_SRC_LIST_FULL_ONLY ${LIBXMP_SRC_LIST_DEPACKERS})
endif()

if(LIBXMP_DISABLE_PROWIZARD)
    list(APPEND LIBXMP_DEFINES -DLIBXMP_NO_PROWIZARD)
else()
    list(APPEND LIBXMP_SRC_LIST_FULL_ONLY ${LIBXMP_SRC_LIST_PROWIZARD})
endif()

list(APPEND LIBXMP_SRC_LITE ${LIBXMP_SRC_LIST_LITE} ${LIBXMP_SRC_LIST_LITE_ONLY} ${LIBXMP_SRC_COMMON})
list(APPEND LIBXMP_SRC_FULL ${LIBXMP_SRC_LIST_LITE} ${LIBXMP_SRC_LIST_FULL_ONLY} ${LIBXMP_SRC_LIST_FULL} ${LIBXMP_SRC_COMMON})

set(XMP_INSTALLS)

# ============= FULL =============
if(LIBXMP_BUILD_FULL AND BUILD_STATIC)
    add_library(xmp_static STATIC ${LIBXMP_SRC_FULL})
    list(APPEND XMP_INSTALLS xmp_static)
    set_target_properties(xmp_static PROPERTIES OUTPUT_NAME xmp C_STANDARD 90)

    if(WIN32 AND BUILD_SHARED)
        set_target_properties(xmp_static PROPERTIES OUTPUT_NAME xmp-static)
    else()
        set_target_properties(xmp_static PROPERTIES OUTPUT_NAME xmp)
    endif()

    target_compile_definitions(xmp_static PRIVATE ${LIBXMP_DEFINES} -DBUILDING_STATIC)

    if(MSVC)
        target_compile_definitions(xmp_static PRIVATE -DPATH_MAX=260)
    endif()

    if(LIBXMP_PIC)
        target_compile_options(xmp_static PRIVATE -fPIC)
    endif()

    target_include_directories(xmp_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(xmp_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    if(LIBM_REQUIRED)
        target_link_libraries(xmp_static PUBLIC ${LIBM_LIBRARY})
    endif()
endif()

if(LIBXMP_BUILD_FULL AND BUILD_SHARED)
    add_library(xmp_shared SHARED ${LIBXMP_SRC_FULL})
    list(APPEND XMP_INSTALLS xmp_shared)
    set_target_properties(xmp_shared PROPERTIES OUTPUT_NAME xmp C_STANDARD 90)

    if(NOT MSVC)
        target_link_libraries(xmp_shared PUBLIC m)
    endif()

    target_compile_definitions(xmp_shared PRIVATE ${LIBXMP_DEFINES} -DBUILDING_DLL)

    if(MSVC)
        target_compile_definitions(xmp_shared PRIVATE -DPATH_MAX=260)
    endif()

    target_include_directories(xmp_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(xmp_shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    set_target_properties(xmp_shared PROPERTIES VERSION ${libxmp_VERSION} SOVERSION ${libxmp_VERSION_MAJOR})
endif()


# ============= LITE =============

set(LIBXMP_LITE_DEFINES
    -DLIBXMP_CORE_PLAYER
    -DLIBXMP_NO_PROWIZARD
    -DLIBXMP_NO_DEPACKERS
)

if(LIBXMP_BUILD_LITE AND BUILD_STATIC)
    add_library(xmp_lite_static STATIC ${LIBXMP_SRC_LITE})
    list(APPEND XMP_INSTALLS xmp_lite_static)

    if(WIN32 AND BUILD_SHARED)
        set_target_properties(xmp_lite_static PROPERTIES OUTPUT_NAME xmp-lite-static)
    else()
        set_target_properties(xmp_lite_static PROPERTIES OUTPUT_NAME xmp-lite)
    endif()

    target_compile_definitions(xmp_lite_static PRIVATE ${LIBXMP_DEFINES} ${LIBXMP_LITE_DEFINES} -DBUILDING_STATIC)

    if(MSVC)
        target_compile_definitions(xmp_lite_static PRIVATE -DPATH_MAX=260)
    endif()

    if(LIBXMP_PIC)
        target_compile_options(xmp_lite_static PRIVATE -fPIC)
    endif()

    target_include_directories(xmp_lite_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(xmp_lite_static PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/loaders
    )
endif()

if(LIBXMP_BUILD_LITE AND BUILD_SHARED)
    add_library(xmp_lite_shared SHARED ${LIBXMP_SRC_LITE})
    list(APPEND XMP_INSTALLS xmp_lite_shared)
    set_target_properties(xmp_lite_shared PROPERTIES OUTPUT_NAME xmp-lite)

    if(NOT MSVC)
        target_link_libraries(xmp_lite_shared PUBLIC m)
    endif()

    target_compile_definitions(xmp_lite_shared PRIVATE ${LIBXMP_DEFINES} ${LIBXMP_LITE_DEFINES} -DBUILDING_DLL)

    if(MSVC)
        target_compile_definitions(xmp_lite_shared PRIVATE -DPATH_MAX=260)
    endif()

    target_include_directories(xmp_lite_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(xmp_lite_shared PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/loaders
    )

    set_target_properties(xmp_lite_shared PROPERTIES VERSION ${libxmp_VERSION} SOVERSION ${libxmp_VERSION_MAJOR})
endif()



add_library(XMP_IF INTERFACE)
if(LIBXMP_BUILD_FULL)
    if(NOT BUILD_SHARED)
        target_link_libraries(XMP_IF INTERFACE xmp_static)
    else()
        target_link_libraries(XMP_IF INTERFACE xmp_shared)
    endif()
elseif(LIBXMP_BUILD_LITE)
    if(NOT BUILD_SHARED)
        target_link_libraries(XMP_IF INTERFACE xmp_lite_static)
    else()
        target_link_libraries(XMP_IF INTERFACE xmp_lite_shared)
    endif()
endif()

# === Unit tests ====
option(WITH_UNIT_TESTS "Enable unit testing" OFF)
if(WITH_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(test-dev)
endif()

option(XMP_WITH_DEMO_PLAYER_SDL1      "Build also demo player for XMP (SDL1 needed)" OFF)
if(XMP_WITH_DEMO_PLAYER_SDL1)
    add_executable(xmpsdl
        examples/player-sdl.c
    )
    add_dependencies(xmpsdl XMP_IF)

    find_package(SDL REQUIRED)
    if(TARGET SDL::SDL)
        set(SDL_LIBRARY SDL::SDLmain SDL::SDL)
    endif()

    target_compile_definitions(xmpsdl PRIVATE -DBUILDING_STATIC)
    if(WIN32)
        target_compile_definitions(xmpsdl PRIVATE -DSDL_MAIN_HANDLED)
    endif()
    target_include_directories(xmpsdl PRIVATE ${SDL_INCLUDE_DIR})
    target_link_libraries(xmpsdl XMP_IF ${SDL_LIBRARY})
endif()

option(XMP_WITH_DEMO_PLAYER_SDL2      "Build also demo player for XMP (SDL2 needed)" OFF)
if(XMP_WITH_DEMO_PLAYER)
    add_executable(xmpsdl2
        examples/player-sdl.c
    )
    add_dependencies(xmpsdl2 XMP_IF)

    find_package(SDL2 REQUIRED)
    if(TARGET SDL2::SDL2)
        set(SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2)
    endif()

    target_compile_definitions(xmpsdl2 PRIVATE -DBUILDING_STATIC)
    if(WIN32)
        target_compile_definitions(xmpsdl2 PRIVATE -DSDL_MAIN_HANDLED)
    endif()
    target_include_directories(xmpsdl2 PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(xmpsdl2 XMP_IF ${SDL2_LIBRARIES})
endif()

install(TARGETS ${XMP_INSTALLS}
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
        INCLUDES DESTINATION "include")

install(FILES
        include/xmp.h
        DESTINATION include/xmp/)
