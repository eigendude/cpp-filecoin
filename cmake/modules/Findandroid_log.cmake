# Copyright (c) 2017, Ruslan Baratov
# All rights reserved.

set(ANDROID_LOG_TARGET_NAME "android_log::android_log")

find_library(ANDROID_LOG_LIBRARY "log")
find_path(ANDROID_LOG_HEADER "android/log.h")

if (NOT TARGET ${ANDROID_LOG_TARGET_NAME})
  message(STATUS "Creating target '${ANDROID_LOG_TARGET_NAME}':")
  message(STATUS "* library: '${ANDROID_LOG_LIBRARY}'")
  message(STATUS "* header: '${ANDROID_LOG_HEADER}'")

  add_library(${ANDROID_LOG_TARGET_NAME} UNKNOWN IMPORTED)
  set_target_properties(
    ${ANDROID_LOG_TARGET_NAME}
    PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGE "CXX"
    IMPORTED_LOCATION "${ANDROID_LOG_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${ANDROID_LOG_HEADER}"
  )
endif()
