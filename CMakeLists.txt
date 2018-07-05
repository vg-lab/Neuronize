cmake_minimum_required (VERSION 3.2)
project (Neuronize)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

#OpenGL
find_package(GLEW REQUIRED)
include_directories($(GLEW_INCLUDE_DIRS))
link_directories(${GLEW_LIBRARY_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})

find_package(GLUT REQUIRED) 
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})

#Qt
find_package (Qt5Widgets)
find_package (Qt5Core)
find_package (Qt5Gui)
find_package (Qt5OpenGL)
find_package (Qt5Xml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC ")
set(CMAKE_AUTORCC ON)

#Build libraries
add_subdirectory( libs/libSysNeuroUtils )
add_subdirectory( libs/libNeuroUtils )
add_subdirectory( libs/libGLNeuroUtils )
add_subdirectory( libs/libQtNeuroUtils )

#Build tools
add_subdirectory( Neuronize )

#Copy content (this must be improved)
file(GLOB MY_CONTENT "Content/*")
file(COPY ${MY_CONTENT} DESTINATION "${CMAKE_BINARY_DIR}/bin")
