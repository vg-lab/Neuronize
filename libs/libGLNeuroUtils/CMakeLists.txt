source_group(\\ FILES CMakeLists.txt)


set (GLNEUROUTILS_SOURCES
		LightManager.cpp
		MeshRenderer.cpp
		NeuroGraphRenderer.cpp
		OldLODController.cpp
		Shader.cpp
		ShaderManager.cpp
		skybox.cpp
		VBOMeshRenderer.cpp
		)

set(GLNEUROUTILS_HEADERS
		config.h
		GLHelper.hpp
		LightManager.h
		MeshRenderer.h
		NeuroGraphRenderer.h
		OldLODController.h
		Shader.h
		ShaderManager.h
		skybox.h
		VBOMeshRenderer.h
		)

set(GLNEUROUTILS_LINK_LIBRARIES
		SysNeuroUtils
		NeuroUtils
		${GLEW_LIBRARIES}
		${OPENGL_LIBRARIES}
		${GLUT_LIBRARY}
		${OPENMESH_CORE_LIBRARY}
		${OPENMESH_TOOLS_LIBRARY}
		)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/src)

if (MSVC)
	add_definitions(-D_USE_MATH_DEFINES)
endif ()

set( GLNEUROUTILS_INCLUDE_NAME glneuroutils )
set( GLNEUROUTILS_NAMESPACE glneuroutils )


common_library(GLNeuroUtils)

