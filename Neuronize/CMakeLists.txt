source_group(\\ FILES CMakeLists.txt)

set (NEURONIZELIB_SOURCES
		batchbuilder.cpp
		FrameConstrictor.cpp
		manipulatedFrameSetConstraint.cpp
		NeuroGeneratorWidget.cpp
		NeuroGeneratorWidgetViewer.cpp
		neuronize.cpp
		object.cpp
		SomaCreatorWidget.cpp
		SomaDeformerWidget.cpp
		SomaDeformerWidgetViewer.cpp
		LoadFileDialog.cpp
		)

set(NEURONIZELIB_HEADERS
		batchbuilder.h
		batchbuilder.ui
		FrameConstrictor.h
		manipulatedFrameSetConstraint.h
		NeuroGeneratorWidget.h
		NeuroGeneratorWidget.ui
		NeuroGeneratorWidgetViewer.h
		neuronize.h
		neuronize.ui
		object.h
		SomaCreatorWidget.h
		SomaCreatorWidget.ui
		SomaDeformerWidget.h
		SomaDeformerWidget.ui
		SomaDeformerWidgetViewer.h
		LoadFileDialog.h
		xneuron.ui
		xneuron.qrc
		neuronize.qrc
		)

set(NEURONIZELIB_LINK_LIBRARIES
		SysNeuroUtils
		NeuroUtils
		GLNeuroUtils
		QtNeuroUtils
		${OPENMESH_CORE_LIBRARY}
		${OPENMESH_TOOLS_LIBRARY}
		${OPENGL_LIBRARIES}
		${GLUT_LIBRARY}
		QGLViewer
		Qt5::Widgets
		Qt5::Xml
		Qt5::OpenGL
		${Boost_LIBRARIES}
		skelgeneratorutil
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)
include_directories(VCG_INCLUDE_DIR)

add_definitions("-w")

if (MSVC)
	add_definitions("/W4")
	# Static build
	add_definitions(-DVISHNU3DVIEWER_STATIC)
	add_definitions(-D_USE_MATH_DEFINES)
endif ()

common_library(neuronizelib)


source_group(\\ FILES CMakeLists.txt)

set (NEURONIZE_SOURCES
		main.cxx
		)

set(NEURONIZE_HEADERS
		)

set(NEURONIZE_LINK_LIBRARIES
		neuronizelib
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)

add_definitions("-w")

if (MSVC)
	add_definitions("/W4")
	# Static build
	add_definitions(-DVISHNU3DVIEWER_STATIC)
endif ()

common_application(neuronize)