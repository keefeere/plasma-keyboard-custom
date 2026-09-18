#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
#
# Generates the bigram lists of the predictive text input.
#
# The bigrams are counted in the Tatoeba sentence corpus
# (https://tatoeba.org, CC-BY 2.0 FR). A bigram is a pair of neighbouring words
# of one sentence; the first word is the key the engine looks the prediction up
# with, the second one is what is offered for it.
#
# The sentences come either from the per-language exports of Tatoeba
# (--source tatoeba) or from the mirror of the whole corpus on Hugging Face
# (--source hf), which is a single file with the sentences of every language
# and is used when downloads.tatoeba.org is not reachable.
#
# The lists are written in the PKD2 container, which the engine reads straight
# out of the resources without building any index of its own:
#
#   "PKD2"                      magic
#   keyCount                    quint32, number of keys
#   entryCount                  quint32, number of continuation entries
#   wordCount                   quint32, number of distinct continuation words
#   keyOffsets[keyCount + 1]    quint32 each, positions in the key pool
#   keyPool                     the keys, UTF-8, in lookup order
#   entryOffsets[keyCount + 1]  quint32 each, indices into the entries
#   entryWords[entryCount]      quint32 each, indices into the word pool
#   wordOffsets[wordCount + 1]  quint32 each, positions in the word pool
#   wordPool                    the continuation words, UTF-8, in lookup order
#
# The entries of a key are ordered by frequency, most frequent first, so the
# engine does not need the frequencies themselves. The keys and the words are
# ordered by the lookup key (lower case, NFKC, «ё» as «е»), the same order the
# word lists of the dictionaries use. The values are little endian.
#
# Usage:
#   tools/generate-prediction-ngrams.sh [--source tatoeba|hf] [--top-keys N]
#                                       [--top-continuations N] [--langs "en ru"]
#                                       [--out DIR] [--cache DIR]
#
# --top-keys limits the number of keys per language (the most frequent ones are
# kept), --top-continuations limits how many words are stored per key. The
# generated files are committed to the repository and compiled into the binary
# (src/prediction.qrc), so this script only has to be re-run when the bigram
# lists themselves should change.
set -euo pipefail

source_kind="tatoeba"
top_keys=120000
top_continuations=8
langs="en ru"
here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/.." && pwd)"
out_dir="$root/src/prediction/data"
cache_dir="${XDG_CACHE_HOME:-$HOME/.cache}/plasma-keyboard-tatoeba"
base_url="https://downloads.tatoeba.org/exports/per_language"
hf_url="https://huggingface.co/datasets/loretoparisi/tatoeba-sentences/resolve/main/sentences.csv"

while [ $# -gt 0 ]; do
    case "$1" in
    --source)
        source_kind="$2"
        shift 2
        ;;
    --top-keys)
        top_keys="$2"
        shift 2
        ;;
    --top-continuations)
        top_continuations="$2"
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

case "$source_kind" in
tatoeba | hf) ;;
*)
    echo "Unknown source \"$source_kind\" (tatoeba or hf)" >&2
    exit 2
    ;;
esac

mkdir -p "$out_dir" "$cache_dir"

# The Tatoeba exports are named after the ISO 639-3 code of the language.
corpus_language() {
    case "$1" in
    en) echo "eng" ;;
    ru) echo "rus" ;;
    *)
        echo "No Tatoeba corpus for the language \"$1\"" >&2
        return 1
        ;;
    esac
}

download() {
    # A big file over a slow connection has to be retried, and the bytes that
    # did arrive are kept.
    curl --fail --location --silent --show-error \
        --retry 5 --retry-delay 5 --retry-all-errors --continue-at - \
        --output "$2" "$1"
}

for lang in $langs; do
    corpus="$(corpus_language "$lang")"
    # One sentence per line, which both sources are reduced to before the
    # bigrams are counted.
    sentences="$cache_dir/${lang}_sentences.txt"

    if [ ! -s "$sentences" ]; then
        case "$source_kind" in
        tatoeba)
            archive="$cache_dir/${corpus}_sentences.tsv.bz2"
            if [ ! -s "$archive" ]; then
                echo "Downloading the $corpus sentences from Tatoeba ..."
                download "$base_url/$corpus/${corpus}_sentences.tsv.bz2" "$archive.part"
                mv "$archive.part" "$archive"
            fi
            python3 - "$archive" "$sentences" <<'PYTHON'
import bz2
import sys

archive, destination = sys.argv[1], sys.argv[2]

# The Tatoeba sentence exports are tab separated: id, language, text.
with bz2.open(archive, "rt", encoding="utf-8") as source, open(destination, "w", encoding="utf-8") as handle:
    for line in source:
        parts = line.rstrip("\n").split("\t")
        if len(parts) >= 3:
            handle.write(parts[2].replace("\t", " ") + "\n")
