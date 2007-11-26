PROJECT (PERTUBIS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# choose the DE su tool. For kde take kdesu, for gnome take gksu
SET(PERTUBIS_SU_TOOL
    CACHE STRING
    "use this to determine the desktop su tool")

# should we optionally build the tests? set this to 1 if yes
SET(PERTUBIS_BUILD_TESTS OFF
    CACHE BOOL
    "set this to ON if you want to run the ctest suite")

# should we optionally build the ypi docs? set this to O if yes
SET(PERTUBIS_BUILD_DOCS OFF
    CACHE BOOL
    "set this to ON for generating pertubis api documenation")

SET(PERTUBIS_DOC_PATH /usr/share/doc/pertubis-scm/
    CACHE STRING
    "set this to the appropriate doc path")

# choose the build mode = debug | release
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE release
    CACHE STRING
    "Choose the type of build, options are: Debug Release."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# make the build process more verbose
SET(CMAKE_VERBOSE_MAKEFILE 1)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

INCLUDE(UsePkgConfig)
PKGCONFIG(paludis PALUDIS_INCLUDE_DIR PALUDIS_LINK_DIR PALUDIS_LINK_FLAGS PALUDIS_CFLAGS)

FIND_LIBRARY(PALUDIS_UTIL_LIB NAMES paludisutil
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

SET(PERTUBIS_CFLAGS "$ENV{CXXFLAGS} ${PALUDIS_CFLAGS} -Wall -Wextra -Wold-style-cast -Wredundant-decls -Wstrict-null-sentinel -Wmissing-noreturn -Woverloaded-virtual -Winit-self -Wunused -Wunused-function -Wshadow -Wwrite-strings"
    CACHE INTERNAL
    "combines your CXXFLAGS paludis and pertubis flags"
    FORCE)

SET(CMAKE_CXX_FLAGS_DEBUG "${PERTUBIS_CFLAGS} -ggdb"
    CACHE STRING
    "The compiler flags you want for the debugging target"
    FORCE)

SET(CMAKE_CXX_FLAGS_RELEASE "${PERTUBIS_CFLAGS} -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT"
    CACHE STRING
    "The compiler flags you want for the release target"
    FORCE)

MESSAGE("CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE}\n\n"
        "CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(utils)

IF (PERTUBIS_BUILD_DOCS MATCHES ON)
    ADD_SUBDIRECTORY(doc)
ENDIF (PERTUBIS_BUILD_DOCS MATCHES ON)
if (PERTUBIS_BUILD_TESTS MATCHES ON)
    ADD_SUBDIRECTORY(test)
    ENABLE_TESTING()
ENDIF (PERTUBIS_BUILD_TESTS MATCHES ON)
