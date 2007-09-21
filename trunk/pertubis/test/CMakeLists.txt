
INCLUDE_DIRECTORIES(. ../src/ ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})

SET(PERTUBIS_SRCS
	../src/ArchModel.cc
	../src/CategoryModel.cc
	../src/DatabaseView.cc
	../src/description_extractor.cc
	../src/extractor.cc
	../src/matcher.cc
	../src/name_extractor.cc
	../src/OptionsDelegate.cc
    ../src/MessageOutput.cc
	../src/Item.cc
	../src/PackageItem.cc
	../src/VersionItem.cc
	../src/PackageModel.cc
	../src/pcre_matcher.cc
	../src/SearchWindow.cc
	../src/InstallTask.cc
	../src/DeinstallTask.cc
	../src/Settings.cc
	../src/TaskBox.cc
	../src/Task.cc
    ../src/SyncTask.cc
	../src/text_matcher.cc
	../src/ThreadBase.cc
	../src/ThreadFetchCategories.cc
	../src/ThreadFetchDetails.cc
	../src/ThreadFetchItem.cc
	../src/ThreadFetchPackages.cc
    ../src/ThreadShowSelections.cc
)

SET(PERTUBIS_MOC_HDRS
	../src/CategoryModel.hh
	../src/DatabaseView.hh
	../src/OptionsDelegate.hh
	../src/InstallTask.hh
    ../src/MessageOutput.hh
	../src/DeinstallTask.hh
	../src/PackageItem.hh
	../src/VersionItem.hh
	../src/Item.hh
	../src/PackageModel.hh
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
# InstallTask test

SET(INSTALLTASK_TEST_SRCS
	InstallTaskTest.cc)

SET(INSTALLTASK_TEST_MOC_HDRS
	InstallTaskTest.hh)

QT4_WRAP_CPP(INSTALLTASK_TEST_MOC_SRCS ${INSTALLTASK_TEST_MOC_HDRS} )
ADD_EXECUTABLE (installtasktest ${INSTALLTASK_TEST_SRCS} ${INSTALLTASK_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
TARGET_LINK_LIBRARIES(installtasktest ${PALUDISENV_LIB} ${PALUDIS_ENV_LIB} ${PALUDIS_TASK_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

ENABLE_TESTING()
# ADD_TEST(tests_itemtest itemtest)
ADD_TEST(tests_installtask installtasktest)
