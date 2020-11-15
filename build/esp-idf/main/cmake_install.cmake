# Install script for directory: /home/maikschulze/ESP32/ESP32_Device/main

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/core/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Attr_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Define_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Delete_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Deleteattr_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Get_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Help_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Include_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/IOWrite_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/List_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Rename_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Rereadcfg_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Save_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Set_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Setstate_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/commands/Shutdown_Command/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/modules/Global_Module/cmake_install.cmake")
  include("/home/maikschulze/ESP32/ESP32_Device/build/esp-idf/main/lib/modules/Telnet_Module/cmake_install.cmake")

endif()

