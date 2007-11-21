PROJECT (PERTUBIS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# choose the DE su tool. For kde take kdesu, for gnome take gksu
SET(PERTUBIS_SU_TOOL
    CACHE STRING
    "use this to determine the desktop su tool")

# should we optionally build the tests? set this to 1 if yes
SET(PERTUBIS_BUILD_TESTS 0
    CACHE STRING
    "set this to 1 if you want to run the ctest suite")

# choose the build mode = Debug | Release
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release
    CACHE STRING
    "Choose the type of build, options are: Debug Release."
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF (CAKE_BUILD_TYPE MATCHES "debug")
	SET(CMAKE_BUILD_TYPE Debug)
ENDIF (CAKE_BUILD_TYPE MATCHES "debug")

# make the build process more verbose
# IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(CMAKE_VERBOSE_MAKEFILE 1)
# ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

# MESSAGE("CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")

INCLUDE(UsePkgConfig)
# SET( ENV{PKG_CONFIG_PATH} ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig )
PKGCONFIG(paludis PALUDIS_INCLUDE_DIR PALUDIS_LINK_DIR PALUDIS_LINK_FLAGS PALUDIS_CFLAGS)

# FIND_LIBRARY(PCRE_LIB NAMES pcre++
# 	PATHS  /usr/lib /usr/local/lib)

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

SET(CMAKE_CXX_FLAGS_DEBUG "${PERTUBIS_CFLAGS} -g"
    CACHE STRING
    "The compiler flags you want for the debugging target"
    FORCE)

SET(CMAKE_CXX_FLAGS_RELEASE "${PERTUBIS_CFLAGS} -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT"
    CACHE STRING
    "The compiler flags you want for the release target"
    FORCE)

FIND_PATH(MODELTEST_DIR modeltest/modeltest.h
    PATHS ${CMAKE_CURRENT_SOURCE_DIR})

ADD_SUBDIRECTORY(src)
if (PERTUBIS_BUILD_TESTS MATCHES 1)
	ADD_SUBDIRECTORY(test)
	ENABLE_TESTING()
ENDIF (PERTUBIS_BUILD_TESTS MATCHES 1)
