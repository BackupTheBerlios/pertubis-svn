
INCLUDE_DIRECTORIES(. ../src/ ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

SET(PERTUBIS_SRCS
    ../src/CategoriesThread.cc
    ../src/CategoryFilterModel.cc
    ../src/CategoryItem.cc
    ../src/CategoryModel.cc
    ../src/DatabaseView.cc
    ../src/DeinstallTask.cc
    ../src/description_extractor.cc
    ../src/DetailsThread.cc
    ../src/extractor.cc
    ../src/HtmlFormatter.cc
    ../src/InstallTask.cc
    ../src/Item.cc
    ../src/ItemInstallTask.cc
    ../src/matcher.cc
    ../src/MessageOutput.cc
    ../src/name_extractor.cc
    ../src/OptionsDelegate.cc
    ../src/PackageFilterModel.cc
    ../src/PackageModel.cc
    ../src/PackagesThread.cc
    ../src/RegexMatcher.cc
    ../src/RepositoryListModel.cc
    ../src/SearchThread.cc
    ../src/SearchWindow.cc
    ../src/Settings.cc
    ../src/ShowSelectionsThread.cc
    ../src/SyncTask.cc
    ../src/TaskBox.cc
    ../src/Task.cc
    ../src/text_matcher.cc
    ../src/ThreadBase.cc
)

SET(PERTUBIS_MOC_HDRS
    ../src/CategoryModel.hh
    ../src/CategoryFilterModel.hh
    ../src/DatabaseView.hh
    ../src/DeinstallTask.hh
    ../src/InstallTask.hh
    ../src/Item.hh
    ../src/ItemInstallTask.hh
    ../src/MessageOutput.hh
    ../src/OptionsDelegate.hh
    ../src/PackageFilterModel.hh
    ../src/PackageModel.hh
    ../src/RepositoryListModel.hh
    ../src/SearchWindow.hh
    ../src/Settings.hh
    ../src/TaskBox.hh
    ../src/Task.hh
    ../src/ThreadBase.hh
    ../src/CategoriesThread.hh
    ../src/PackagesThread.hh
    ../src/SearchThread.hh
    ../src/DetailsThread.hh
    ../src/ShowSelectionsThread.hh
)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )
QT4_ADD_RESOURCES(PERTUBIS_RCCS_SRCS ${PERTUBIS_RCCS} )

INCLUDE_DIRECTORIES(BEFORE ${PALUDIS_INCLUDE_DIR} )

##################################
# Item test

SET(ITEM_TEST_SRCS
	ItemTest.cc)

SET(ITEM_TEST_MOC_HDRS
	ItemTest.hh)

QT4_WRAP_CPP(ITEM_TEST_MOC_SRCS ${ITEM_TEST_MOC_HDRS} )
ADD_EXECUTABLE (itemtest ${ITEM_TEST_SRCS} ${ITEM_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
TARGET_LINK_LIBRARIES(itemtest ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

##################################

SET(TASKBOX_TEST_SRCS
    TaskBoxTest.cc)

SET(TASKBOX_TEST_MOC_HDRS
    TaskBoxTest.hh)

QT4_WRAP_CPP(TASKBOX_TEST_MOC_SRCS ${TASKBOX_TEST_MOC_HDRS} )
ADD_EXECUTABLE (taskboxtest ${TASKBOX_TEST_SRCS} ${TASKBOX_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
TARGET_LINK_LIBRARIES(taskboxtest ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

##################################
# InstallTask test

# SET(INSTALLTASK_TEST_SRCS
#     InstallTaskTest.cc)
#
# SET(INSTALLTASK_TEST_MOC_HDRS
#     InstallTaskTest.hh)

# QT4_WRAP_CPP(INSTALLTASK_TEST_MOC_SRCS ${INSTALLTASK_TEST_MOC_HDRS} )
# ADD_EXECUTABLE (installtasktest ${INSTALLTASK_TEST_SRCS} ${INSTALLTASK_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
# TARGET_LINK_LIBRARIES(installtasktest ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PALUDIS_TASK_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

#################################

ENABLE_TESTING()
ADD_TEST(tests_itemtest itemtest)
# ADD_TEST(tests_installtask installtasktest)
ADD_TEST(tests_taskbox taskboxtest)
