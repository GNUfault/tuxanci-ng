# Install script for directory: /home/connor/extreme-tuxanci/data/sound

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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/share/tuxanci/sound/dead.ogg;/usr/local/share/tuxanci/sound/end.ogg;/usr/local/share/tuxanci/sound/explozion.ogg;/usr/local/share/tuxanci/sound/gun2.ogg;/usr/local/share/tuxanci/sound/gun_lasser.ogg;/usr/local/share/tuxanci/sound/gun_revolver.ogg;/usr/local/share/tuxanci/sound/gun_scatter.ogg;/usr/local/share/tuxanci/sound/gun_tommy.ogg;/usr/local/share/tuxanci/sound/item_bonus.ogg;/usr/local/share/tuxanci/sound/item_gun.ogg;/usr/local/share/tuxanci/sound/odraz-gun.ogg;/usr/local/share/tuxanci/sound/odraz-l.ogg;/usr/local/share/tuxanci/sound/odraz-r.ogg;/usr/local/share/tuxanci/sound/put_mine.ogg;/usr/local/share/tuxanci/sound/smrt.ogg;/usr/local/share/tuxanci/sound/switch_gun.ogg;/usr/local/share/tuxanci/sound/teleport.ogg;/usr/local/share/tuxanci/sound/tuxanci-smrt.ogg")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/tuxanci/sound" TYPE FILE FILES
    "/home/connor/extreme-tuxanci/data/sound/dead.ogg"
    "/home/connor/extreme-tuxanci/data/sound/end.ogg"
    "/home/connor/extreme-tuxanci/data/sound/explozion.ogg"
    "/home/connor/extreme-tuxanci/data/sound/gun2.ogg"
    "/home/connor/extreme-tuxanci/data/sound/gun_lasser.ogg"
    "/home/connor/extreme-tuxanci/data/sound/gun_revolver.ogg"
    "/home/connor/extreme-tuxanci/data/sound/gun_scatter.ogg"
    "/home/connor/extreme-tuxanci/data/sound/gun_tommy.ogg"
    "/home/connor/extreme-tuxanci/data/sound/item_bonus.ogg"
    "/home/connor/extreme-tuxanci/data/sound/item_gun.ogg"
    "/home/connor/extreme-tuxanci/data/sound/odraz-gun.ogg"
    "/home/connor/extreme-tuxanci/data/sound/odraz-l.ogg"
    "/home/connor/extreme-tuxanci/data/sound/odraz-r.ogg"
    "/home/connor/extreme-tuxanci/data/sound/put_mine.ogg"
    "/home/connor/extreme-tuxanci/data/sound/smrt.ogg"
    "/home/connor/extreme-tuxanci/data/sound/switch_gun.ogg"
    "/home/connor/extreme-tuxanci/data/sound/teleport.ogg"
    "/home/connor/extreme-tuxanci/data/sound/tuxanci-smrt.ogg"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/connor/extreme-tuxanci/cmake/data/sound/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
