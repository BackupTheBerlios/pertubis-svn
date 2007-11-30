

SET(PERTUBIS_SRCS
    CategoryThread.cc
    CategoryFilterModel.cc
    CategoryItem.cc
    CategoryModel.cc
    MainWindow.cc
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
    PackageModel.cc
    PackagesThread.cc
    RegexMatcher.cc
    RepositoryListModel.cc
    SearchThread.cc
    SearchWindow.cc
    SetModel.cc
    SetThread.cc
    Settings.cc
    ShowSelectionsThread.cc
    SyncTask.cc
    TaskBox.cc
    Task.cc
    text_matcher.cc
    ThreadBase.cc
)

SET(PERTUBIS_MAIN_SRCS
    main.cc)

SET(PERTUBIS_MOC_HDRS
    CategoryThread.hh
    CategoryFilterModel.hh
    CategoryModel.hh
    MainWindow.hh
    DeinstallTask.hh
    DetailsThread.hh
    InstallTask.hh
    Item.hh
    ItemInstallTask.hh
    MessageOutput.hh
    OptionsDelegate.hh
    PackageFilterModel.hh
    PackageModel.hh
    PackagesThread.hh
    RepositoryListModel.hh
    SearchThread.hh
    SearchWindow.hh
    SetModel.hh
    SetThread.hh
    Settings.hh
    ShowSelectionsThread.hh
    SyncTask.hh
    TaskBox.hh
    Task.hh
    ThreadBase.hh
)

EXEC_PROGRAM("/bin/sh"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../utils/scripts/gen_qresource.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    OUTPUT_VARIABLE PERTUBIS_RCCS)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )
QT4_ADD_RESOURCES(PERTUBIS_RCCS_SRCS ${PERTUBIS_RCCS} )

INCLUDE_DIRECTORIES(BEFORE ${PALUDIS_INCLUDE_DIR} )

ADD_EXECUTABLE(pertubis ${PERTUBIS_SRCS} ${PERTUBIS_MAIN_SRCS} ${PERTUBIS_MOC_SRCS} ${PERTUBIS_RCCS_SRCS} )
LINK_DIRECTORIES(${PALUDIS_LINK_DIR} )
TARGET_LINK_LIBRARIES(pertubis ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${QT_QTGUI_LIBRARY_DEBUG} ${QT_QTCORE_LIBRARY_DEBUG} )
INSTALL(TARGETS pertubis DESTINATION /usr/sbin COMPONENT pertubis)

