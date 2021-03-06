source_group(\\ FILES CMakeLists.txt)

set (NEURONIZE_SOURCES
		FrameConstrictor.cpp
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
        RepairWidget.cpp
		CompareMeshesWidget.cpp
		CompareMeshWidgetViewer.cpp
		ExportDialog.cpp
		WizzardInputOne.cpp
		NeuronSetDialog.cpp
		SelectSpinesDialog.cpp
		)

set(NEURONIZE_HEADERS
		FrameConstrictor.h
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
        RepairWidget.h
		CompareMeshesWidget.h
		ExportDialog.h
		CompareMeshWidgetViewer.h
		WizzardInputOne.h
		NeuronSetDialog.h
		SelectSpinesDialog.h
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
		${CMAKE_DL_LIBS}
		${Boost_LIBRARIES}
		skelgeneratorutil
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)
include_directories(VCG_INCLUDE_DIR)

if ( APPLE )
  set( NEURONIZE_ICON neuronize.icns )
endif()

if ( UNIX AND NOT APPLE )
  set( NEURONIZE_ICON icons/neuronize.png )
endif( )


if (MSVC)
	add_definitions(-DNEURONIZE_STATIC)
	add_definitions(-D_USE_MATH_DEFINES)
	common_application(neuronize GUI WIN32)
else(MSVC)
	common_application(neuronize GUI)
endif ()
