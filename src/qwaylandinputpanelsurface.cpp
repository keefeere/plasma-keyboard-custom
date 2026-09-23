/*
 * Copyright (c) 2017 Jan Arne Petersen
 * SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "inputpanelrole.h"
#include "qwaylandinputpanelsurface_p.h"

#include <QScreen>
#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcQpaShellIntegration, "qt.qpa.wayland.shell")

QWaylandInputPanelSurface::QWaylandInputPanelSurface(struct ::zwp_input_panel_surface_v1 *object, QtWaylandClient::QWaylandWindow *window)
    : QWaylandShellSurface(window)
    , QtWayland::zwp_input_panel_surface_v1(object)
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
    window->applyConfigureWhenPossible();
}

QWaylandInputPanelSurface::~QWaylandInputPanelSurface()
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
    zwp_input_panel_surface_v1_destroy(object());
}

void QWaylandInputPanelSurface::applyConfigure()
{
    const QVariant roleVariant = window()->window()->property("plasmaKeyboardInputPanelRole");
    const int role = roleVariant.isValid() ? roleVariant.toInt() : -1;

    switch (role) {
    case InputPanelRole::OverlayPanel:
        set_overlay_panel();
        break;
    case InputPanelRole::Keyboard:
    default: {
        QtWaylandClient::QWaylandScreen *screen = window()->waylandScreen();
        if (!screen) {
            qCWarning(qLcQpaShellIntegration) << "No Wayland screen available, cannot configure input panel surface";
            return;
        }

        if (QScreen *qScreen = screen->screen()) {
            window()->window()->setScreen(qScreen);
        }

        set_toplevel(screen->output(), position_center_bottom);
        break;
    }
    }

    // The input panel surface is not the keyboard window in this application: it
    // is an invisible one pixel stub that only carries the panel rectangle for
    // the compositor, while the keys are drawn in a layer-shell window of their
    // own (see main.cpp). Announcing it as the active window would take Qt's
    // focus window away from the window that holds the input item, and Qt
    // Virtual Keyboard hides the panel again when the focus object goes away.
}

QT_END_NAMESPACE
