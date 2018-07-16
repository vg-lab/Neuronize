source_group(\\ FILES CMakeLists.txt)

set (SYSNEUROUTILS_SOURCES
        defs.cpp
        MathUtils.cpp
        randomUtils.cpp
        SystemUtils.cpp
        Timer.cpp
        )

set(SYSNEUROUTILS_HEADERS
        defs.h
        MathUtils.h
        randomUtils.h
        SystemUtils.h
        Timer.h
        )

set(SYSNEUROUTILS_LINK_LIBRARIES
        ${Boost_LIBRARIES}
        )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)

add_definitions("-w")

if (MSVC)
    add_definitions("/W4")
    # Static build
    add_definitions(-DVISHNU3DVIEWER_STATIC)
endif ()



common_library(SysNeuroUtils)
