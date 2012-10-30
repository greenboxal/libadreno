#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

cmake_minimum_required (VERSION 2.8)

project (UnitTest++)

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}/
  )

list (APPEND SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/AssertException.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Checks.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestResults.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestReporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestReporterStdout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ReportAssert.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestList.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TimeConstraint.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestDetails.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MemoryOutStream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredTestReporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DeferredTestResult.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XmlTestReporter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CurrentTest.cpp
  )

if (UNIX)
  list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Posix/SignalTranslator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Posix/TimeHelpers.cpp
    )
elseif (WIN32)
  list(APPEND SOURCES
    Win32/TimeHelpers.cpp
    )
endif()

add_library (UnitTest++ STATIC ${SOURCES})

add_definitions(
  -D_CRT_SECURE_NO_DEPRECATE
  )

if (UNIX)
  set_target_properties(UnitTest++ PROPERTIES
    COMPILE_FLAGS "-g -Wall -W -ansi"
    )
endif(UNIX)
