get_filename_component(libxmp_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
get_filename_component(libxmp_libdir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(libxmp_bindir "${libxmp_root}/bin")
set(libxmp_incdir "${libxmp_root}/include")

set(libxmp_FOUND OFF)

if(WIN32)
    set(libxmp_sharedlib "${libxmp_bindir}/${CMAKE_SHARED_LIBRARY_PREFIX}xmp${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(libxmp_implib "${libxmp_libdir}/${CMAKE_STATIC_LIBRARY_PREFIX}xmp${CMAKE_SHARED_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    if(EXISTS "${libxmp_sharedlib}" AND EXISTS "${libxmp_implib}")
        set(libxmp_FOUND ON)
        if(NOT TARGET libxmp::xmp_shared)
            add_library(libxmp::xmp_shared SHARED IMPORTED)
            set_target_properties(libxmp::xmp_shared
                PROPERTIES
                    IMPORTED_LOCATION "${libxmp_sharedlib}"
                    IMPORTED_IMPLIB "${libxmp_implib}"
                    INTERFACE_INCLUDE_DIRECTORIES "${libxmp_incdir}"
            )
        endif()
    endif()
else()
    set(libxmp_sharedlib "${libxmp_libdir}/${CMAKE_SHARED_LIBRARY_PREFIX}xmp${CMAKE_SHARED_LIBRARY_SUFFIX}")
    if(EXISTS "${libxmp_sharedlib}")
        set(libxmp_FOUND ON)
        if(NOT TARGET libxmp::xmp_shared)
            add_library(libxmp::xmp_shared SHARED IMPORTED)
            set_target_properties(libxmp::xmp_shared
                PROPERTIES
                    IMPORTED_LOCATION "${libxmp_sharedlib}"
                    INTERFACE_INCLUDE_DIRECTORIES "${libxmp_incdir}"
            )
        endif()
    endif()
endif()

set(libxmp_staticlib "${libxmp_libdir}/${CMAKE_STATIC_LIBRARY_PREFIX}xmp${CMAKE_STATIC_LIBRARY_SUFFIX}")
if(EXISTS "${libxmp_staticlib}")
    set(libxmp_FOUND ON)
    if(NOT TARGET libxmp::xmp_static)
        add_library(libxmp::xmp_static STATIC IMPORTED)
        set_target_properties(libxmp::xmp_static
            PROPERTIES
                IMPORTED_LOCATION "${libxmp_staticlib}"
                INTERFACE_INCLUDE_DIRECTORIES "${libxmp_incdir}"
        )
    endif()
endif()

unset(libxmp_root)
unset(libxmp_bindir)
unset(libxmp_incdir)
unset(libxmp_libdir)
unset(libxmp_sharedlib)
unset(libxmp_implib)
unset(libxmp_staticlib)
