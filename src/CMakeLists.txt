### Find packages files
find_package(LibV4L2)
set_package_properties(LibV4L2 PROPERTIES
    DESCRIPTION "video4linux support libraries"
    URL "https://linuxtv.org/downloads/v4l-utils/"
    TYPE RUNTIME
    PURPOSE "Required for better webcam support")

option(USE_GPROF "enable gprof library embedding" OFF)

if (USE_GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

if(NOT WIN32)
    find_package(PkgConfig QUIET)
    execute_process(
        COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=mltdatadir mlt-framework
        OUTPUT_VARIABLE MLT_DATADIR
        RESULT_VARIABLE MLT_DATADIR_failed)
    if(NOT MLT_DATADIR_failed)
        string(REGEX REPLACE "[\r\n]" "" MLT_DATADIR "${MLT_DATADIR}")
    endif()

    execute_process(
        COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=meltbin mlt-framework
        OUTPUT_VARIABLE MLT_MELTBIN
        RESULT_VARIABLE MLT_MELTBIN_failed)
    if(NOT MLT_MELTBIN_failed)
        string(REGEX REPLACE "[\r\n]" "" MLT_MELTBIN "${MLT_MELTBIN}")
    endif()
else()
    set(MLT_MELTBIN "melt.exe")
    set(MLT_DATADIR "../share/mlt")
endif()
configure_file(mlt_config.h.in ${CMAKE_BINARY_DIR}/mlt_config.h)

check_include_files(linux/input.h HAVE_LINUX_INPUT_H)
if(HAVE_LINUX_INPUT_H)
    list(APPEND kdenlive_SRCS
        jogshuttle/jogmanager.cpp
        jogshuttle/jogaction.cpp
        jogshuttle/jogshuttle.cpp
        jogshuttle/jogshuttleconfig.cpp)
endif()

### Find source files
add_subdirectory(abstractmodel)
add_subdirectory(assets)
add_subdirectory(audiomixer)
add_subdirectory(bin)
add_subdirectory(capture)
add_subdirectory(dialogs)
add_subdirectory(doc)
add_subdirectory(effects)
add_subdirectory(effectslist)
add_subdirectory(jobs)
add_subdirectory(lib)
add_subdirectory(library)
add_subdirectory(mltcontroller)
add_subdirectory(monitor)
add_subdirectory(onlineresources)
add_subdirectory(profiles)
add_subdirectory(project)
add_subdirectory(qml)
add_subdirectory(scopes)
add_subdirectory(simplekeyframes)
add_subdirectory(timeline2)
add_subdirectory(titler)
add_subdirectory(transitions)
add_subdirectory(utils)
add_subdirectory(widgets)
add_subdirectory(xml)
## top_SRCS
file(GLOB top_SRCS "*.cpp")
if(NOT CRASH_AUTO_TEST)
    list(REMOVE_ITEM top_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp)
endif()
# string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/[^;]+;?" "" top_SRCS "${top_SRCS}")
list(REMOVE_ITEM top_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
list(APPEND kdenlive_SRCS ${top_SRCS})
## Others special cases
kconfig_add_kcfg_files(kdenlive_SRCS kdenlivesettings.kcfgc)
install(FILES kdenlivesettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})
ecm_qt_declare_logging_category(kdenlive_SRCS HEADER kdenlive_debug.h IDENTIFIER KDENLIVE_LOG CATEGORY_NAME org.kde.multimedia.kdenlive)
qt5_add_dbus_adaptor(kdenlive_SRCS org.kdenlive.MainWindow.xml mainwindow.h MainWindow)
## UI's
file(GLOB kdenlive_UIS "ui/*.ui")
ki18n_wrap_ui(kdenlive_UIS ${kdenlive_UIS})
## MOC
qt5_wrap_cpp(kdenlive_MOC definitions.h)
set_property(SOURCE definitions.h PROPERTY SKIP_AUTOMOC ON)

add_library(kdenliveLib STATIC ${kdenlive_SRCS} ${kdenlive_UIS} ${kdenlive_MOC} lib/localeHandling.cpp lib/localeHandling.h)

target_link_libraries(kdenliveLib Qt::WinExtras)

if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	add_definitions("-DDEBUG_BUILD")
	qt5_add_resources(kdenlive_extra_SRCS icons.qrc icon.qrc uiresources.qrc)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options("-BigObj")
	add_definitions("-DDEBUG_BUILD")
	qt5_add_resources(kdenlive_extra_SRCS icons.qrc icon.qrc uiresources.qrc)
	
else()
	find_package(Qt5QuickCompiler)
	qt5_add_resources(kdenlive_extra_SRCS icons.qrc icon.qrc)
	qtquick_compiler_add_resources(UI_FILES uiresources.qrc)
endif()

## Icon for Windows and OSX
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/icons/*-apps-smartip.png")
ecm_add_app_icon(SmartIP-Editor ICONS ${ICONS_SRCS}) # icon name = variable name
add_executable(SmartIP-Editor main.cpp ${kdenlive_extra_SRCS} ${SmartIP-Editor} ${UI_FILES})
target_include_directories(SmartIP-Editor
    PRIVATE ${CMAKE_BINARY_DIR}
    )
target_include_directories(SmartIP-Editor
	SYSTEM PRIVATE ${MLT_INCLUDE_DIR} ${MLTPP_INCLUDE_DIR})
target_link_libraries(SmartIP-Editor kdenliveLib)
install(TARGETS SmartIP-Editor DESTINATION ${BIN_INSTALL_DIR})

install(FILES kdenliveui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kdenlive)


target_include_directories(kdenliveLib
    PRIVATE ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/external
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )
target_include_directories(kdenliveLib
	SYSTEM  PRIVATE ${MLT_INCLUDE_DIR} ${MLTPP_INCLUDE_DIR})
# Adds Qt definitions and include directories, and sets QT_LIBRARIES according
# to the components requested in find_package().
# KDE definitions and include directories *must* always come first, Qt follows
# (to avoid breaking builds when KDE and/or Qt are installed to different
# prefixes).
target_link_libraries(kdenliveLib
    KF5::WidgetsAddons
    KF5::Archive
    KF5::CoreAddons
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::NotifyConfig
    KF5::NewStuff
    KF5::DBusAddons
    KF5::XmlGui
    KF5::GuiAddons
    KF5::Notifications
    KF5::TextWidgets
    KF5::Declarative
    KF5::IconThemes
    KF5::Solid
    Qt5::Svg
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::QuickWidgets
    Qt5::Concurrent
    Qt5::Multimedia
    Qt5::NetworkAuth
    ${OPENGL_LIBRARIES}
    ${OPENGLES_LIBRARIES}
    ${MLT_LIBRARIES}
    ${MLTPP_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    kiss_fft)
if(BUILD_COVERAGE)
  target_link_libraries(kdenliveLib gcov)
endif()
set_property(TARGET kdenliveLib PROPERTY CXX_STANDARD 17)
add_definitions(${qt5gui_definitions} -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -D__LOG_DEBUG)

# Optional deps
if(CRASH_AUTO_TEST)
    target_compile_definitions(kdenliveLib PRIVATE CRASH_AUTO_TEST)
    if(TARGET RTTR::Core)
        target_link_libraries(kdenliveLib RTTR::Core)
    else()
        target_link_libraries(kdenliveLib RTTR::Core_Lib)
    endif()
endif()

if(DRMINGW_FOUND)
    target_compile_definitions(SmartIP-Editor PRIVATE -DUSE_DRMINGW)
    target_include_directories(SmartIP-Editor SYSTEM PRIVATE ${DRMINGW_INCLUDE_DIR})
    target_link_libraries(SmartIP-Editor ${DRMINGW_LIBRARY})
elseif(KF5Crash_FOUND)
    target_compile_definitions(SmartIP-Editor PRIVATE -DKF5_USE_CRASH)
    target_link_libraries(SmartIP-Editor KF5::Crash)
endif()

if(KF5_FILEMETADATA)
    target_compile_definitions(kdenliveLib PRIVATE -DKF5_USE_FILEMETADATA)
    target_link_libraries(kdenliveLib KF5::FileMetaData)
endif()

if(KF5_PURPOSE)
    target_compile_definitions(kdenliveLib PRIVATE -DKF5_USE_PURPOSE)
    target_link_libraries(kdenliveLib KF5::Purpose KF5::PurposeWidgets)
endif()

if(Q_WS_X11)
	target_include_directories(kdenliveLib SYSTEM PRIVATE ${X11_Xlib_INCLUDE_PATH})
    target_link_libraries(kdenliveLib ${X11_LIBRARIES})
endif()

if(SDL2_FOUND)
    target_link_libraries(kdenliveLib ${SDL2_LIBRARY})
elseif(SDL_FOUND)
    target_link_libraries(kdenliveLib ${SDL_LIBRARY})
endif()

if(LIBV4L2_FOUND)
    target_include_directories(kdenliveLib SYSTEM PRIVATE ${LIBV4L2_INCLUDE_DIR})
    target_link_libraries(kdenliveLib ${LIBV4L2_LIBRARY})
    target_compile_definitions(kdenliveLib PRIVATE -DUSE_V4L)
endif()

if(HAVE_LINUX_INPUT_H)
    target_compile_definitions(kdenliveLib PRIVATE -DUSE_JOGSHUTTLE)
    target_link_libraries(kdenliveLib media_ctrl)
endif()

target_link_libraries(kdenliveLib DbgHelp)

if (BUILD_QCH)
    ecm_add_qch(
        Kdenlive_QCH
        NAME SmartIP-Editor
        VERSION ${SMARTIP_EDITOR_VERSION}
        ORG_DOMAIN org.kde
        SOURCE_DIRS
            .
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        TAGFILE_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/share/docs/tags
        QCH_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/share/docs/qch
        COMPONENT Devel
    )
endif()
