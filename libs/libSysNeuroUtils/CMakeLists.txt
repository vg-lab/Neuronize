if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

mark_as_advanced(CMAKE_BUILD_TYPE)

#Boost
find_package (Boost REQUIRED COMPONENTS)

#Code and headers
file (GLOB SysNeuroUtils_src "*.cpp")
file (GLOB SysNeuroUtils_h "*.h")

INCLUDE_DIRECTORIES ( ${Boost_INCLUDE_DIRS} )
											
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR}
					${CMAKE_CURRENT_SOURCE_DIR})

#Construccion de la librería
add_library (SysNeuroUtils SHARED 
    ${SysNeuroUtils_src}
    ${SysNeuroUtils_h}
)

target_link_libraries ( SysNeuroUtils	${Boost_LIBRARIES} )

set_target_properties( SysNeuroUtils
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
