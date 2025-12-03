# Install script for directory: E:/Better-Fortress-2/src/thirdparty/sentry-native/external/crashpad

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "E:/Better-Fortress-2/src/thirdparty/sentry-native/build/src/thirdparty/sentry-native/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/compat/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/minidump/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/snapshot/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/util/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/third_party/mini_chromium/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/client/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/third_party/zlib/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/third_party/getopt/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/third_party/mpack/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/tools/cmake_install.cmake")
  include("E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/handler/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "E:/Better-Fortress-2/src/thirdparty/sentry-native/build/crashpad_build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
