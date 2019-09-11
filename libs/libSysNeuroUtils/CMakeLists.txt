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

set( SYSNEUROUTILS_INCLUDE_NAME sysneuroutils )
set( SYSNEUROUTILS_NAMESPACE sysneuroutils )

common_library(SysNeuroUtils)
