cmake_minimum_required(VERSION 3.10.0)

# An odd patch version number means development version, while an even one means
# stable release. An additional number can be used for bugfix-only releases.

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "0")
set (RELEASE_SERVICE_VERSION_MINOR "0")
set (RELEASE_SERVICE_VERSION_MICRO "1")

set(SMARTIP_PWD ${CMAKE_CURRENT_SOURCE_DIR})

set(SMARTIP_EDITOR_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(SmartIP-Editor VERSION ${SMARTIP_EDITOR_VERSION})

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()
if(POLICY CMP0053)
    cmake_policy(SET CMP0053 NEW)
endif()

# To be switched on when releasing.
option(RELEASE_BUILD "Remove Git revision from program version" ON)
option(BUILD_TESTING "Build tests" ON)
option(CRASH_AUTO_TEST "Auto-generate testcases upon some crashes (uses RTTR library, needed for fuzzing)" OFF)
option(BUILD_FUZZING "Build fuzzing target" OFF)

# Minimum versions of main dependencies.
set(MLT_MIN_MAJOR_VERSION 6)
set(MLT_MIN_MINOR_VERSION 20)
set(MLT_MIN_PATCH_VERSION 0)
set(MLT_MIN_VERSION ${MLT_MIN_MAJOR_VERSION}.${MLT_MIN_MINOR_VERSION}.${MLT_MIN_PATCH_VERSION})

# KDE Frameworks
find_package(ECM 5.45.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMInstallIcons)
include(GenerateExportHeader)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMOptionalAddSubdirectory)
include(ECMMarkNonGuiExecutable)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMEnableSanitizers)
include(ECMAddQch)
add_definitions(-DTRANSLATION_DOMAIN=\"kdenlive\")

find_package(KF5 REQUIRED COMPONENTS Archive Bookmarks CoreAddons Config ConfigWidgets
                            DBusAddons KIO WidgetsAddons NotifyConfig NewStuff XmlGui Notifications GuiAddons TextWidgets IconThemes Declarative Solid
                 OPTIONAL_COMPONENTS DocTools FileMetaData Crash Purpose)

# Qt
set(QT_MIN_VERSION 5.11.0)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Widgets Svg Quick QuickControls2 Concurrent QuickWidgets Multimedia NetworkAuth)

add_definitions(-DQT_NO_CAST_TO_ASCII -DQT_NO_URL_CAST_FROM_STRING)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# MLT
find_package(MLT ${MLT_MIN_VERSION} REQUIRED)
set_package_properties(MLT PROPERTIES DESCRIPTION "Multimedia framework"
                URL "https://mltframework.org"
                PURPOSE "Required to do video processing")
message(STATUS "Found MLT++: ${MLTPP_LIBRARIES}")

# Windows
include(CheckIncludeFiles)
check_include_files(malloc.h HAVE_MALLOC_H)
check_include_files(pthread.h HAVE_PTHREAD_H)
if(WIN32)
    find_package(DrMinGW)
	
	find_package(Qt5 REQUIRED COMPONENTS WinExtras)
	include_directories(${Qt5WinExtras_INCLUDE_DIRS})
	add_definitions(${Qt5WinExtras_DEFINITIONS})
	add_compile_definitions(${Qt5WinExtras_COMPILE_DEFINITIONS})
	
    set(MLT_PREFIX "..")
else()
    set(MLT_PREFIX ${MLT_ROOT_DIR})
endif()

if(KF5FileMetaData_FOUND)
  message(STATUS "Found KF5 FileMetadata to extract file metadata")
  set(KF5_FILEMETADATA TRUE)
else()
  message(STATUS "KF5 FileMetadata not found, file metadata will not be available")
endif()

if(KF5Purpose_FOUND)
  message(STATUS "Found KF5 Purpose, filesharing enabled")
  set(KF5_PURPOSE TRUE)
else()
  message(STATUS "KF5 Purpose not found, filesharing disabled")
endif()

if(KF5DocTools_FOUND)
    add_subdirectory(doc)
    kdoctools_install(po)
endif()

# Get current version.
set(SMARTIP_EDITOR_VERSION_STRING "${SMARTIP_EDITOR_VERSION}")
if(NOT RELEASE_BUILD AND EXISTS ${CMAKE_SOURCE_DIR}/.git)
  # Probably a Git workspace; determine the revision.
  find_package(Git QUIET)
  if(GIT_FOUND)
    exec_program(${GIT_EXECUTABLE} ${CMAKE_SOURCE_DIR}
	  ARGS "log -n 1 --pretty=format:\"%h\""
      OUTPUT_VARIABLE KDENLIVE_GIT_REVISION)
    message(STATUS "Kdenlive Git revision: ${KDENLIVE_GIT_REVISION}")
    set(SMARTIP_EDITOR_VERSION_STRING "${SMARTIP_EDITOR_VERSION} (rev. ${KDENLIVE_GIT_REVISION})")
  else()
    message(STATUS "Kdenlive Git revision could not be determined")
  endif()
endif()

if(CRASH_AUTO_TEST)
  find_package(RTTR 0.9.6 QUIET)
  if(NOT RTTR_FOUND)
    message(STATUS "RTTR not found on system, will download source and build it")
    include(rttr.CMakeLists.txt)
  endif()
  if(BUILD_FUZZING)
    set(ECM_ENABLE_SANITIZERS fuzzer;address)
  endif()
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

option(BUILD_QCH "Build source code documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "Source code documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

set(FFMPEG_SUFFIX "" CACHE STRING "FFmpeg custom suffix")
configure_file(config-kdenlive.h.cmake config-kdenlive.h @ONLY)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# Sources
add_subdirectory(src)
add_subdirectory(renderer)
add_subdirectory(thumbnailer)
add_subdirectory(data)
ki18n_install(po)

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS Kdenlive_QCH
        FILE KdenliveQCHTargets.cmake
        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/kdenlive"
        COMPONENT Devel
    )
endif()

include(GNUInstallDirs)
install(FILES AUTHORS COPYING README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

if (ECM_VERSION VERSION_LESS "5.59.0")
    install(FILES kdenlive.categories DESTINATION ${KDE_INSTALL_CONFDIR})
else()
    install(FILES kdenlive.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
endif()

# Tests
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
if(BUILD_FUZZING AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    add_subdirectory(fuzzer)
endif()

