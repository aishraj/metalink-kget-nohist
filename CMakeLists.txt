project(kdenetwork)

set(INSIDE_KDENETWORK TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckPrototypeExists)
include(CheckTypeSize)

# If definitions like -D_GNU_SOURCE are needed for these checks they
# should be added to _KDE4_PLATFORM_DEFINITIONS when it is originally
# defined outside this file.  Here we include these definitions in
# CMAKE_REQUIRED_DEFINITIONS so they will be included in the build of
# checks below.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})
if (WIN32)
   set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN_LIBRARIES} )
   set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN_INCLUDES} )
endif (WIN32)

find_package(KdepimLibs REQUIRED)
# find_package(X11VidMode) not used at this time

# NX support is not ready for KDE 4.2; disabled (uwolfer)
# macro_optional_find_package(LibNXCL)
# macro_log_feature(LIBNXCL_FOUND "libnxcl" "NX X compression client library" "http://svn.berlios.de/svnroot/repos/freenx/trunk/freenx-client/nxcl/" FALSE "1.0" "Needed to build Krdc with NX support")

macro_optional_find_package(JPEG)
macro_log_feature(JPEG_FOUND "libjpeg" "Development library for JPEG formatted image files" "http://www.ijg.org/" FALSE "" "Provides JPEG images in Krdc.")

macro_optional_find_package(Nepomuk)
macro_log_feature(Nepomuk_FOUND "Nepomuk" "Semantic Desktop" "http://nepomuk.kde.org" FALSE "" "Nepomuk is needed for the integration into KGet")

macro_optional_find_package(Sqlite)
macro_log_feature(SQLITE_FOUND "SQLite" "SQLite is a Binary-Database" "" FALSE "" "Needed for the SQLite-Backend of the KGet-History and the Kopete-Statistic-Plugin")

macro_optional_find_package(Strigi)
macro_log_feature(STRIGI_FOUND "Strigi" "Metadata extraction" "" FALSE "" "Needed to get metadata for .torrent files")

macro_optional_find_package(QCA2)
macro_log_feature(QCA2_FOUND "QCA2" "Qt Cryptographic Architecture" "http://delta.affinix.com/qca" FALSE "2.0.0" "Needed for the KGet bittorrent-plugin and some Kopete plugins")

macro_optional_find_package(QGpgme)
macro_log_feature(QGPGME_FOUND "QGpgME" "The QGpgME library" "http://www.kde.org" FALSE "" "QGpgME is required to have signature verifying support in KGet.")

macro_optional_find_package(KTorrent)
macro_log_feature(LIBKTORRENT_FOUND "libktorrent" "Backend Library of KTorrent" "http://ktorrent.org" FALSE "" "Needed to build KGet bittorrent support.")

macro_optional_find_package(LibMms)
macro_log_feature(LIBMMS_FOUND "libmms" "Library to read mms streams" "http://libmms.sourceforge.net" FALSE "" "Needed to build KGet mms support.")

macro_optional_find_package(TelepathyQt4)
macro_log_feature(TelepathyQt4_FOUND "telepathy-qt" "Telepathy Qt Bindings" "http://telepathy.freedesktop.org" FALSE "0.9" "Needed to build Telepathy Tubes support.")

add_definitions (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${KDEPIMLIBS_INCLUDE_DIR})

macro_optional_add_subdirectory(kfile-plugins)
macro_optional_add_subdirectory(kget)

macro_optional_add_subdirectory(kopete)

macro_optional_add_subdirectory(krdc)

if(Q_WS_X11)
  # kppp is broken on FreeBSD >= 8.0
  # disabling until a fix appears
  if(NOT CMAKE_SYSTEM_NAME MATCHES FreeBSD AND NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL GNU)
    macro_optional_add_subdirectory(kppp)
  endif(NOT CMAKE_SYSTEM_NAME MATCHES FreeBSD AND NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL GNU)

  macro_optional_add_subdirectory(krfb)
endif(Q_WS_X11)

macro_optional_add_subdirectory(kdnssd)

option(SAMBA_INSTALL "Offer to install Samba for file sharing with PackageKit if it is not already installed, use -DSAMBA_INSTALL=off to disable, use -DSAMBA_PACKAGE_NAME= to set package name." ON)
if(NOT SAMBA_PACKAGE_NAME)
    set(SAMBA_PACKAGE_NAME \\\"samba\\\")
endif(NOT SAMBA_PACKAGE_NAME)
if(SAMBA_INSTALL)
    message(STATUS "Samba install feature will be enabled.")
    add_definitions(-DSAMBA_INSTALL)
    add_definitions(-DSAMBA_PACKAGE_NAME=${SAMBA_PACKAGE_NAME})
else(SAMBA_INSTALL)
    message(STATUS "Samba install feature will NOT be enabled.")
endif(SAMBA_INSTALL)

if(NOT WIN32)
  macro_optional_add_subdirectory(filesharing)
endif(NOT WIN32)

macro_optional_add_subdirectory(doc)
macro_display_feature_log()
