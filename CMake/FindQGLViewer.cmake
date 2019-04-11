FIND_PACKAGE(Qt5 COMPONENTS Core Xml OpenGL Gui Widgets)
IF(NOT Qt5_FOUND)
    MESSAGE("Qt5 not found. Install it and set Qt5_DIR accordingly")
    IF (WIN32)
        MESSAGE("  In Windows, Qt5_DIR should be something like C:/Qt/5.4/msvc2013_64_opengl/lib/cmake/Qt5")
    ENDIF()
ENDIF()
FIND_PATH(QGLVIEWER_INCLUDE_DIR QGLViewer/qglviewer.h
        /usr/include/QGLViewer
        /opt/local/include/QGLViewer
        /usr/local/include/QGLViewer
        /sw/include/QGLViewer
        $ENV{QGLVIEWERROOT}
        $ENV{QGLVIEWERROOT}/usr/local/include
        $ENV{QGLVIEWERROOT}/usr/local/include/QGLViewer.framework/Headers
        )

find_library(QGLVIEWER_LIBRARY_RELEASE
        NAMES qglviewer QGLViewer qglviewer-qt5 QGLViewer-qt5 QGLViewer2
        PATHS /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ENV QGLVIEWERROOT
        ENV LD_LIBRARY_PATH
        ENV LIBRARY_PATH
        PATH_SUFFIXES x64/Release QGLViewer QGLViewer/release
        )

find_library(QGLVIEWER_LIBRARY_DEBUG
        NAMES dqglviewer dQGLViewer dqglviewer-qt5 dQGLViewer-qt5 QGLViewerd2
        PATHS /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ENV QGLVIEWERROOT
        ENV LD_LIBRARY_PATH
        ENV LIBRARY_PATH
        PATH_SUFFIXES x64/Debug QGLViewer QGLViewer/debug
        )

if(QGLVIEWER_LIBRARY_RELEASE)
    if(QGLVIEWER_LIBRARY_DEBUG)
        set(QGLVIEWER_LIBRARY optimized ${QGLVIEWER_LIBRARY_RELEASE} debug ${QGLVIEWER_LIBRARY_DEBUG})
    else()
        set(QGLVIEWER_LIBRARY ${QGLVIEWER_LIBRARY_RELEASE})
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QGLViewer
        "QGLViewer NOT FOUND. Please provide QGLVIEWERROOT (via cmake or env. var)"
        QGLVIEWER_INCLUDE_DIR QGLVIEWER_LIBRARY)