if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

mark_as_advanced(CMAKE_BUILD_TYPE)

#Code and headers
file (GLOB GLNeuroUtils_src "*.cpp")
file (GLOB GLNeuroUtils_h "*.h")
											
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}
					)

include_directories(../libSysNeuroUtils)
include_directories(../libNeuroUtils)

#Construccion de la librería
add_library (GLNeuroUtils SHARED 
    ${GLNeuroUtils_src}
    ${GLNeuroUtils_h}
)

target_link_libraries ( GLNeuroUtils	SysNeuroUtils
										NeuroUtils
										SOIL
										${GLEW_LIBRARIES}
										${OPENGL_LIBRARIES} 
										${GLUT_LIBRARY}
						)

set_target_properties( GLNeuroUtils
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
