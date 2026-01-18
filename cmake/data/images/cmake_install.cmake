# Install script for directory: /home/connor/extreme-tuxanci/data/images

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
   "/usr/local/share/tuxanci/images/big-explosion.png;/usr/local/share/tuxanci/images/bombball.png;/usr/local/share/tuxanci/images/button0.png;/usr/local/share/tuxanci/images/button1.png;/usr/local/share/tuxanci/images/chat.png;/usr/local/share/tuxanci/images/check.png;/usr/local/share/tuxanci/images/choice.png;/usr/local/share/tuxanci/images/cross.png;/usr/local/share/tuxanci/images/explosion.png;/usr/local/share/tuxanci/images/frame.png;/usr/local/share/tuxanci/images/graf.bonus.png;/usr/local/share/tuxanci/images/icon.bonus.4x.png;/usr/local/share/tuxanci/images/icon.bonus.ghost.png;/usr/local/share/tuxanci/images/icon.bonus.hidden.png;/usr/local/share/tuxanci/images/icon.bonus.shot.png;/usr/local/share/tuxanci/images/icon.bonus.speed.png;/usr/local/share/tuxanci/images/icon.bonus.teleport.png;/usr/local/share/tuxanci/images/icon.gun.bombball.png;/usr/local/share/tuxanci/images/icon.gun.dual.simple.png;/usr/local/share/tuxanci/images/icon.gun.lasser.png;/usr/local/share/tuxanci/images/icon.gun.mine.png;/usr/local/share/tuxanci/images/icon.gun.scatter.png;/usr/local/share/tuxanci/images/icon.gun.simple.png;/usr/local/share/tuxanci/images/icon.gun.tommy.png;/usr/local/share/tuxanci/images/in_a_captivity_of_8-bit.png;/usr/local/share/tuxanci/images/item.bombball.png;/usr/local/share/tuxanci/images/item.bonus.4x.png;/usr/local/share/tuxanci/images/item.bonus.ghost.png;/usr/local/share/tuxanci/images/item.bonus.hidden.png;/usr/local/share/tuxanci/images/item.bonus.shot.png;/usr/local/share/tuxanci/images/item.bonus.speed.png;/usr/local/share/tuxanci/images/item.bonus.teleport.png;/usr/local/share/tuxanci/images/item.dual.png;/usr/local/share/tuxanci/images/item.lasser.png;/usr/local/share/tuxanci/images/item.mine.png;/usr/local/share/tuxanci/images/item.scatter.png;/usr/local/share/tuxanci/images/item.tommy.png;/usr/local/share/tuxanci/images/lasserX.png;/usr/local/share/tuxanci/images/lasserY.png;/usr/local/share/tuxanci/images/level4.png;/usr/local/share/tuxanci/images/mine.png;/usr/local/share/tuxanci/images/naboj.png;/usr/local/share/tuxanci/images/panel.png;/usr/local/share/tuxanci/images/panel_shot.png;/usr/local/share/tuxanci/images/pauza.png;/usr/local/share/tuxanci/images/pokus.png;/usr/local/share/tuxanci/images/radar.png;/usr/local/share/tuxanci/images/screen_main.png;/usr/local/share/tuxanci/images/screen_table.png;/usr/local/share/tuxanci/images/shot.png;/usr/local/share/tuxanci/images/textfield0.png;/usr/local/share/tuxanci/images/textfield1.png;/usr/local/share/tuxanci/images/tux2.png;/usr/local/share/tuxanci/images/tux4.png;/usr/local/share/tuxanci/images/tux6.png;/usr/local/share/tuxanci/images/tux8.png;/usr/local/share/tuxanci/images/tuxanci.png")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/share/tuxanci/images" TYPE FILE FILES
    "/home/connor/extreme-tuxanci/data/images/big-explosion.png"
    "/home/connor/extreme-tuxanci/data/images/bombball.png"
    "/home/connor/extreme-tuxanci/data/images/button0.png"
    "/home/connor/extreme-tuxanci/data/images/button1.png"
    "/home/connor/extreme-tuxanci/data/images/chat.png"
    "/home/connor/extreme-tuxanci/data/images/check.png"
    "/home/connor/extreme-tuxanci/data/images/choice.png"
    "/home/connor/extreme-tuxanci/data/images/cross.png"
    "/home/connor/extreme-tuxanci/data/images/explosion.png"
    "/home/connor/extreme-tuxanci/data/images/frame.png"
    "/home/connor/extreme-tuxanci/data/images/graf.bonus.png"
    "/home/connor/extreme-tuxanci/data/images/icon.bonus.4x.png"
    "/home/connor/extreme-tuxanci/data/images/icon.bonus.ghost.png"
    "/home/connor/extreme-tuxanci/data/images/icon.bonus.hidden.png"
    "/home/connor/extreme-tuxanci/data/images/icon.bonus.shot.png"
    "/home/connor/extreme-tuxanci/data/images/icon.bonus.speed.png"
    "/home/connor/extreme-tuxanci/data/images/icon.bonus.teleport.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.bombball.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.dual.simple.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.lasser.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.mine.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.scatter.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.simple.png"
    "/home/connor/extreme-tuxanci/data/images/icon.gun.tommy.png"
    "/home/connor/extreme-tuxanci/data/images/in_a_captivity_of_8-bit.png"
    "/home/connor/extreme-tuxanci/data/images/item.bombball.png"
    "/home/connor/extreme-tuxanci/data/images/item.bonus.4x.png"
    "/home/connor/extreme-tuxanci/data/images/item.bonus.ghost.png"
    "/home/connor/extreme-tuxanci/data/images/item.bonus.hidden.png"
    "/home/connor/extreme-tuxanci/data/images/item.bonus.shot.png"
    "/home/connor/extreme-tuxanci/data/images/item.bonus.speed.png"
    "/home/connor/extreme-tuxanci/data/images/item.bonus.teleport.png"
    "/home/connor/extreme-tuxanci/data/images/item.dual.png"
    "/home/connor/extreme-tuxanci/data/images/item.lasser.png"
    "/home/connor/extreme-tuxanci/data/images/item.mine.png"
    "/home/connor/extreme-tuxanci/data/images/item.scatter.png"
    "/home/connor/extreme-tuxanci/data/images/item.tommy.png"
    "/home/connor/extreme-tuxanci/data/images/lasserX.png"
    "/home/connor/extreme-tuxanci/data/images/lasserY.png"
    "/home/connor/extreme-tuxanci/data/images/level4.png"
    "/home/connor/extreme-tuxanci/data/images/mine.png"
    "/home/connor/extreme-tuxanci/data/images/naboj.png"
    "/home/connor/extreme-tuxanci/data/images/panel.png"
    "/home/connor/extreme-tuxanci/data/images/panel_shot.png"
    "/home/connor/extreme-tuxanci/data/images/pauza.png"
    "/home/connor/extreme-tuxanci/data/images/pokus.png"
    "/home/connor/extreme-tuxanci/data/images/radar.png"
    "/home/connor/extreme-tuxanci/data/images/screen_main.png"
    "/home/connor/extreme-tuxanci/data/images/screen_table.png"
    "/home/connor/extreme-tuxanci/data/images/shot.png"
    "/home/connor/extreme-tuxanci/data/images/textfield0.png"
    "/home/connor/extreme-tuxanci/data/images/textfield1.png"
    "/home/connor/extreme-tuxanci/data/images/tux2.png"
    "/home/connor/extreme-tuxanci/data/images/tux4.png"
    "/home/connor/extreme-tuxanci/data/images/tux6.png"
    "/home/connor/extreme-tuxanci/data/images/tux8.png"
    "/home/connor/extreme-tuxanci/data/images/tuxanci.png"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/connor/extreme-tuxanci/cmake/data/images/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
