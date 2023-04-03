set(libxmp_FOUND OFF)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/libxmp-shared-targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/libxmp-shared-targets.cmake")
    set(libxmp_FOUND ON)
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/libxmp-static-targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/libxmp-static-targets.cmake")
    set(libxmp_FOUND ON)
endif()
