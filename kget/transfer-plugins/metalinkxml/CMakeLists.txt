#Set supported mime type
SET(SUPPORTED_KGET_MIMETYPES "${SUPPORTED_KGET_MIMETYPES}application/metalink+xml;" PARENT_SCOPE)

include_directories(
   ../../
)

set(kget_metalinkxmlfactory_PART_SRCS
  metalinkxmlfactory.cpp
  metalinkxml.cpp
  fileselectiondlg.cpp
  ../../ui/metalinkcreator/metalinker.cpp
)

kde4_add_ui_files(kget_metalinkxmlfactory_PART_SRCS
    fileselection.ui
)

kde4_add_kcfg_files(kget_metalinkxmlfactory_PART_SRCS metalinkxmlsettings.kcfgc)

kde4_add_plugin(kget_metalinkxmlfactory ${kget_metalinkxmlfactory_PART_SRCS})

target_link_libraries(kget_metalinkxmlfactory ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kgetcore)
if (HAVE_NEPOMUK)
    target_link_libraries(kget_metalinkxmlfactory ${SOPRANO_LIBRARIES} ${NEPOMUK_LIBRARIES})
endif (HAVE_NEPOMUK)

install(TARGETS kget_metalinkxmlfactory DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kget_metalinkxmlfactory.desktop DESTINATION ${SERVICES_INSTALL_DIR})


###Build KCM-Module
set(kcm_kget_metalinkxmlfactory_PART_SRCS
dlgmetalink.cpp
)

kde4_add_ui_files(kcm_kget_metalinkxmlfactory_PART_SRCS
dlgmetalink.ui
)

kde4_add_kcfg_files(kcm_kget_metalinkxmlfactory_PART_SRCS metalinkxmlsettings.kcfgc)
kde4_add_plugin(kcm_kget_metalinkxmlfactory ${kcm_kget_metalinkxmlfactory_PART_SRCS})
target_link_libraries(kcm_kget_metalinkxmlfactory ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})
install(TARGETS kcm_kget_metalinkxmlfactory DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kget_metalinkxmlfactory_config.desktop DESTINATION ${SERVICES_INSTALL_DIR})