
SET(PERTUBIS_SRCS
    CategoryFilterModel.cc
    CategoryItem.cc
    CategoryModel.cc
    CategoryThread.cc
    DeinstallTask.cc
    DepListSettings.cc
    description_extractor.cc
    DetailsThread.cc
    extractor.cc
    HtmlFormatter.cc
    InstallSettings.cc
    InstallTask.cc
    ItemInstallTask.cc
    MainWindow.cc
    matcher.cc
    MessageOutput.cc
    name_extractor.cc
    OptionsDelegate.cc
    Package.cc
    PackageFilterModel.cc
    PackageModel.cc
    PackagesThread.cc
    PaludisUtils.cc
    QuerySettings.cc
    RegexMatcher.cc
    RepositoryListModel.cc
    SearchThread.cc
    SearchWindow.cc
    SetThread.cc
    Settings.cc
    ShowSelectionsThread.cc
    SyncTask.cc
    TaskBox.cc
    Task.cc
    text_matcher.cc
    ThreadBase.cc
    UninstallSettings.cc
)

SET(PERTUBIS_MAIN_SRCS
    main.cc)

SET(PERTUBIS_MOC_HDRS
    CategoryFilterModel.hh
    CategoryModel.hh
    CategoryThread.hh
    DeinstallTask.hh
    DepListSettings.hh
    DetailsThread.hh
    InstallSettings.hh
    InstallTask.hh
    ItemInstallTask.hh
    MainWindow.hh
    MessageOutput.hh
    OptionsDelegate.hh
    PackageFilterModel.hh
    Package.hh
    PackageModel.hh
    PackagesThread.hh
    QuerySettings.hh
    RepositoryListModel.hh
    SearchThread.hh
    SearchWindow.hh
    SetThread.hh
    Settings.hh
    ShowSelectionsThread.hh
    SyncTask.hh
    TaskBox.hh
    Task.hh
    ThreadBase.hh
    UninstallSettings.hh
)

EXEC_PROGRAM("/bin/sh"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../utils/scripts/gen_qresource.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    OUTPUT_VARIABLE PERTUBIS_RCCS)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )
QT4_ADD_RESOURCES(PERTUBIS_RCCS_SRCS ${PERTUBIS_RCCS} )

INCLUDE_DIRECTORIES(BEFORE ${PALUDIS_INCLUDE_DIR} )

ADD_EXECUTABLE(pertubis ${PERTUBIS_MAIN_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} ${PERTUBIS_RCCS_SRCS} )
TARGET_LINK_LIBRARIES(pertubis ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${PALUDIS_ARGS_LIB} ${QT_QTGUI_LIBRARY_DEBUG} ${QT_QTCORE_LIBRARY_DEBUG} )
INSTALL(TARGETS pertubis DESTINATION /usr/sbin COMPONENT pertubis)
