#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
#
# Generates the word lists of the predictive text input.
#
# The words and their frequencies come from the FrequencyWords project
# (https://github.com/hermitdave/FrequencyWords, MIT), word lists built from
# the OpenSubtitles corpus. Every word of the source lists is kept, except for
# the entries that are not words of the language (see the patterns below).
#
# The word lists are written in the PKD1 container, which the engine reads
# straight out of the resources without building any index of its own:
#
#   "PKD1"                     magic
#   count                      quint32, number of words
#   offsets[count + 1]         quint32 each, positions in the word pool
#   pool                       the words, UTF-8, in lookup order
#   frequencies[count]         quint32 each, occurrences in the corpus
#
# The words are sorted by the lookup key (lower case, NFKC, «ё» as «е»), so the
# engine finds a prefix with one binary search over the pool. The values are
# little endian.
#
# Usage:
#   tools/generate-prediction-dictionaries.sh [--top N] [--langs "en ru"] [--out DIR] [--cache DIR]
#
# --top limits the number of words per language (0 keeps all of them). The
# generated files are committed to the repository and compiled into the binary
# (src/prediction.qrc), so this script only has to be re-run when the word
# lists themselves should change.
set -euo pipefail

top=0
langs="en ru"
here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/.." && pwd)"
out_dir="$root/src/prediction/data"
cache_dir="${XDG_CACHE_HOME:-$HOME/.cache}/plasma-keyboard-frequencywords"
base_url="https://raw.githubusercontent.com/hermitdave/FrequencyWords/master/content/2018"

while [ $# -gt 0 ]; do
    case "$1" in
    --top)
        top="$2"
        shift 2
        ;;
    --langs)
        langs="$2"
        shift 2
        ;;
    --out)
        out_dir="$2"
        shift 2
        ;;
    --cache)
        cache_dir="$2"
        shift 2
        ;;
    *)
        echo "Unknown argument: $1" >&2
        exit 2
        ;;
    esac
done

mkdir -p "$out_dir" "$cache_dir"

for lang in $langs; do
    source_file="$cache_dir/${lang}_full.txt"
    if [ ! -s "$source_file" ]; then
        echo "Downloading the $lang word list ..."
        curl --fail --location --silent --show-error \
            --output "$source_file.part" \
            "$base_url/$lang/${lang}_full.txt"
        mv "$source_file.part" "$source_file"
    fi

    python3 - "$lang" "$top" "$source_file" "$out_dir/$lang.bin" <<'PYTHON'
import re
import struct
import sys
import unicodedata

lang, top, source, destination = sys.argv[1], int(sys.argv[2]), sys.argv[3], sys.argv[4]

# What a word of the language looks like: letters, and the separator the
# language actually uses inside a word. A word never starts or ends with a
# separator, so an entry such as "'s" or "i-" is not a word. Hyphenated
# English entries are mostly subtitle interjections ("uh-huh", "mm-hmm") and
# are left out, hyphenated Russian words ("что-то", "из-за") are not.
patterns = {
    "en": re.compile(r"^[a-z]+(?:'[a-z]+)*$"),
    "ru": re.compile(r"^[а-яё]+(?:-[а-яё]+)*$"),
}
pattern = patterns.get(lang)
if pattern is None:
    sys.exit(f"No word pattern for the language \"{lang}\"")

MAX_LENGTH = 32

words = []
frequencies = []
seen = set()
skipped = 0

with open(source, encoding="utf-8") as handle:
    for line in handle:
        parts = line.split()
        if len(parts) < 2:
            continue
        # The lists contain ligatures and compatibility forms; the keyboard
        # types the decomposed characters.
        word = unicodedata.normalize("NFKC", parts[0]).lower()
        if len(word) > MAX_LENGTH or not pattern.match(word):
            skipped += 1
            continue
        if word in seen:
            skipped += 1
            continue
        seen.add(word)
        words.append(word)
        frequencies.append(int(parts[1]))
        if top and len(words) >= top:
            break

# The lookup order: the engine compares the words with a prefix in which «ё»
# is typed as «е», so the words have to be ordered by the same form. Words with
# equal keys ("все" and "всё") stay next to each other, the more frequent first.
order = sorted(range(len(words)), key=lambda index: (words[index].replace("ё", "е").encode("utf-8"), -frequencies[index]))

pool = bytearray()
offsets = [0]
sorted_words = []
sorted_frequencies = []
for index in order:
    pool += words[index].encode("utf-8")
    offsets.append(len(pool))
    sorted_words.append(words[index])
    sorted_frequencies.append(frequencies[index])

with open(destination, "wb") as handle:
    handle.write(b"PKD1")
    handle.write(struct.pack("<I", len(sorted_words)))
    handle.write(struct.pack(f"<{len(offsets)}I", *offsets))
    handle.write(pool)
    handle.write(struct.pack(f"<{len(sorted_frequencies)}I", *sorted_frequencies))

print(f"{lang}: {len(sorted_words)} words ({skipped} entries skipped) -> {destination}")
PYTHON
done
