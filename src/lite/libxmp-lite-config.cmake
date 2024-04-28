set(libxmp-lite_FOUND OFF)

if(EXISTS "${CMAKE_CURRENT_LISTDIR}/libxmp-lite-shared-targets.cmake")
    include("${CMAKE_CURRENT_LISTDIR}/libxmp-lite-shared-targets.cmake")
    set(libxmp-lite_FOUND ON)
endif()

if(EXISTS "${CMAKE_CURRENT_LISTDIR}/libxmp-lite-static-targets.cmake")
    include("${CMAKE_CURRENT_LISTDIR}/libxmp-lite-static-targets.cmake")
    set(libxmp-lite_FOUND ON)
endif()
