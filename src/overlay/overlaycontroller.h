/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "candidatemodel.h"
#include "overlaytrigger.h"

#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <QVirtualKeyboardInputEngine>
#include <qqmlintegration.h>

#include <xkbcommon/xkbcommon-compose.h>

class InputPlugin;

/**
 * Central controller for overlay popups (diacritics, emoji, text expansion).
 *
 * Manages trigger registration, event dispatch, overlay lifecycle, and candidate population.
 * This class bridges C++ input handling with QML overlay views.
 *
 * Usage:
 * 1. Register triggers with registerTrigger()
 * 2. Feed input events via processKeyPress/Release/PreeditChanged/TextCommitted
 * 3. Connect to overlayVisibleChanged signal in QML
 * 4. Call commitCandidate() when user selects an option
 */
class OverlayController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("OverlayController is created in C++ and passed to QML.")

    /**
     * Whether an overlay popup is currently shown.
     */
    Q_PROPERTY(bool overlayVisible READ overlayVisible NOTIFY overlayVisibleChanged)

    /**
     * The trigger ID of the currently active overlay.
     */
    Q_PROPERTY(QString activeTriggerId READ activeTriggerId NOTIFY activeTriggerIdChanged)

    /**
     * The base text that triggered the overlay (e.g., "a" for diacritics).
     */
    Q_PROPERTY(QString pendingText READ pendingText NOTIFY pendingTextChanged)

    /**
     * Model of candidates for the current overlay.
     */
    Q_PROPERTY(CandidateModel *candidateModel READ candidateModel CONSTANT)

    /**
     * Whether the current overlay is a list of alternate characters that was
     * opened without anything being typed (the gamepad case). The view then
     * highlights alternateSelection itself and the command buttons of the
     * gamepad commit and dismiss the selection.
     */
    Q_PROPERTY(bool alternatesOnly READ alternatesOnly NOTIFY alternatesOnlyChanged)

    /**
     * Index of the character selected in the alternates overlay, -1 when none
     * is selected.
     */
    Q_PROPERTY(int alternateSelection READ alternateSelection NOTIFY alternateSelectionChanged)

public:
    explicit OverlayController(InputPlugin *inputPlugin, QObject *parent = nullptr);
    ~OverlayController() override;

    /**
     * Register a trigger strategy.
     *
     * Takes ownership of the trigger. Multiple triggers can be active simultaneously;
     * they are evaluated in registration order.
     *
     * @param trigger The trigger to register.
     */
    void registerTrigger(OverlayTrigger *trigger);

    /**
     * Process a key press event.
     *
     * @param event The key event.
     * @return True if the event was consumed.
     */
    bool processKeyPress(QKeyEvent *event);

    /**
     * Process a key release event.
     *
     * @param event The key event.
     * @return True if the event was consumed.
     */
    bool processKeyRelease(QKeyEvent *event);

    /**
     * Process a preedit text change.
     *
     * @param preedit The new preedit text.
     * @return True if an overlay action was triggered.
     */
    bool processPreeditChanged(const QString &preedit);

    /**
     * Process committed text.
     *
     * @param text The committed text.
     * @return True if an overlay action was triggered.
     */
    bool processTextCommitted(const QString &text);

    bool overlayVisible() const;
    QString activeTriggerId() const;
    QString pendingText() const;
    CandidateModel *candidateModel() const;
    bool alternatesOnly() const;
    int alternateSelection() const;

    /**
     * Pending native scan code for release matching.
     */
    quint32 pendingNativeScanCode() const;

    /**
     * Get the associated InputPlugin for commits.
     */
    InputPlugin *inputPlugin() const;

    /**
     * Hand the Qt Virtual Keyboard input engine to the controller.
     *
     * Characters picked in the alternates overlay are inserted as key clicks
     * through this engine, the way the on-screen alternate-keys popup inserts
     * them, instead of a bare commit_string.
     */
    void setInputEngine(QVirtualKeyboardInputEngine *engine);

public Q_SLOTS:
    /**
     * Insert one alternate character of the highlighted key.
     *
     * Used when the key offers a single alternate, so no list is shown and the
     * character goes in right away.
     *
     * @param text The character to insert.
     */
    Q_INVOKABLE void commitAlternate(const QString &text);

    /**
     * Commit the candidate at the given index.
     *
     * @param index Row index in the candidate model.
     */
    void commitCandidate(int index);

    /**
     * Commit arbitrary text.
     *
     * @param text The text to commit.
     */
    void commitText(const QString &text);

    /**
     * Cancel the current overlay without committing.
     *
     * The base character that was committed on key-press remains in the text field
     * unchanged. No text modifications are performed by this method.
     */
    void cancelOverlay();

    /**
     * Notify the controller that the surrounding text (and therefore cursor position) has
     * changed.
     *
     * Called from InputListenerItem whenever the compositor reports a surrounding-text
     * update. The controller distinguishes changes caused by its own commit_string
     * operations from external cursor movements (e.g. the user tapping elsewhere in the
     * text field). If an external cursor movement is detected while an overlay or hold
     * timer is active, the overlay/timer is cancelled since it is no longer relevant to
     * the new cursor position.
     */
    void handleSurroundingTextChanged();

    /**
     * Open the overlay with the given trigger and candidates.
     *
     * @param triggerId The trigger that activated.
     * @param baseText The base/pending text.
     * @param candidates The candidate options.
     */
    void openOverlay(const QString &triggerId, const QString &baseText, const QStringList &candidates);

    /**
     * Offer the given alternate characters of the key that currently has the
     * focus on the on-screen keyboard.
     *
     * Used by the gamepad, which cannot produce a key event for the pressed
     * button: the characters come from the layout of the highlighted key (its
     * alternativeKeys), which is exactly what a long press on that key shows.
     * Nothing is typed, so no character has to be deleted when a candidate is
     * picked afterwards.
     *
     * @param alternates The characters to offer.
     * @return True if the overlay was opened.
     */
    Q_INVOKABLE bool openAlternates(const QStringList &alternates);

    /**
     * Move the selection inside the alternates overlay that was opened with
     * openAlternates().
     *
     * @param key Qt::Key_Left/Right/Up/Down to move, Qt::Key_Return to pick
     *            the selected character, Qt::Key_Escape to dismiss.
     */
    Q_INVOKABLE void navigateAlternates(int key);

