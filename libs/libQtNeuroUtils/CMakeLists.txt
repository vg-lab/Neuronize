if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

mark_as_advanced(CMAKE_BUILD_TYPE)

#Code and headers
file (GLOB QtNeuroUtils_src "*.cpp")
file (GLOB QtNeuroUtils_h "*.h")
											
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})

include_directories(${QT_INCLUDES} 
					${CMAKE_CURRENT_BINARY_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}
					)

include_directories(../libSysNeuroUtils)
include_directories(../libNeuroUtils)

add_definitions(${QT_DEFINITIONS})

#Construccion de la librería
add_library (QtNeuroUtils SHARED 
    ${QtNeuroUtils_src}
    ${QtNeuroUtils_h}
)

target_link_libraries ( QtNeuroUtils	
                        SysNeuroUtils															
                        Qt5::Widgets
                        Qt5::Core
                        Qt5::Xml
                      )

set_target_properties( QtNeuroUtils
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
