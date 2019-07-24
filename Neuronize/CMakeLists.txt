source_group(\\ FILES CMakeLists.txt)

set (NEURONIZE_SOURCES
		batchbuilder.cpp
		FrameConstrictor.cpp
		LoadFileDialog.cpp
		main.cxx
		manipulatedFrameSetConstraint.cpp
		NeuroGeneratorWidget.cpp
		NeuroGeneratorWidget.cpp
		NeuroGeneratorWidgetViewer.cpp
		neuronize.cpp
		object.cpp
		SomaCreatorWidget.cpp
		SomaDeformerWidget.cpp
		SomaDeformerWidgetViewer.cpp
		)

set(NEURONIZE_HEADERS
		batchbuilder.h
		batchbuilder.ui
		FrameConstrictor.h
		LoadFileDialog.h
		manipulatedFrameSetConstraint.h
		NeuroGeneratorWidget.h
		NeuroGeneratorWidget.ui
		NeuroGeneratorWidgetViewer.h
		neuronize.h
		neuronize.qrc
		neuronize.ui
		object.h
		SomaCreatorWidget.h
		SomaCreatorWidget.ui
		SomaDeformerWidgetViewer.h
		xneuron.qrc
		xneuron.ui
		)

set(NEURONIZE_LINK_LIBRARIES
		SysNeuroUtils
		NeuroUtils
		GLNeuroUtils
		QtNeuroUtils
		${OPENMESH_LIBRARIES}
		${OPENGL_LIBRARIES}
		${GLUT_LIBRARY}
		${QGLVIEWER_LIBRARY}
		Qt5::Widgets
		Qt5::Xml
		Qt5::OpenGL
		${Boost_LIBRARIES}
		skelgeneratorutil
		${SQLite3_LIBRARIES}
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)
include_directories(VCG_INCLUDE_DIR)


add_definitions("-w")

if ( APPLE )
  set( NEURONIZE_ICON neuronize.icns )
endif()

if ( UNIX AND NOT APPLE )
  set( NEURONIZE_ICON icons/neuronize.png )
endif( )


if (MSVC)
	add_definitions("/W4")
	# Static build
	add_definitions(-DNEURONIZE_STATIC)
	add_definitions(-D_USE_MATH_DEFINES)
	common_application(neuronize GUI WIN32)
else(MSVC)
	common_application(neuronize GUI)
endif ()
