
IF(PERTUBIS_SU_TOOL MATCHES "kde")
    SET(PERTUBIS_SU_TOOL "kdesu -t -i perturbis.png")
ENDIF(PERTUBIS_SU_TOOL MATCHES "kde")

IF(PERTUBIS_SU_TOOL MATCHES "gnome")
    SET(PERTUBIS_SU_TOOL "gksu")
ENDIF(PERTUBIS_SU_TOOL MATCHES "gnome")

INSTALL(FILES desktop-integration/perturbis.desktop DESTINATION /usr/share/applnk/Settings/System COMPONENT perturbis)
INSTALL(FILES glsa/glsa.css DESTINATION /usr/lib/perturbis COMPONENT perturbis)
INSTALL(PROGRAMS scripts/perturbis-launcher DESTINATION /usr/bin COMPONENT perturbis)
INSTALL(FILES icons/16x16/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/16x16/apps COMPONENT perturbis)
INSTALL(FILES icons/22x22/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/22x22/apps COMPONENT perturbis)
INSTALL(FILES icons/32x32/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/32x32/apps COMPONENT perturbis)
INSTALL(FILES icons/48x48/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/48x48/apps COMPONENT perturbis)
INSTALL(FILES icons/64x64/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/64x64/apps COMPONENT perturbis)
INSTALL(FILES icons/128x128/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/128x128/apps COMPONENT perturbis)
INSTALL(FILES icons/128x128/apps/perturbis.png DESTINATION /usr/share/icons/hicolor/128x128/apps COMPONENT perturbis)