Q_SIGNALS:
    /**
     * Emitted when an overlay should be shown.
     *
     * @param triggerId Which trigger type is active.
     * @param baseText The text that triggered the overlay.
     */
    void overlayRequested(const QString &triggerId, const QString &baseText);

    void overlayVisibleChanged();
    void activeTriggerIdChanged();
    void pendingTextChanged();
    void alternatesOnlyChanged();
    void alternateSelectionChanged();

    /**
     * Emitted when a navigation key (arrow or Enter) is pressed while the overlay is visible.
     *
     * @param key The Qt key code (e.g. Qt::Key_Left, Qt::Key_Return).
     */
    void overlayNavigationKeyPressed(int key);

private Q_SLOTS:
    void handleTimerExpired();
    void handleOverlayGraceTimer();

private:
    void executeAction(const OverlayTriggerResult &result, OverlayTrigger *trigger);
    void resetState();
    void setOverlayVisible(bool visible);

    InputPlugin *m_inputPlugin = nullptr;

    /**
     * The Qt Virtual Keyboard input engine, when the panel handed one over.
     *
     * Used to insert a picked alternate character as a key click, so a client
     * sees ordinary typing and keeps the input session alive.
     */
    QVirtualKeyboardInputEngine *m_inputEngine = nullptr;
    QList<OverlayTrigger *> m_triggers;
    CandidateModel *m_candidateModel = nullptr;

    QTimer m_holdTimer;
    QTimer m_overlayGraceTimer;
    bool m_overlayVisible = false;
    QString m_activeTriggerId;
    QString m_pendingText;
    quint32 m_pendingNativeScanCode = 0;
    bool m_swallowNextRelease = false;
    quint32 m_ignoreReleaseNativeScanCode = 0;

    /**
     * Whether the overlay currently shown is a list of alternate characters
     * with nothing typed before it (the gamepad opens it for the highlighted
     * key instead of for a pressed one).
     */
    bool m_alternatesOnly = false;

    /** Index of the character selected in that list, -1 when none is. */
    int m_alternateSelection = -1;

    /**
     * Tracks whether the pending key was released while the overlay was still open.
     *
     * This scenario is likely when the user long-presses a key to open the overlay, then
     * releases the key after the overlay is shown but before selecting a candidate.
     */
    bool m_pendingKeyReleased = false;

    /** The trigger that is currently timing (for long-press). */
    OverlayTrigger *m_pendingTrigger = nullptr;

    /**
     * Native scan code of the key currently being repeated. 0 when idle.
     */
    quint32 m_repeatNativeScanCode = 0;

    /**
     * Number of compositor surrounding-text echo events the controller is still
     * waiting to receive as a consequence of its own commit_string operations.
     *
     * Only commit_string causes the compositor to send a surrounding_text
     * event back to the input method. delete_surrounding_text does not.
     *
     * While this counter is non-zero the next incoming surrounding_text event
     * is treated as self-caused and the counter is decremented rather than
     * cancelling the active overlay.
     *
     * When the counter reaches zero, m_surroundingTextSettleTimer is used as
     * a fallback: any surrounding_text event that arrives while the timer is
     * still running is also treated as a self-caused echo. This handles clients
     * that send more than one update per commit_string (e.g. Firefox, Chromium,
     * VS Code), where a cursor-position comparison is unreliable because those
     * clients report a shifted surrounding-text window after text insertion.
     */
    int m_pendingSurroundingTextUpdates = 0;

    /**
     * Single-shot timer that suppresses spurious surrounding_text echoes after
     * a commit_string.
     *
     * Some clients (Firefox, Chromium, VS Code, …) send two or more
     * surrounding_text events in response to a single commit_string. Once the
     * primary credit counter (m_pendingSurroundingTextUpdates) is exhausted,
     * events arriving while this timer is still active are treated as
     * self-caused echoes rather than external cursor moves.
     *
     * The interval (see SURROUNDING_TEXT_SETTLE_DELAY_MS) is chosen to be:
     *   - long enough for all compositor echoes to arrive (Wayland roundtrips
     *     on a local session are typically < 5 ms)
     *   - short enough not to mask genuine user interactions (well under
     *     the long-press threshold)
     */
    QTimer m_surroundingTextSettleTimer;

    /**
     * XKB compose state machine for handling client-side compose sequences (e.g.
     * Multi_key + t + m → ™). The controller processes compose sequences locally so that
     * the intermediate keys (e.g. Multi_key, t) can be consumed and not forwarded to the
     * client, while only the final composed result (e.g. ™) is sent via commit_string.
     */
    xkb_context *m_xkbContext = nullptr;

    /**
     * XKB compose table compiled from the system locale, used to initialize the compose state.
     */
    xkb_compose_table *m_xkbComposeTable = nullptr;

    /**
     * XKB compose state initialized from the compose table, used to track the current
     * compose sequence and produce the final composed result.
     */
    xkb_compose_state *m_xkbComposeState = nullptr;
};
