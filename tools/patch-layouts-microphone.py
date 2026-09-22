#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
#
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
"""Put the microphone key to the right of the space bar in every layout.

The layouts are copies of the Qt Virtual Keyboard layouts with the keys of this
project in them, so the microphone key is inserted the same way in all of them:
right after the space bar. Running the script twice changes nothing.
"""

from __future__ import annotations

import pathlib
import re
import sys


def closing_brace(text: str, opening: int) -> int:
    depth = 0
    for index in range(opening, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return index + 1
    raise ValueError("unbalanced braces")


def patch(text: str) -> str | None:
    if "MicrophoneKey" in text:
        return None
    match = re.search(r"([ \t]*)SpaceKey\s*\{", text)
    if not match:
        return None
    end = closing_brace(text, text.index("{", match.start()))
    indent = match.group(1)
    block = (
        "\n"
        + indent
        + "PlasmaKeyboard.MicrophoneKey {\n"
        + indent
        + "    weight: normalKeyWidth\n"
        + indent
        + "    Layout.fillWidth: false\n"
        + indent
        + "}"
    )
    return text[:end] + block + text[end:]


def main() -> int:
    root = pathlib.Path(__file__).resolve().parent.parent / "src" / "layouts"
    changed = 0
    skipped = 0
    for layout in sorted(root.glob("*/main.qml")):
        text = layout.read_text(encoding="utf-8")
        patched = patch(text)
        if patched is None:
            skipped += 1
            continue
        layout.write_text(patched, encoding="utf-8")
        changed += 1
    print(f"patched {changed} layouts, skipped {skipped}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
