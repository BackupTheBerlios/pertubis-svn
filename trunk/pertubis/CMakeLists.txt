PROJECT (PERTUBIS)

# comment out the line below to make the build process more verbose
# SET(CMAKE_VERBOSE_MAKEFILE 1)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# SET(CMAKE_BUILD_TYPE Release)

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

MESSAGE("CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")

# here we combine system provided compiler flags with our ones
SET(PERTUBIS_FLAGS $ENV{CXXFLAGS})
SET(CMAKE_CXX_FLAGS "${PERTUBIS_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra -D_REENTRANT -DQT_SHARED -Wextra -Wold-style-cast -Wredundant-decls -Wstrict-null-sentinel -Wmissing-noreturn -Woverloaded-virtual -Winit-self -Wunused -Wunused-function -Wshadow -Wwrite-strings")
SET(CMAKE_CXX_FLAGS_DEBUG "${PERTUBIS_FLAGS} -Wall -Wextra -g -D_REENTRANT -DQT_SHARED")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "${PERTUBIS_FLAGS} -Os -pipe -fvisibility=hidden -Wall -Wextra -D_REENTRANT -DQT_SHARED")
SET(CMAKE_CXX_FLAGS_RELEASE "${PERTUBIS_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -D_REENTRANT -DQT_SHARED -DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT")

ADD_SUBDIRECTORY(src)
