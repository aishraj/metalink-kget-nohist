include_directories(
   ../../
)

set(kget_metalinkhttpfactory_PART_SRCS
  metalinkhttpfactory.cpp
  metalinkhttp.cpp
  ../../ui/metalinkcreator/metalinker.cpp
)

kde4_add_kcfg_files(kget_metalinkhttpfactory_PART_SRCS metalinkhttpsettings.kcfgc)
# 
kde4_add_plugin(kget_metalinkhttpfactory ${kget_metalinkhttpfactory_PART_SRCS})

target_link_libraries(kget_metalinkhttpfactory ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kgetcore)
if (HAVE_NEPOMUK)
    target_link_libraries(kget_metalinkhttpfactory ${SOPRANO_LIBRARIES} ${NEPOMUK_LIBRARIES})
endif (HAVE_NEPOMUK)

install(TARGETS kget_metalinkhttpfactory DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kget_metalinkhttpfactory.desktop DESTINATION ${SERVICES_INSTALL_DIR})

###Build KCM-Module
set(kcm_kget_metalinkhttpfactory_PART_SRCS
dlgmetalinkhttp.cpp
)

kde4_add_ui_files(kcm_kget_metalinkhttpfactory_PART_SRCS
dlgmetalinkhttp.ui
)
kde4_add_kcfg_files(kcm_kget_metalinkhttpfactory_PART_SRCS metalinkhttpsettings.kcfgc)
kde4_add_plugin(kcm_kget_metalinkhttpfactory ${kcm_kget_metalinkhttpfactory_PART_SRCS})
target_link_libraries(kcm_kget_metalinkhttpfactory ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})
install(TARGETS kcm_kget_metalinkhttpfactory DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kget_metalinkhttpfactory_config.desktop DESTINATION ${SERVICES_INSTALL_DIR})