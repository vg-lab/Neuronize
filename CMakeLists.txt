cmake_minimum_required( VERSION 3.1 )
set( PROJECT_VERSION_MAJOR "0" )
set( PROJECT_VERSION_MINOR "0" )
set( PROJECT_VERSION_PATCH "1" )
set( PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )
project( Neuronize VERSION ${PROJECT_VERSION} )
set( neuronize_VERSION_ABI 1 )
# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif( )

list( APPEND CMAKE_MODULE_PATH
        ${PROJECT_SOURCE_DIR}/CMake
        ${CMAKE_SOURCE_DIR}/CMake/common
        )

# Default build type is Debug
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif( NOT CMAKE_BUILD_TYPE )

if ( CMAKE_BUILD_TYPE MATCHES "Debug" )
  add_definitions( -DDEBUG )
endif( )

include( GitExternal )

set( NEURONIZE_DESCRIPTION "Neuronize" )
set( COMMON_PROJECT_DOMAIN gmrv.es )

include( Common )


common_find_package( VCG REQUIRED )
common_find_package(Boost REQUIRED)
common_find_package(GLEW REQUIRED)
common_find_package(OpenGL REQUIRED)
common_find_package(GLUT REQUIRED)
common_find_package(Qt5Widgets REQUIRED)
common_find_package(Qt5Core REQUIRED)
common_find_package(Qt5Gui REQUIRED)
common_find_package(Qt5OpenGL REQUIRED)
common_find_package(Qt5Xml REQUIRED)
common_find_package(Eigen3 SYSTEM REQUIRED)
common_find_package(SkelGenerator REQUIRED )
common_find_package_post( )

list( APPEND NEURONIZE_DEPENDENT_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::OpenGL
        Qt5::Xml
        GLEW
        GLUT
        OpenGL
        ${Boost_LIBRARIES}
        SkelGenerator
        VCG
        )

add_subdirectory( libs/libSysNeuroUtils )
add_subdirectory( libs/libNeuroUtils )
add_subdirectory( libs/libGLNeuroUtils )
add_subdirectory( libs/libQtNeuroUtils )
add_subdirectory( Neuronize )



if( MSVC )
  # Set DataCooking as startup project on Visual Studio, CMake v3.6
  if( CMAKE_VERSION VERSION_GREATER 3.6 )
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DataCooking )
  endif( )
endif( )

set( DOXYGEN_MAINPAGE_MD README.md )
set( DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md )
include( DoxygenRule )

include( CPackConfig )


#Copy content (this must be improved)
file(GLOB MY_CONTENT "Content/*")
file(COPY ${MY_CONTENT} DESTINATION "${CMAKE_BINARY_DIR}/bin")
