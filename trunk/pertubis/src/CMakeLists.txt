
include(${QT_USE_FILE})

SET(PERTUBIS_SRCS
	matcher.cc
	text_matcher.cc
	pcre_matcher.cc
	description_extractor.cc
	name_extractor.cc
	ArchModel.cc
	extractor.cc
	CategoryModel.cc
	PackageItem.cc
	KeywordManager.cc
	PackageDetails.cc
	PackageModel.cc
	SearchWindow.cc
	DatabaseView.cc
	MainWindow.cc
	Threads.cc
	Workspace.cc
	TaskBox.cc
	OptionsDelegate.cc
	main.cc
# 	UseFlagManager.cc
)

SET(PERTUBIS_MOC_HDRS
	CategoryModel.hh
	PackageDetails.hh
	PackageItem.hh
	OptionsDelegate.hh
	TaskBox.hh
	PackageModel.hh
	DatabaseView.hh
	MainWindow.hh
	SearchWindow.hh
	Workspace.hh
	Threads.hh
# 	UseFlagManager.hh
)

SET( ../utils/images/picons/pertubis.qrc)

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
INSTALL(FILES ../images/icons/16x16/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/16x16/apps)
INSTALL(FILES ../images/icons/22x22/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/22x22/apps)
INSTALL(FILES ../images/icons/32x32/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/32x32/apps)
INSTALL(FILES ../images/icons/48x48/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/48x48/apps)
INSTALL(FILES ../images/icons/64x64/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/64x64/apps)
INSTALL(FILES ../images/icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
INSTALL(FILES ../images/icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
