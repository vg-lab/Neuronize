source_group(\\ FILES CMakeLists.txt)

set (QTNEUROUTILS_SOURCES
		CriterionTypes.cpp
		libqtneuroutils.cpp
		NeuroDistGenerator.cpp
		QtIndexedPalette.cpp
		StateControlTypes.cpp
		XMLProgressiveNeuroViz.cpp
		XMLProgressiveNeuroVizCriterion.cpp
		XMLProgressiveNeuroVizManager.cpp
		XMLSomaDef.cpp
		XMLSomaDefDendriticModif.cpp
		XMLSomaDefManager.cpp
		
		)

set(LQTNEUROUTILS_HEADERS
		CriterionTypes.h
		libqtneuroutils.h
		NeuroDistGenerator.h
		QtIndexedPalette.h
		QtThreadsManager.hpp
		StateControlTypes.h
		XMLProgressiveNeuroViz.h
		XMLProgressiveNeuroVizCriterion.h
		XMLProgressiveNeuroVizManager.h
		XMLSomaDef.h
		XMLSomaDefDendriticModif.h
		XMLSomaDefManager.h
		
		)

set(QTNEUROUTILS_LINK_LIBRARIES
		SysNeuroUtils
		Qt5::Widgets
		Qt5::Core
		Qt5::Xml
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)

set( QTNEUROUTILS_INCLUDE_NAME qtneuroutils )
set( QTNEUROUTILS_NAMESPACE qtneuroutils )

common_library(QtNeuroUtils)
