#
# Try to find SOIL library and include path.
# Once done this will define
#
# SOIL_FOUND
# SOIL_INCLUDE_PATH
# SOIL_LIBRARY
#

IF(WIN32)
    FIND_PATH( SOIL_INCLUDE_PATH SOIL/SOIL.h
            $ENV{PROGRAMFILES}/SOIL/include
            ${SOIL_ROOT_DIR}/include
            $ENV{SOIL_ROOT_DIR}/include
            DOC "The directory where SOIL/SOIL.h resides")
    FIND_LIBRARY( SOIL_LIBRARY
            NAMES SOIL
            PATHS
            $ENV{PROGRAMFILES}/SOIL/lib
            ${SOIL_ROOT_DIR}/lib
            $ENV{SOIL_ROOT_DIR}/lib
            $ENV{SOIL_ROOT_DIR}/x64/Release
            DOC "The SOIL library")
ELSE(WIN32)
    FIND_PATH( SOIL_INCLUDE_PATH SOIL/SOIL.h
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            ${SOIL_ROOT_DIR}/include
            DOC "The directory where SOIL/SOIL.h resides")

    # Prefer the static library.
    FIND_LIBRARY( SOIL_LIBRARY
            NAMES libSOIL.a SOIL
            PATHS
            /usr/lib64
            /usr/lib
            /usr/local/lib64
            /usr/local/lib
            /sw/lib
            /opt/local/lib
            ${SOIL_ROOT_DIR}/lib
            DOC "The SOIL library")
ENDIF(WIN32)

SET(SOIL_FOUND "NO")
IF(SOIL_INCLUDE_PATH AND SOIL_LIBRARY)
    SET(SOIL_INCLUDE_DIRS ${SOIL_INCLUDE_PATH})
    SET(SOIL_LIBRARIES ${SOIL_LIBRARY})
    SET(SOIL_FOUND "YES")
    message(STATUS "Found SOIL")
ENDIF(SOIL_INCLUDE_PATH AND SOIL_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SOIL DEFAULT_MSG SOIL_LIBRARIES SOIL_INCLUDE_DIRS)