get_filename_component(libxmplite_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
get_filename_component(libxmplite_libdir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(libxmplite_bindir "${libxmplite_root}/bin")
set(libxmplite_incdir "${libxmplite_root}/include")

set(libxmplite_FOUND OFF)

if(WIN32)
    set(libxmplite_sharedlib "${libxmplite_bindir}/${CMAKE_SHARED_LIBRARY_PREFIX}xmp-lite${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(libxmplite_implib "${libxmplite_libdir}/${CMAKE_STATIC_LIBRARY_PREFIX}xmp-lite${CMAKE_SHARED_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    if(EXISTS "${libxmplite_sharedlib}" AND EXISTS "${libxmplite_implib}")
        set(libxmplite_FOUND ON)
        if(NOT TARGET libxmp-lite::xmp_lite_shared)
            add_library(libxmp-lite::xmp_lite_shared SHARED IMPORTED)
            set_target_properties(libxmp-lite::xmp_lite_shared
                PROPERTIES
                    IMPORTED_LOCATION "${libxmplite_sharedlib}"
                    IMPORTED_IMPLIB "${libxmplite_implib}"
                    INTERFACE_INCLUDE_DIRECTORIES "${libxmplite_incdir}"
            )
        endif()
    endif()
else()
    set(libxmplite_sharedlib "${libxmplite_libdir}/${CMAKE_SHARED_LIBRARY_PREFIX}xmp-lite${CMAKE_SHARED_LIBRARY_SUFFIX}")
    if(EXISTS "${libxmplite_sharedlib}")
        set(libxmplite_FOUND ON)
        if(NOT TARGET libxmp-lite::xmp_lite_shared)
            add_library(libxmp-lite::xmp_lite_shared SHARED IMPORTED)
            set_target_properties(libxmp-lite::xmp_lite_shared
                PROPERTIES
                    IMPORTED_LOCATION "${libxmplite_sharedlib}"
                    INTERFACE_INCLUDE_DIRECTORIES "${libxmplite_incdir}"
            )
        endif()
    endif()
endif()

set(libxmplite_staticlib "${libxmplite_libdir}/${CMAKE_STATIC_LIBRARY_PREFIX}xmp-lite${CMAKE_STATIC_LIBRARY_SUFFIX}")
if(EXISTS "${libxmplite_staticlib}")
    set(libxmplite_FOUND ON)
    if(NOT TARGET libxmp-lite::xmp_lite_static)
        add_library(libxmp-lite::xmp_lite_static STATIC IMPORTED)
        set_target_properties(libxmp-lite::xmp_lite_static
            PROPERTIES
                IMPORTED_LOCATION "${libxmplite_staticlib}"
                INTERFACE_INCLUDE_DIRECTORIES "${libxmplite_incdir}"
        )
    endif()
endif()

unset(libxmplite_root)
unset(libxmplite_bindir)
unset(libxmplite_incdir)
unset(libxmplite_libdir)
unset(libxmplite_sharedlib)
unset(libxmplite_implib)
unset(libxmplite_staticlib)
