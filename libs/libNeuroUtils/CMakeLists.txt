source_group(\\ FILES CMakeLists.txt)

set (NEUROUTILS_SOURCES
		AABB.cpp
		ASC2SWC.cpp
		BaseMesh.cpp
		BaseMeshContainer.cpp
		BSphere.cpp
		DeformableObject.cpp
		LecturaFichero.cpp
		MSNode.cpp
		MSObject.cpp
		MSSpring.cpp
		MSSpringConstraint.cpp
		NeuroLevelVizNode.cpp
		NeuroLevelVizTable.cpp
		NeuronGraph.cpp
		NeuronGraphOptimizer.cpp
		NeuroSWC.cpp
		ProceduralMesh.cpp
		SpinesSWC.cpp
		SWCImporter.cpp
		SWCSpinesDistributor.cpp
		)

set(NEUROUTILS_HEADERS
		AABB.h
		ASC2SWC.h
		BaseMesh.h
		BaseMeshContainer.h
		BSphere.h
		CriterionsEvaluators.hpp
		DeformableObject.h
		glmSpline.hpp
		imppsimpl.h
		LecturaFichero.h
		MeshDef.hpp
		MSNode.h
		MSObject.h
		MSSpring.h
		MSSpringConstraint.h
		NeuroLevelVizNode.h
		NeuroLevelVizTable.h
		NeuronGraph.h
		NeuronGraphOptimizer.h
		NeuroSWC.h
		NodoParseado.h
		ProceduralMesh.h
		psimpl.h
		SpinesSWC.h
		Subdivider.hpp
		SWCImporter.h
		SWCSpinesDistributor.h
		)

set(NEUROUTILS_LINK_LIBRARIES
		SysNeuroUtils
		${Boost_LIBRARIES}
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)

add_definitions("-w")

if (MSVC)
	add_definitions("/W4")
	# Static build
	add_definitions(-DVISHNU3DVIEWER_STATIC)
endif ()


common_library(NeuroUtils)