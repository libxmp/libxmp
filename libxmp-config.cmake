set(libxmp_FOUND OFF)

if(EXISTS "${CMAKE_CURRENT_LISTDIR}/libxmp-shared-targets.cmake")
    include("${CMAKE_CURRENT_LISTDIR}/libxmp-shared-targets.cmake")
    set(libxmp_FOUND ON)
endif()

if(EXISTS "${CMAKE_CURRENT_LISTDIR}/libxmp-static-targets.cmake")
    include("${CMAKE_CURRENT_LISTDIR}/libxmp-static-targets.cmake")
    set(libxmp_FOUND ON)
endif()
