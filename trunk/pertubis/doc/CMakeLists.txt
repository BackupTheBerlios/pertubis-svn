
OPTION( PERTUBIS_BUILD_DOCS "Build the doxygen documentation" OFF )
IF ( PERTUBIS_BUILD_DOCS )
    FIND_PACKAGE(Doxygen REQUIRED)
    IF (DOXYGEN_FOUND)
        SET(DOXYGEN_INPUT "${PERTUBIS_BINARY_DIR}/doc/Doxyfile")
        SET(APIDOC_DIR ${PERTUBIS_BINARY_DIR}/doc/html CACHE PATH "API documentation directory")
        SET(DOXYGEN_OUTPUT ${APIDOC_DIR}/index.html)
        MESSAGE("DOXYGEN_INPUT ${DOXYGEN_INPUT}")
        MESSAGE("DOXYGEN_OUTPUT ${DOXYGEN_OUTPUT}")
        MESSAGE("APIDOC_DIR ${APIDOC_DIR}")

        CONFIGURE_FILE(${PERTUBIS_SOURCE_DIR}/doc/Doxyfile.in
            ${DOXYGEN_INPUT})
        CONFIGURE_FILE(${PERTUBIS_SOURCE_DIR}/doc/build-docs.sh
            ${PERTUBIS_BINARY_DIR}/doc/build-docs.sh)

        FILE(MAKE_DIRECTORY ${PERTUBIS_BINARY_DIR}/doc/html)

        ADD_CUSTOM_COMMAND(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND /bin/sh ${PERTUBIS_BINARY_DIR}/doc/build-docs.sh
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT}
            )

        ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

        ADD_CUSTOM_TARGET(apidoc_forced
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND /bin/sh ${PERTUBIS_BINARY_DIR}/doc/build-docs.sh
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

        INSTALL(DIRECTORY ${APIDOC_DIR} DESTINATION ${PERTUBIS_DOC_PATH})

    ENDIF (DOXYGEN_FOUND)
ENDIF ( PERTUBIS_BUILD_DOCS )
