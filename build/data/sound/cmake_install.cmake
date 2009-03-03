# Install script for directory: /home/scarab/tuxanci/data/sound

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
  FILE(INSTALL DESTINATION "/usr/local/share/tuxanci/sound" TYPE FILE FILES
    "/home/scarab/tuxanci/data/sound/tuxanci-smrt.ogg"
    "/home/scarab/tuxanci/data/sound/odraz-gun.ogg"
    "/home/scarab/tuxanci/data/sound/item_gun.ogg"
    "/home/scarab/tuxanci/data/sound/gun_tommy.ogg"
    "/home/scarab/tuxanci/data/sound/gun_scatter.ogg"
    "/home/scarab/tuxanci/data/sound/odraz-l.ogg"
    "/home/scarab/tuxanci/data/sound/explozion.ogg"
    "/home/scarab/tuxanci/data/sound/odraz-r.ogg"
    "/home/scarab/tuxanci/data/sound/item_bonus.ogg"
    "/home/scarab/tuxanci/data/sound/gun2.ogg"
    "/home/scarab/tuxanci/data/sound/gun_revolver.ogg"
    "/home/scarab/tuxanci/data/sound/dead.ogg"
    "/home/scarab/tuxanci/data/sound/switch_gun.ogg"
    "/home/scarab/tuxanci/data/sound/put_mine.ogg"
    "/home/scarab/tuxanci/data/sound/teleport.ogg"
    "/home/scarab/tuxanci/data/sound/smrt.ogg"
    "/home/scarab/tuxanci/data/sound/end.ogg"
    "/home/scarab/tuxanci/data/sound/gun_lasser.ogg"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