PYTHON
            ;;
        hf)
            archive="$cache_dir/tatoeba-sentences.csv"
            if [ ! -s "$archive" ]; then
                echo "Downloading the Tatoeba sentences from Hugging Face ..."
                download "$hf_url" "$archive.part"
                mv "$archive.part" "$archive"
            fi
            python3 - "$archive" "$corpus" "$sentences" <<'PYTHON'
import sys

archive, language, destination = sys.argv[1], sys.argv[2], sys.argv[3]

# The mirror is one tab separated file with the sentences of every language
# (language code, text); only the wanted language is kept.
with open(archive, encoding="utf-8") as source, open(destination, "w", encoding="utf-8") as handle:
    for line in source:
        parts = line.rstrip("\n").split("\t", 1)
        if len(parts) == 2 and parts[0] == language:
            handle.write(parts[1] + "\n")
PYTHON
            ;;
        esac
    fi

    python3 - "$lang" "$top_keys" "$top_continuations" "$sentences" "$out_dir/$lang.ngrams" <<'PYTHON'
import collections
import re
import struct
import sys
import unicodedata

lang, top_keys, top_continuations, source, destination = (
    sys.argv[1],
    int(sys.argv[2]),
    int(sys.argv[3]),
    sys.argv[4],
    sys.argv[5],
)

# What a word of the language looks like: the same patterns the word lists of
# the dictionaries are built with (tools/generate-prediction-dictionaries.sh).
patterns = {
    "en": re.compile(r"[a-z]+(?:'[a-z]+)*"),
    "ru": re.compile(r"[а-яё]+(?:-[а-яё]+)*"),
}
pattern = patterns.get(lang)
if pattern is None:
    sys.exit(f"No word pattern for the language \"{lang}\"")

MAX_LENGTH = 32


def sentences():
    # The corpus keeps ligatures and compatibility forms; the keyboard types
    # the decomposed characters. Only the words of the language are counted, so
    # that numbers, names in another script and punctuation do not end up in
    # the lists.
    with open(source, encoding="utf-8") as handle:
        for line in handle:
            yield unicodedata.normalize("NFKC", line).lower()


def words(text):
    for word in pattern.findall(text):
        if len(word) <= MAX_LENGTH:
            yield word


# The first pass counts how often every word starts a bigram: those counts
# order the keys, and only the most frequent keys are kept. Counting them
# separately keeps the second pass from having to hold every bigram of the
# corpus at once.
key_counts = collections.Counter()
for sentence in sentences():
    previous = None
    for word in words(sentence):
        if previous is not None:
            key_counts[previous] += 1
        previous = word

keys = [key for key, _ in key_counts.most_common(top_keys)]
chosen = set(keys)

# The second pass counts the words that follow the kept keys.
continuations = {key: collections.Counter() for key in keys}
for sentence in sentences():
    previous = None
    for word in words(sentence):
        if previous is not None and previous in chosen:
            continuations[previous][word] += 1
        previous = word


def lookup_key(word):
    return word.replace("ё", "е").encode("utf-8")


key_order = sorted(keys, key=lookup_key)

# The words offered for a key, most frequent first.
per_key = {}
for key in key_order:
    per_key[key] = [word for word, _ in continuations[key].most_common(top_continuations)]

word_forms = set()
for words_of_key in per_key.values():
    word_forms.update(words_of_key)
word_order = sorted(word_forms, key=lookup_key)
word_index = {word: index for index, word in enumerate(word_order)}

key_pool = bytearray()
key_offsets = [0]
for key in key_order:
    key_pool += key.encode("utf-8")
    key_offsets.append(len(key_pool))

entry_words = []
entry_offsets = [0]
for key in key_order:
    for word in per_key[key]:
        entry_words.append(word_index[word])
    entry_offsets.append(len(entry_words))

word_pool = bytearray()
word_offsets = [0]
for word in word_order:
    word_pool += word.encode("utf-8")
    word_offsets.append(len(word_pool))

with open(destination, "wb") as handle:
    handle.write(b"PKD2")
    handle.write(struct.pack("<III", len(key_order), len(entry_words), len(word_order)))
    handle.write(struct.pack(f"<{len(key_offsets)}I", *key_offsets))
    handle.write(key_pool)
    handle.write(struct.pack(f"<{len(entry_offsets)}I", *entry_offsets))
    handle.write(struct.pack(f"<{len(entry_words)}I", *entry_words))
    handle.write(struct.pack(f"<{len(word_offsets)}I", *word_offsets))
    handle.write(word_pool)

print(
    f"{lang}: {len(key_order)} keys, {len(entry_words)} continuations, "
    f"{len(word_order)} words ({len(key_counts)} keys seen) -> {destination}"
)
PYTHON
done
