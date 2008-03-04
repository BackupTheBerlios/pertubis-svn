
OPTION( PERTUBIS_BUILD_TESTS "if you want to run the ctest suite" OFF )
IF ( PERTUBIS_BUILD_TESTS )

    INCLUDE_DIRECTORIES(BEFORE ${PALUDIS_INCLUDE_DIR} )

    SET(PERTUBIS_SRCS
        InstallDeinstallTest.cc
        ../src/PertubisInstallTask.cc
        ../src/PertubisDeinstallTask.cc
        ../src/Package.cc
        ../src/Selections.cc
        ../src/PaludisUtils.cc
        ../src/InstallSelections.cc
        ../src/ThreadBase.cc
    )

    SET(PERTUBIS_MAIN_SRCS
        main.cc)

    SET(PERTUBIS_MOC_HDRS
        ../src/PertubisInstallTask.hh
        ../src/PertubisDeinstallTask.hh
        InstallDeinstallTest.hh
        ../src/Package.hh
        ../src/Selections.hh
        ../src/InstallSelections.hh
        ../src/ThreadBase.hh
    )

    QT4_WRAP_CPP(PERTUBIS_MOC_SRCS ${PERTUBIS_MOC_HDRS} )
    ADD_EXECUTABLE (i_d_test ${PERTUBIS_SRCS} ${PERTUBIS_MOC_SRCS} )
    TARGET_LINK_LIBRARIES(i_d_test ${PALUDIS_LIB} ${PALUDIS_UTIL_LIB} ${PALUDIS_ARGS_LIB} ${PCRE_LIB} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})

    ENABLE_TESTING()
    ADD_TEST(tests_i_d_test i_d_test)

ENDIF ( PERTUBIS_BUILD_TESTS )
