PROJECT (PERTUBIS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

SET(PERTUBIS_REVISION_NUMBER CACHE STRING "")
EXECUTE_PROCESS(COMMAND svn info
            COMMAND grep "Revision: "
            COMMAND sed -e "s/Revision: //"
            WORKING_DIRECTORY ${PERTUBIS_SOURCE_DIR}
            OUTPUT_VARIABLE  PERTUBIS_REVISION_NUMBER
            OUTPUT_STRIP_TRAILING_WHITESPACE)

# make the build process more verbose
SET(CMAKE_VERBOSE_MAKEFILE 1)

# choose the DE su tool. For kde take kdesu, for gnome take gksu
SET(PERTUBIS_SU_TOOL
    CACHE STRING
    "use this to determine the desktop su tool")

SET(PERTUBIS_DOC_PATH /usr/share/doc/perturbis-scm/
    CACHE STRING
    "set this to the appropriate doc path")

# choose the build mode = debug | release
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE release
    CACHE STRING
    "Choose the type of build, options are: debug release."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


SET(QT_USE_QTXML TRUE)
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})

INCLUDE(UsePkgConfig)
PKGCONFIG(paludis PALUDIS_INCLUDE_DIR PALUDIS_LINK_DIR PALUDIS_LINK_FLAGS PALUDIS_CFLAGS)

FIND_LIBRARY(PALUDIS_UTIL_LIB NAMES paludisutil
            PATHS  ${PALUDIS_LINK_DIR}
            NO_DEFAULT_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH)

FIND_LIBRARY(PALUDIS_ARGS_LIB NAMES paludisargs
            PATHS  ${PALUDIS_LINK_DIR}
            NO_DEFAULT_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH)

FIND_LIBRARY(PALUDIS_LIB NAMES paludis
            PATHS  ${PALUDIS_LINK_DIR}
            NO_DEFAULT_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH)

SET(PERTUBIS_CFLAGS "${PALUDIS_CFLAGS} -Wall -Wextra -Wold-style-cast -Wredundant-decls -Wstrict-null-sentinel -Wmissing-noreturn -Woverloaded-virtual -Winit-self -Wunused -Wunused-function -Wshadow -Wwrite-strings"
    CACHE INTERNAL
    "combines your CXXFLAGS paludis and perturbis flags"
    FORCE)

SET(CMAKE_CXX_FLAGS_DEBUG "${PERTUBIS_CFLAGS} -ggdb"
    CACHE STRING
    "The compiler flags you want for the debugging target"
    FORCE)

SET(CMAKE_CXX_FLAGS_RELEASE "${PERTUBIS_CFLAGS} -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT"
    CACHE STRING
    "The compiler flags you want for the release target"
    FORCE)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(doc)
# ADD_SUBDIRECTORY(test)
