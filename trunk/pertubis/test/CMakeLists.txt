
OPTION( PERTUBIS_BUILD_TESTS "if you want to run the ctest suite" OFF )
IF ( PERTUBIS_BUILD_TESTS )

    INCLUDE_DIRECTORIES(BEFORE ${PALUDIS_INCLUDE_DIR} )

    SET(PERTUBIS_SRCS
        ../src/InstallTask.cc
        ../src/ItemInstallTask.cc
        ../src/Package.cc
        ../src/TaskBox.cc
        ../src/Task.cc
        ../src/ThreadBase.cc
    )

    SET(PERTUBIS_MAIN_SRCS
        main.cc)

    SET(PERTUBIS_MOC_HDRS
        ../src/InstallTask.hh
        ../src/ItemInstallTask.hh
        ../src/Package.hh
        ../src/TaskBox.hh
        ../src/ThreadBase.hh
        ../src/Task.hh
    )

    QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )

    ##################################
    # Item test

    SET(ITEM_TEST_SRCS
        ItemTest.cc)

    SET(ITEM_TEST_MOC_HDRS
        ItemTest.hh)

    QT4_WRAP_CPP(ITEM_TEST_MOC_SRCS ${ITEM_TEST_MOC_HDRS} )
    ADD_EXECUTABLE (itemtest ${ITEM_TEST_SRCS} ${ITEM_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
    TARGET_LINK_LIBRARIES(itemtest ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${PALUDIS_ARGS_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

    ##################################

    SET(TASKBOX_TEST_SRCS
        TaskBoxTest.cc)

    SET(TASKBOX_TEST_MOC_HDRS
        TaskBoxTest.hh)

    QT4_WRAP_CPP(TASKBOX_TEST_MOC_SRCS ${TASKBOX_TEST_MOC_HDRS} )
    ADD_EXECUTABLE (taskboxtest ${TASKBOX_TEST_SRCS} ${TASKBOX_TEST_MOC_SRCS} ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
    TARGET_LINK_LIBRARIES(taskboxtest ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${PALUDIS_ARGS_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

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

ENDIF ( PERTUBIS_BUILD_TESTS )
