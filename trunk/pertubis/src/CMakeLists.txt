
INCLUDE(${QT_USE_FILE})


SET(PERTUBIS_SRCS
    CategoryModel.cc
    DatabaseView.cc
    DeinstallTask.cc
    description_extractor.cc
    extractor.cc
    InstallTask.cc
    QTOutputStream.hh
    Item.cc
    matcher.cc
    MessageOutput.cc
    name_extractor.cc
    OptionsDelegate.cc
    PackageItem.cc
    PackageModel.cc
    pcre_matcher.cc
    SearchWindow.cc
    Settings.cc
    TaskBox.cc
    Task.cc
    text_matcher.cc
    SyncTask.cc
    ThreadBase.cc
    ThreadFetchCategories.cc
    ThreadFetchDetails.cc
    ThreadFetchItem.cc
    ThreadFetchPackages.cc
    ThreadShowSelections.cc
    VersionItem.cc)

SET(PERTUBIS_MAIN_SRCS
 	main.cc)

SET(PERTUBIS_MOC_HDRS
    CategoryModel.hh
    DatabaseView.hh
    DeinstallTask.hh
    InstallTask.hh
    Item.hh
    MessageOutput.hh
    OptionsDelegate.hh
    PackageItem.hh
    PackageModel.hh
    SearchWindow.hh
    Settings.hh
    TaskBox.hh
    Task.hh
    ThreadBase.hh
    ThreadFetchCategories.hh
    ThreadFetchDetails.hh
    ThreadFetchItem.hh
    ThreadShowSelections.hh
    ThreadFetchPackages.hh
    VersionItem.hh)

EXEC_PROGRAM("/bin/sh"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../utils/scripts/gen_qresource.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    OUTPUT_VARIABLE	PERTUBIS_RCCS)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )

QT4_ADD_RESOURCES(PERTUBIS_RCCS_SRCS ${PERTUBIS_RCCS} )

ADD_DEFINITIONS(-DPALUDIS_TR1_MEMORY_IS_STD_TR1=1 -DPALUDIS_TR1_TYPE_TRAITS_IS_STD_TR1=1 -DPALUDIS_TR1_FUNCTIONAL_IS_STD_TR1=1)
SET (CMAKE_INCLUDE_PATH /usr/local/include/paludis-0.25)
INCLUDE_DIRECTORIES(BEFORE /usr/local/include/paludis-0.25)


ADD_EXECUTABLE (pertubis ${PERTUBIS_SRCS} ${PERTUBIS_MAIN_SRCS} ${PERTUBIS_MOC_SRCS} ${PERTUBIS_RCCS_SRCS} )

TARGET_LINK_LIBRARIES(pertubis ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PALUDIS_TASK_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY_DEBUG} ${QT_QTCORE_LIBRARY_DEBUG} )

######################################################
# install
######################################################

INSTALL(TARGETS pertubis DESTINATION /usr/sbin)
INSTALL(FILES ../utils/desktop-integration/pertubis.desktop DESTINATION /usr/share/applnk/Settings/System)
INSTALL(FILES ../utils/icons/16x16/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/16x16/apps)
INSTALL(FILES ../utils/icons/22x22/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/22x22/apps)
INSTALL(FILES ../utils/icons/32x32/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/32x32/apps)
INSTALL(FILES ../utils/icons/48x48/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/48x48/apps)
INSTALL(FILES ../utils/icons/64x64/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/64x64/apps)
INSTALL(FILES ../utils/icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
INSTALL(FILES ../utils/icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
