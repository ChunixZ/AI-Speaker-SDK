# Install script for directory: /home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/DCSApp/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux"
         RPATH "$ORIGIN/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output" TYPE EXECUTABLE FILES "/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/DCSApp/src/duer_linux")
  if(EXISTS "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux"
         OLD_RPATH "/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/sdk/lib:/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/third/lib:/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/Framework/DeviceIo/src:"
         NEW_RPATH "$ORIGIN/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/aozhou/new_work/wu/R16_D9/tina/package/allwinner/dueros/src/OMtech_buildout/output/duer_linux")
    endif()
  endif()
endif()

