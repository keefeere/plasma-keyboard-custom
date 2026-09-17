# SPDX-FileCopyrightText: 2024 Aleix Pol <aleixpol@kde.org>
# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>
#
# SPDX-License-Identifier: BSD-2-Clause

configure_file(config-plasma-keyboard.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-plasma-keyboard.h)

add_executable(plasma-keyboard-custom)

ecm_add_qtwayland_client_protocol(plasma-keyboard-custom PROTOCOL ${WaylandProtocols_DATADIR}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)
ecm_add_qtwayland_client_protocol(plasma-keyboard-custom PROTOCOL ${WaylandProtocols_DATADIR}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)
ecm_add_qtwayland_client_protocol(plasma-keyboard-custom PROTOCOL "${Wayland_DATADIR}/wayland.xml" BASENAME wayland)

target_sources(plasma-keyboard-custom PRIVATE
    main.cpp
    inputpanelintegration.cpp
    inputpanelintegration.h
    inputpanelrole.h
    clipboard/clipboardhistory.cpp
    clipboard/clipboardhistory.h
    gamepadhandler.cpp
    gamepadhandler.h
    inputlisteneritem.cpp
    inputlisteneritem.h
    inputmethod.cpp
    inputmethod_p.h
    inputpanelwindow.cpp
    inputpanelwindow.h
    inputplugin.cpp
    restartwatcher.cpp
    restartwatcher.h
    touchholdwatcher.cpp
    inputplugin.h
    theme/thememanager.cpp
    theme/thememanager.h
    qwaylandinputpanelshellintegration.cpp
    qwaylandinputpanelshellintegration_p.h
    qwaylandinputpanelsurface.cpp
    qwaylandinputpanelsurface_p.h
    overlay/overlaycontroller.cpp
    overlay/overlaycontroller.h
    overlay/candidatemodel.cpp
    overlay/candidatemodel.h
    overlay/overlaytrigger.cpp
    overlay/overlaytrigger.h
    overlay/longpresstrigger.cpp
    overlay/longpresstrigger.h
    overlay/diacriticsdataloader.cpp
    overlay/diacriticsdataloader.h
    overlay/diacritics.qrc
    overlay/prefixquerytrigger.cpp
    overlay/prefixquerytrigger.h
    overlay/textexpansiontrigger.cpp
    overlay/textexpansiontrigger.h
)

if(PLASMA_KEYBOARD_VIBRATION_ENABLED)
    set_source_files_properties(dbus/org.sigxcpu.Feedback.Haptic.xml PROPERTIES INCLUDE vibrationevent.h)
    qt_add_dbus_interfaces(dbusinterface_SRCS
        dbus/org.sigxcpu.Feedback.Haptic.xml)

    target_sources(plasma-keyboard-custom PRIVATE
        vibration.cpp
        vibration.h
        vibrationevent.h
        ${dbusinterface_SRCS}
    )
endif()

if(PLASMA_KEYBOARD_SOUND_ENABLED)
    target_sources(plasma-keyboard-custom PRIVATE
        soundresources.qrc
    )
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../data/70-plasma-keyboard-touchscreen.rules
        DESTINATION ${KDE_INSTALL_LIBDIR}/udev/rules.d)

ecm_qt_declare_logging_category(plasma-keyboard-custom
    HEADER logging.h
    IDENTIFIER "PlasmaKeyboard"
    CATEGORY_NAME "org.kde.plasma.keyboard.custom"
    DESCRIPTION "Plasma Keyboard"
    DEFAULT_SEVERITY Warning
)

ecm_add_qml_module(plasma-keyboard-custom
    URI "org.kde.plasma.keyboard.custom"
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
    QtQuick
)

ecm_target_qml_sources(plasma-keyboard-custom SOURCES
    qml/main.qml
    qml/LanguagePopup.qml
    qml/LanguagePopupDelegate.qml
    qml/DiacriticsOverlay.qml
    qml/OverlayWindow.qml
)
ecm_finalize_qml_module(plasma-keyboard-custom)

kconfig_add_kcfg_files(plasma-keyboard-custom GENERATE_MOC plasmakeyboardsettings.kcfgc)

target_include_directories(plasma-keyboard-custom PRIVATE
    ${CMAKE_BINARY_DIR}
    # Add directories to include path for QML type registration
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/clipboard
    ${CMAKE_CURRENT_SOURCE_DIR}/overlay
    ${CMAKE_CURRENT_SOURCE_DIR}/theme
)

target_link_libraries(plasma-keyboard-custom
    PRIVATE
    KF6::CoreAddons
    KF6::I18n
    KF6::I18nQml
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::Crash
    KF6::GlobalAccel
    PkgConfig::XKBCommon
    Plasma::PlasmaQuick
    Qt::DBus
    Qt::Gui
    Qt::GuiPrivate
    Qt::VirtualKeyboard
    Qt::WaylandClient
    Qt::WaylandClientPrivate
    Wayland::Client
)

install(TARGETS plasma-keyboard-custom ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.plasma.keyboard.custom.desktop DESTINATION ${KDE_INSTALL_APPDIR})

add_subdirectory(styles)
add_subdirectory(qmlplugin)

install(DIRECTORY layouts DESTINATION ${CMAKE_INSTALL_PREFIX}/share/plasma/keyboard-custom)
install(DIRECTORY overlay/diacritics DESTINATION ${CMAKE_INSTALL_PREFIX}/share/plasma/keyboard-custom)

install(FILES plasmakeyboardsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR} RENAME plasmakeyboardcustomsettings.kcfg)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.plasma.keyboard.custom.metainfo.xml
    DESTINATION ${KDE_INSTALL_METAINFODIR}
)
