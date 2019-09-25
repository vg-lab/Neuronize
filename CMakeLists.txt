
cmake_minimum_required( VERSION 3.1 )
project(Neuronize VERSION 0.2.0 )
set( neuronize_VERSION_ABI 1 )
# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif( )

if( NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake )
    message( FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif( )

if( NOT EXISTS ${CMAKE_SOURCE_DIR}/VCG/vcg/complex/all_types.h )
    message( FATAL_ERROR "VCG missing, run: git submodule update --init")
endif( )

set (VCG_DIR "${CMAKE_SOURCE_DIR}/VCG")

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

if( APPLE )
  set( CMAKE_CXX_FLAGS "-framework CoreFoundation" )
endif()

include( Common )

common_find_package(VCG REQUIRED)
common_find_package(Boost COMPONENTS filesystem REQUIRED)
common_find_package(GLEW REQUIRED)
common_find_package(OpenGL SYSTEM REQUIRED)
common_find_package(GLUT SYSTEM REQUIRED)
common_find_package(Qt5Widgets SYSTEM REQUIRED)
common_find_package(Qt5Core SYSTEM REQUIRED)
common_find_package(Qt5Gui SYSTEM REQUIRED)
common_find_package(Qt5OpenGL SYSTEM REQUIRED)
common_find_package(Qt5Xml SYSTEM REQUIRED)
common_find_package(GLM SYSTEM REQUIRED)
common_find_package(Eigen3 SYSTEM REQUIRED)
common_find_package(SkelGenerator SYSTEM REQUIRED)
common_find_package(SOIL REQUIRED)
common_find_package(OpenMesh SYSTEM REQUIRED)
common_find_package(QGLViewer SYSTEM REQUIRED)
common_find_package(SQLite3 REQUIRED)
common_find_package_post( )

list( APPEND NEURONIZE_DEPENDENT_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::OpenGL
        Qt5::Xml
        ${GLEW_LIBRARIES}
        GLUT
        OpenGL
        ${Boost_LIBRARIES}
        SkelGenerator
        VCG
        glm
        ${OPENMESH_LIBRARY}
        QGLViewer
        SOIL
        ${SQLite3_LIBRARIES}
        )

if( NOT MSVC )
  add_definitions("-w")
endif()

add_subdirectory( libs/libSysNeuroUtils )
add_subdirectory( libs/libNeuroUtils )
add_subdirectory( libs/libGLNeuroUtils )
add_subdirectory( libs/libQtNeuroUtils )
add_subdirectory( Neuronize )


if( MSVC )
  # Set DataCooking as startup project on Visual Studio, CMake v3.6
  if( CMAKE_VERSION VERSION_GREATER 3.6 )
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT neuronize)
  endif( )
endif( )

set( DOXYGEN_MAINPAGE_MD README.md )
set( DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md )
include( DoxygenRule )

include( CPackConfig )


#Copy content (this must be improved)

file(GLOB MY_CONTENT "Content/*")
file(COPY ${MY_CONTENT} DESTINATION "${CMAKE_BINARY_DIR}/bin")

file(GLOB MeshReconstruct "MeshReconstruct/src/*")
file(COPY ${MeshReconstruct} DESTINATION "${CMAKE_BINARY_DIR}/bin/src"  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)



