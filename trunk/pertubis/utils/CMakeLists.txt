
IF(PERTUBIS_SU_TOOL MATCHES "kde")
    SET(PERTUBIS_SU_TOOL "kdesu -t -i pertubis.png")
ENDIF(PERTUBIS_SU_TOOL MATCHES "kde")

IF(PERTUBIS_SU_TOOL MATCHES "gnome")
    SET(PERTUBIS_SU_TOOL "gksu")
ENDIF(PERTUBIS_SU_TOOL MATCHES "gnome")

CONFIGURE_FILE(desktop-integration/pertubis.desktop.in utils/pertubis.desktop)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/pertubis.desktop DESTINATION /usr/share/applnk/Settings/System COMPONENT pertubis)
INSTALL(FILES icons/16x16/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/16x16/apps COMPONENT pertubis)
INSTALL(FILES icons/22x22/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/22x22/apps COMPONENT pertubis)
INSTALL(FILES icons/32x32/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/32x32/apps COMPONENT pertubis)
INSTALL(FILES icons/48x48/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/48x48/apps COMPONENT pertubis)
INSTALL(FILES icons/64x64/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/64x64/apps COMPONENT pertubis)
INSTALL(FILES icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps COMPONENT pertubis)
INSTALL(FILES icons/128x128/apps/pertubis.png DESTINATION /usr/share/icons/hicolor/128x128/apps COMPONENT pertubis)
