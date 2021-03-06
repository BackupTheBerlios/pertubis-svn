
include(${QT_USE_FILE})

SET(PERTUBIS_SRCS
	ArchModel.cc
	CategoryModel.cc
	DatabaseView.cc
	description_extractor.cc
	extractor.cc
	main.cc

	matcher.cc
	name_extractor.cc
	OptionsDelegate.cc
	PackageItem.cc
	PackageModel.cc
	pcre_matcher.cc
	SearchWindow.cc
	Settings.cc
	TaskBox.cc
	text_matcher.cc
	ThreadBase.cc
	ThreadFetchCategories.cc
	ThreadFetchDetails.cc
	ThreadFetchItem.cc
	ThreadFetchPackages.cc
	ThreadKeywordManager.cc
	UseFlagEditor.cc
	UseFlagModel.cc
)

SET(PERTUBIS_MOC_HDRS
	CategoryModel.hh
	DatabaseView.hh

	OptionsDelegate.hh
	PackageItem.hh
	PackageModel.hh
	SearchWindow.hh
	Settings.hh
	TaskBox.hh
	ThreadBase.hh
	ThreadFetchCategories.hh
	ThreadFetchDetails.hh
	ThreadFetchItem.hh
	ThreadFetchPackages.hh
	ThreadKeywordManager.hh
	UseFlagEditor.hh
	UseFlagModel.hh
)

EXEC_PROGRAM("/bin/sh"
			ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../utils/scripts/gen_qresource.sh"
			"${CMAKE_CURRENT_SOURCE_DIR}/../"
			OUTPUT_VARIABLE	PERTUBIS_RCCS)

FIND_LIBRARY(PCRE_LIB NAMES pcre++
	PATHS  /usr/lib /usr/local/lib)

FIND_LIBRARY(PCRE_LIB NAMES pcre++
	PATHS  /usr/lib /usr/local/lib)

FIND_LIBRARY(PALUDIS_LIB NAMES paludis
	PATHS  /usr/lib /usr/local/lib)

FIND_LIBRARY(PALUDIS_ENV_LIB NAMES paludisenvironments
	PATHS  /usr/lib /usr/local/lib)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )

QT4_ADD_RESOURCES(PERTUBIS_RCCS_SRCS ${PERTUBIS_RCCS} )

ADD_EXECUTABLE (pertubis ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} ${PERTUBIS_RCCS_SRCS} )

TARGET_LINK_LIBRARIES(pertubis ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
INSTALL(TARGETS pertubis DESTINATION /usr/sbin)
INSTALL(FILES ../utils/desktop-integration/pertubis.desktop DESTINATION /usr/share/applnk/Settings/System)
INSTALL(FILES ../utils/icons/16x16/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/16x16/apps)
INSTALL(FILES ../utils/icons/22x22/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/22x22/apps)
INSTALL(FILES ../utils/icons/32x32/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/32x32/apps)
INSTALL(FILES ../utils/icons/48x48/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/48x48/apps)
INSTALL(FILES ../utils/icons/64x64/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/64x64/apps)
INSTALL(FILES ../utils/icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
INSTALL(FILES ../utils/icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
