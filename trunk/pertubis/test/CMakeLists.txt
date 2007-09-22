
INCLUDE_DIRECTORIES(. ../src/ ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

SET(PERTUBIS_SRCS
    ../src/ArchModel.cc
    ../src/CategoryModel.cc
    ../src/DatabaseView.cc
    ../src/DeinstallTask.cc
    ../src/description_extractor.cc
    ../src/extractor.cc
    ../src/InstallTask.cc
    ../src/Item.cc
    ../src/matcher.cc
    ../src/MessageOutput.cc
    ../src/name_extractor.cc
    ../src/OptionsDelegate.cc
    ../src/PackageItem.cc
    ../src/PackageModel.cc
    ../src/pcre_matcher.cc
    ../src/RepositoryView.cc
    ../src/RepositoryInfoModel.cc
    ../src/RepositoryListModel.cc
    ../src/SearchWindow.cc
    ../src/Settings.cc
    ../src/SyncTask.cc
    ../src/TaskBox.cc
    ../src/Task.cc
    ../src/text_matcher.cc
    ../src/ThreadBase.cc
    ../src/ThreadFetchCategories.cc
    ../src/ThreadFetchDetails.cc
    ../src/ThreadFetchItem.cc
    ../src/ThreadFetchPackages.cc
    ../src/ThreadShowSelections.cc
    ../src/VersionItem.cc
)

SET(PERTUBIS_MOC_HDRS
    ../src/CategoryModel.hh
    ../src/DatabaseView.hh
    ../src/DeinstallTask.hh
    ../src/InstallTask.hh
    ../src/Item.hh
    ../src/MessageOutput.hh
    ../src/OptionsDelegate.hh
    ../src/PackageItem.hh
    ../src/PackageModel.hh
    ../src/RepositoryView.hh
    ../src/RepositoryInfoModel.hh
    ../src/RepositoryListModel.hh
    ../src/SearchWindow.hh
    ../src/Settings.hh
    ../src/TaskBox.hh
    ../src/Task.hh
    ../src/ThreadBase.hh
    ../src/ThreadFetchCategories.hh
    ../src/ThreadFetchDetails.hh
    ../src/ThreadFetchItem.hh
    ../src/ThreadFetchPackages.hh
    ../src/ThreadShowSelections.hh
    ../src/VersionItem.hh
)

ADD_DEFINITIONS(-DPALUDIS_TR1_MEMORY_IS_STD_TR1=1 -DPALUDIS_TR1_TYPE_TRAITS_IS_STD_TR1=1 -DPALUDIS_TR1_FUNCTIONAL_IS_STD_TR1=1)
SET (CMAKE_INCLUDE_PATH /usr/local/include/paludis-0.25)
INCLUDE_DIRECTORIES(BEFORE /usr/local/include/paludis-0.25)

QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )

##################################
# Item test

# SET(ITEM_TEST_SRCS
# 	ItemTest.cc)
#
# SET(ITEM_TEST_MOC_HDRS
# 	ItemTest.hh)
#
# QT4_WRAP_CPP(ITEM_TEST_MOC_SRCS ${ITEM_TEST_MOC_HDRS} )
# ADD_EXECUTABLE (itemtest ${ITEM_TEST_SRCS} ${ITEM_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
# TARGET_LINK_LIBRARIES(itemtest ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

##################################

SET(TASKBOX_TEST_SRCS
    TaskBoxTest.cc)

SET(TASKBOX_TEST_MOC_HDRS
    TaskBoxTest.hh)

QT4_WRAP_CPP(TASKBOX_TEST_MOC_SRCS ${TASKBOX_TEST_MOC_HDRS} )
ADD_EXECUTABLE (taskboxtest ${TASKBOX_TEST_SRCS} ${TASKBOX_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
TARGET_LINK_LIBRARIES(taskboxtest ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PALUDIS_TASK_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

##################################
# InstallTask test

SET(INSTALLTASK_TEST_SRCS
    InstallTaskTest.cc)

SET(INSTALLTASK_TEST_MOC_HDRS
    InstallTaskTest.hh)

QT4_WRAP_CPP(INSTALLTASK_TEST_MOC_SRCS ${INSTALLTASK_TEST_MOC_HDRS} )
ADD_EXECUTABLE (installtasktest ${INSTALLTASK_TEST_SRCS} ${INSTALLTASK_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
TARGET_LINK_LIBRARIES(installtasktest ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PALUDIS_TASK_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

#################################

ENABLE_TESTING()
# ADD_TEST(tests_itemtest itemtest)
ADD_TEST(tests_installtask installtasktest)
ADD_TEST(tests_taskbox taskboxtest)
