# cmake macro to test if we use upnp
#
#  UPNP_FOUND - system has UPNP libs
#  UPNP_INCLUDE_DIR - the UPNP include directory
#  UPNP_LIBRARIES - The libraries needed to use UPNP
#  IXML_LIBRARIES - The libraries needed to use UPNP

# Copyright (c) 2008, Romain CASTAN <romaincastan@gmail.com> 2008
# Copyright (c) 2008, Bertrand Demay <bertranddemay@gmail.com> 2008
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (UPNP_INCLUDE_DIR AND UPNP_LIBRARY AND IXML_LIBRARY AND THREADUTIL_LIBRARY)
  # Already in cache, be silent
  set(UPNP_FIND_QUIETLY TRUE)
endif (UPNP_INCLUDE_DIR AND UPNP_LIBRARY AND IXML_LIBRARY AND THREADUTIL_LIBRARY)

FIND_PATH(UPNP_INCLUDE_DIR NAMES
	upnp.h
	ixml.h PATH_SUFFIXES upnp )

FIND_LIBRARY(UPNP_LIBRARY NAMES libupnp upnp
)

FIND_LIBRARY(IXML_LIBRARY NAMES libixml ixml
)

FIND_LIBRARY(THREADUTIL_LIBRARY NAMES libthreadutil threadutil
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(upnp DEFAULT_MSG UPNP_INCLUDE_DIR UPNP_LIBRARY IXML_LIBRARY THREADUTIL_LIBRARY)

IF (UPNP_FOUND)
  IF (NOT UPNP_FIND_QUIETLY)
    MESSAGE(STATUS "Found Libupnp: ${UPNP_LIBRARY}")
  ENDIF (NOT UPNP_FIND_QUIETLY)
ELSE (UPNP_FOUND)
  IF (NOT UPNP_FOUND)
	MESSAGE(FATAL_ERROR "Could NOT find Libupnp")
  ENDIF (NOT UPNP_FOUND)
  IF (UPNP_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could NOT find acceptable version of Libupnp (version 1.3.1 or later required).")
  ENDIF (UPNP_FIND_REQUIRED)
ENDIF (UPNP_FOUND)


MARK_AS_ADVANCED(UPNP_INCLUDE_DIR UPNP_LIBRARY IXML_LIBRARY THREADUTIL_LIBRARY)
