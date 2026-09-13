# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

ecm_add_qml_module(breezestyle URI QtQuick.VirtualKeyboard.Styles.Breeze GENERATE_PLUGIN_SOURCE DEPENDENCIES QtQuick)

ecm_target_qml_sources(breezestyle SOURCES
    breeze/style.qml
)

target_link_libraries(breezestyle
    PUBLIC
        Qt::Core
    PRIVATE
        Qt::Qml
        Qt::Quick
)

ecm_finalize_qml_module(breezestyle)

# Also install the style under a unique name. Qt Virtual Keyboard resolves the
# style by scanning the QML import paths and prefers the system-installed
# "Breeze" style, so a distinct name guarantees the version built from this
# repository is the one that gets loaded.
install(FILES breeze/style.qml DESTINATION ${KDE_INSTALL_QMLDIR}/QtQuick/VirtualKeyboard/Styles/PlasmaBreeze)
