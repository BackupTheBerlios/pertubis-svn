
INCLUDE(${QT_USE_FILE})


SET(PERTUBIS_SRCS
    CategoriesThread.cc
    CategoryFilterModel.cc
    CategoryModel.cc
    DatabaseView.cc
    DeinstallTask.cc
    description_extractor.cc
    DetailsThread.cc
    extractor.cc
    HtmlFormatter.cc
    InstallTask.cc
    Item.cc
    ItemInstallTask.cc
    matcher.cc
    MessageOutput.cc
    name_extractor.cc
    OptionsDelegate.cc
    PackageFilterModel.cc
    CategoryItem.cc
    PackageModel.cc
    PackagesThread.cc
    pcre_matcher.cc
    QTOutputStream.hh
    RepositoryInfoModel.cc
    RepositoryListModel.cc
    SearchThread.cc
    SearchWindow.cc
    Settings.cc
    ShowSelectionsThread.cc
    SyncTask.cc
    TaskBox.cc
    Task.cc
    text_matcher.cc
    ThreadBase.cc
#     UseFlagEditor.cc
#     UseFlagModel.cc
)

SET(PERTUBIS_MAIN_SRCS
 	main.cc)

SET(PERTUBIS_MOC_HDRS
    CategoriesThread.hh
    CategoryFilterModel.hh
    CategoryModel.hh
    DatabaseView.hh
    DeinstallTask.hh
    DetailsThread.hh
    InstallTask.hh
#     ItemInstallTask.hh
    Item.hh
    MessageOutput.hh
    OptionsDelegate.hh
    PackageFilterModel.hh
    PackageModel.hh
    PackagesThread.hh
    RepositoryInfoModel.hh
    RepositoryListModel.hh
    SearchThread.hh
    SearchWindow.hh
    Settings.hh
    ShowSelectionsThread.hh
    Task.hh
    TaskBox.hh
    ThreadBase.hh
#     UseFlagEditor.hh
#     UseFlagModel.hh
)

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

TARGET_LINK_LIBRARIES(pertubis ${PALUDIS_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY_DEBUG} ${QT_QTCORE_LIBRARY_DEBUG} )

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
