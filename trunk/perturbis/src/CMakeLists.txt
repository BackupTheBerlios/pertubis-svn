
CONFIGURE_FILE(version.hh.in ${PERTUBIS_SOURCE_DIR}/src/version.hh)

SET(PERTUBIS_SRCS
    CategoryFilterModel.cc
    CategoryModel.cc
    CategoryThread.cc
    DeinstallSelections.cc
    DeinstallSettings.cc
    DepListSettings.cc
    DetailsThread.cc
    extractor.cc
    GeneralSettings.cc
    GLSAParser.cc
    HtmlFormatter.cc
    InstallSelections.cc
    InstallSettings.cc
    main.cc
    MainWindow.cc
    matcher.cc
    MessageOutput.cc
    NameDescriptionExtractor.cc
    PackageBrowsingPage.cc
    Package.cc
    PackageFilterModel.cc
    PackageModel.cc
    PackageModelDelegate.cc
    PackagesThread.cc
    Page.cc
    make_package.cc
    PaludisUtils.cc
    PerturbisDeinstallTask.cc
    PerturbisInstallTask.cc
    QuerySettings.cc
    RegexMatcher.cc
    ReportModelDelegate.cc
    ReportPackage.cc
    RepositoryPage.cc
    ReportPackageModel.cc
    RepositoryInfoModel.cc
    RepositoryListModel.cc
    SearchPage.cc
    SearchThread.cc
    SelectionModel.cc
    SelectionModelDelegate.cc
    SelectionPage.cc
    Selections.cc
    SetThread.cc
    Settings.cc
    SystemReport.cc
    SystemReportPage.cc
    text_matcher.cc
    ThreadBase.cc
    TaskQueue.cc
)

SET(PERTUBIS_MOC_HDRS
    CategoryFilterModel.hh
    CategoryModel.hh
    CategoryThread.hh
    DeinstallSelections.hh
    DeinstallSettings.hh
    DepListSettings.hh
    DetailsThread.hh
    GeneralSettings.hh
    InstallSelections.hh
    InstallSettings.hh
    MainWindow.hh
    MessageOutput.hh
    PackageBrowsingPage.hh
    PackageFilterModel.hh
    Package.hh
    Page.hh
    PackageModel.hh
    PackagesThread.hh
    PerturbisDeinstallTask.hh
    PerturbisInstallTask.hh
    QuerySettings.hh
    RepositoryPage.hh
    ReportPackageModel.hh
    RepositoryInfoModel.hh
    RepositoryListModel.hh
    SearchPage.hh
    SearchThread.hh
    SelectionModel.hh
    SelectionPage.hh
    SetThread.hh
    Settings.hh
    SystemReport.hh
    SystemReportPage.hh
    ThreadBase.hh
    TaskQueue.hh
)

EXEC_PROGRAM("/bin/sh"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../utils/scripts/gen_qresource.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    OUTPUT_VARIABLE PERTUBIS_RCCS)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )

QT4_ADD_RESOURCES(PERTUBIS_RCCS_SRCS ${PERTUBIS_RCCS} )

INCLUDE_DIRECTORIES(BEFORE ${PALUDIS_INCLUDE_DIR} )

ADD_EXECUTABLE(perturbis
    ${PERTUBIS_SRCS}
    ${PERTUBIS_MOC_SRCS}
    ${PERTUBIS_RCCS_SRCS}
)

TARGET_LINK_LIBRARIES(perturbis
    ${PALUDIS_LIB}
    ${PALUDIS_UTIL_LIB}
    ${PALUDIS_ARGS_LIB}
    ${QT_QTGUI_LIBRARY_DEBUG}
    ${QT_QTXML_LIBRARY_DEBUG}
    ${QT_QTCORE_LIBRARY_DEBUG}
)

INSTALL(TARGETS perturbis
        DESTINATION /usr/lib/perturbis
        COMPONENT perturbis)
