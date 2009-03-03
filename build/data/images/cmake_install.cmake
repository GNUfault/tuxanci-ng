# Install script for directory: /home/scarab/tuxanci/data/images

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/tuxanci/images" TYPE FILE FILES
    "/home/scarab/tuxanci/data/images/textfield0.png"
    "/home/scarab/tuxanci/data/images/radar.png"
    "/home/scarab/tuxanci/data/images/explosion.png"
    "/home/scarab/tuxanci/data/images/item.lasser.png"
    "/home/scarab/tuxanci/data/images/tux4.png"
    "/home/scarab/tuxanci/data/images/mine.png"
    "/home/scarab/tuxanci/data/images/tuxanci.png"
    "/home/scarab/tuxanci/data/images/tux6.png"
    "/home/scarab/tuxanci/data/images/icon.bonus.shot.png"
    "/home/scarab/tuxanci/data/images/icon.gun.tommy.png"
    "/home/scarab/tuxanci/data/images/item.bonus.shot.png"
    "/home/scarab/tuxanci/data/images/pauza.png"
    "/home/scarab/tuxanci/data/images/naboj.png"
    "/home/scarab/tuxanci/data/images/check.png"
    "/home/scarab/tuxanci/data/images/cross.png"
    "/home/scarab/tuxanci/data/images/big-explosion.png"
    "/home/scarab/tuxanci/data/images/icon.bonus.teleport.png"
    "/home/scarab/tuxanci/data/images/icon.bonus.ghost.png"
    "/home/scarab/tuxanci/data/images/item.scatter.png"
    "/home/scarab/tuxanci/data/images/icon.gun.simple.png"
    "/home/scarab/tuxanci/data/images/icon.bonus.speed.png"
    "/home/scarab/tuxanci/data/images/item.bombball.png"
    "/home/scarab/tuxanci/data/images/item.dual.png"
    "/home/scarab/tuxanci/data/images/textfield1.png"
    "/home/scarab/tuxanci/data/images/item.bonus.4x.png"
    "/home/scarab/tuxanci/data/images/button0.png"
    "/home/scarab/tuxanci/data/images/screen_main.png"
    "/home/scarab/tuxanci/data/images/level4.png"
    "/home/scarab/tuxanci/data/images/item.bonus.ghost.png"
    "/home/scarab/tuxanci/data/images/item.mine.png"
    "/home/scarab/tuxanci/data/images/lasserX.png"
    "/home/scarab/tuxanci/data/images/item.bonus.hidden.png"
    "/home/scarab/tuxanci/data/images/screen_table.png"
    "/home/scarab/tuxanci/data/images/shot.png"
    "/home/scarab/tuxanci/data/images/item.tommy.png"
    "/home/scarab/tuxanci/data/images/button1.png"
    "/home/scarab/tuxanci/data/images/icon.gun.dual.simple.png"
    "/home/scarab/tuxanci/data/images/chat.png"
    "/home/scarab/tuxanci/data/images/icon.gun.bombball.png"
    "/home/scarab/tuxanci/data/images/panel_shot.png"
    "/home/scarab/tuxanci/data/images/item.bonus.speed.png"
    "/home/scarab/tuxanci/data/images/icon.bonus.4x.png"
    "/home/scarab/tuxanci/data/images/tux8.png"
    "/home/scarab/tuxanci/data/images/icon.gun.lasser.png"
    "/home/scarab/tuxanci/data/images/panel.png"
    "/home/scarab/tuxanci/data/images/bombball.png"
    "/home/scarab/tuxanci/data/images/choice.png"
    "/home/scarab/tuxanci/data/images/icon.gun.scatter.png"
    "/home/scarab/tuxanci/data/images/lasserY.png"
    "/home/scarab/tuxanci/data/images/graf.bonus.png"
    "/home/scarab/tuxanci/data/images/in_a_captivity_of_8-bit.png"
    "/home/scarab/tuxanci/data/images/pokus.png"
    "/home/scarab/tuxanci/data/images/icon.bonus.hidden.png"
    "/home/scarab/tuxanci/data/images/item.bonus.teleport.png"
    "/home/scarab/tuxanci/data/images/tux2.png"
    "/home/scarab/tuxanci/data/images/icon.gun.mine.png"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

