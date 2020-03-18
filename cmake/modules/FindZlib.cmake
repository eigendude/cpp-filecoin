# Copyright (c) 2017, Ruslan Baratov
# All rights reserved.

set(ZLIB_TARGET_NAME "ZLIB::zlib")

if (NOT TARGET ${ZLIB_TARGET_NAME})
  find_package(PkgConfig)

  if (PKG_CONFIG_FOUND)
    pkg_check_modules(ZLIB zlib)

    if (ZLIB_FOUND)
      message(STATUS "Creating target '${ZLIB_TARGET_NAME}':")
      message(STATUS "* library: '${ZLIB_LIBRARIES}'")
      message(STATUS "* include dirs: '${ZLIB_INCLUDE_DIRS}'")

      add_library(${ZLIB_TARGET_NAME} UNKNOWN IMPORTED)
      set_target_properties(
        ${ZLIB_TARGET_NAME}
        PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGE "CXX"
        IMPORTED_LOCATION "${ZLIB_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}"
      )
    endif()
  endif()
endif()
