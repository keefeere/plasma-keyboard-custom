# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

add_definitions(-DQT_NO_KEYWORDS)

kcmutils_add_qml_kcm(kcm_plasmakeyboardcustom)

kconfig_add_kcfg_files(kcm_plasmakeyboardcustom GENERATE_MOC ../src/plasmakeyboardsettings.kcfgc)

target_sources(kcm_plasmakeyboardcustom PRIVATE
    plasmakeyboardkcm.cpp
    plasmakeyboardkcm.h
    ../src/theme/thememanager.cpp
    ../src/theme/thememanager.h
)

target_include_directories(kcm_plasmakeyboardcustom PRIVATE
    ../src/theme
)

target_link_libraries(kcm_plasmakeyboardcustom PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    KF6::CoreAddons
    KF6::I18n
    KF6::KCMUtilsQuick
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::GlobalAccel
)
