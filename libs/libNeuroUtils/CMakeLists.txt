if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

mark_as_advanced(CMAKE_BUILD_TYPE)

#Boost
find_package (Boost)

#Code and headers
file (GLOB NeuroUtils_src "*.cpp")
file (GLOB NeuroUtils_h "*.h")
											
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})

INCLUDE_DIRECTORIES ( ${Boost_INCLUDE_DIRS} )

include_directories(
					${CMAKE_CURRENT_BINARY_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}
					)

include_directories(../libSysNeuroUtils)

#Construccion de la librería
add_library (NeuroUtils SHARED 
            ${NeuroUtils_src}
            ${NeuroUtils_h})

target_link_libraries ( NeuroUtils	SysNeuroUtils	${Boost_LIBRARIES} )

set_target_properties( NeuroUtils
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
