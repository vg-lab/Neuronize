if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

#Boost
find_package (Boost)

INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS} )

include_directories( ../libs/libSysNeuroUtils )
include_directories( ../libs/libNeuroUtils )
include_directories( ../libs/libGLNeuroUtils )
include_directories( ../libs/libQtNeuroUtils )

file (GLOB neuronize_src "*.cpp")
file (GLOB neuronize_hdr "*.h")

set (neuronize_ui   ${CMAKE_CURRENT_SOURCE_DIR}/neuronize.ui
                    ${CMAKE_CURRENT_SOURCE_DIR}/batchbuilder.ui
                    ${CMAKE_CURRENT_SOURCE_DIR}/NeuroGeneratorWidget.ui
                    ${CMAKE_CURRENT_SOURCE_DIR}/SomaCreatorWidget.ui
                    ${CMAKE_CURRENT_SOURCE_DIR}/SomaDeformerWidget.ui
)

#Luego se genera la aplica
set (MyProjectBin_src ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx)

#Para invocar al MOC
qt5_wrap_cpp(neuronize_hdr_moc ${neuronize_hdr})
qt5_wrap_ui(neuronize_ui_moc  ${neuronize_ui})

SET(UI_RESOURCES neuronize.qrc)
QT5_ADD_RESOURCES(UI_RESOURCES_RCC ${UI_RESOURCES})

include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})

include_directories(${QT_INCLUDES} 
					${CMAKE_CURRENT_BINARY_DIR}
					${CMAKE_CURRENT_SOURCE_DIR} 
					)

#Construccion de la librería
add_library (neuronizeLib SHARED 
    ${neuronize_src}
    ${neuronize_hdr_moc}
    ${neuronize_ui_moc}
    ${UI_RESOURCES_RCC}
)

target_link_libraries ( neuronizeLib  SysNeuroUtils
                                      NeuroUtils
                                      GLNeuroUtils 
                                      QtNeuroUtils 
                                      OpenMeshCore 
                                      OpenMeshTools
                                      ${OPENGL_LIBRARIES} 
                                      ${GLUT_LIBRARY}								
                                      QGLViewer
                                      Qt5::Widgets
                                      Qt5::Xml
                                      Qt5::OpenGL
                                      ${Boost_LIBRARIES}
                                  )
						
#Construcción de la apli
add_executable(neuronize ${MyProjectBin_src} ${UI_RESOURCES_RCC})
target_link_libraries (neuronize neuronizeLib)

set_target_properties( neuronizeLib
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_target_properties( neuronize
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#add_custom_command(TARGET neuronize PRE_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                       ${PROJECT_SOURCE_DIR}/Content ${RUNTIME_OUTPUT_DIRECTORY}/Content)
