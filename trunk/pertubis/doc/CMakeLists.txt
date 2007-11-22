PROJECT(doxygen)
FIND_PACKAGE(Doxygen REQUIRED)

CONFIGURE_FILE(${doxygen_SOURCE_DIR}/Doxyfile.in
            ${doxygen_BINARY_DIR}/Doxyfile)

ADD_CUSTOM_TARGET(docs-doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxygen_BINARY_DIR}/Doxyfile)
