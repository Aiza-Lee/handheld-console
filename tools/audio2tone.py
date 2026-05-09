#!/usr/bin/env python3
"""
audio2tone.py — 将人类可读的音符描述文件转换为 C++ Tone 数组。

用法:
  audio2tone.py <input.tone> <output.cpp> <array_name>

输入格式 (.tone):
  # 注释以 # 开头
  # 支持三种行格式:

  440 200           → 原始频率 (Hz) + 持续时间 (ms)
  REST 100          → 静音 (等同于频率 0)
  C4 200            → 音符名称 + 八度 + 持续时间

音符名称: C, C#, D, D#, E, F, F#, G, G#, A, A#, B
八度范围: 0-8 (C4 = 中央 C = 262 Hz)
持续时间: 毫秒

示例 (assets/sounds/jingle.tone):
  # 简单音阶上行
  C4 200
  D4 200
  E4 200
  F4 200
  G4 400
  REST 100
  G3 200
  C4 400
"""

import os
import re
import sys

# 音符名称 → 半音数 (C=0, C#=1, ..., B=11)
_NOTES = {
    "C": 0, "C#": 1, "Db": 1,
    "D": 2, "D#": 3, "Eb": 3,
    "E": 4, "Fb": 4,
    "F": 5, "F#": 6, "Gb": 6,
    "G": 7, "G#": 8, "Ab": 8,
    "A": 9, "A#": 10, "Bb": 10,
    "B": 11, "Cb": 11,
}

# 正则: 音符名(可选升降) + 八度 + 持续时间
_NOTE_RE = re.compile(r"^([A-G][#b]?)(\d)\s+(\d+)$")
# 正则: REST + 持续时间
_REST_RE = re.compile(r"^REST\s+(\d+)$", re.IGNORECASE)
# 正则: 原始频率 + 持续时间 (两个数字)
_RAW_RE = re.compile(r"^(\d+)\s+(\d+)$")


def note_to_freq(name: str, octave: int) -> int:
    """将音符名称和八度转换为频率 (Hz)。"""
    semitone = _NOTES[name] + (octave + 1) * 12
    return int(440.0 * (2.0 ** ((semitone - 69) / 12.0)))


def parse_tone_file(path: str) -> list[tuple[int, int]]:
    """解析 .tone 文件，返回 [(freq, dur_ms), ...] 列表。"""
    tones: list[tuple[int, int]] = []
    line_num = 0

    with open(path) as f:
        for raw in f:
            line_num += 1
            line = raw.split("#")[0].strip()  # 去掉注释和首尾空白
            if not line:
                continue

            m = _NOTE_RE.match(line)
            if m:
                name, oct_str, dur_str = m.group(1), m.group(2), m.group(3)
                try:
                    freq = note_to_freq(name, int(oct_str))
                    tones.append((freq, int(dur_str)))
                    continue
                except KeyError:
                    print(f"Warning: {path}:{line_num}: unknown note '{name}', skipping", file=sys.stderr)
                    continue

            m = _REST_RE.match(line)
            if m:
                tones.append((0, int(m.group(1))))
                continue

            m = _RAW_RE.match(line)
            if m:
                tones.append((int(m.group(1)), int(m.group(2))))
                continue

            print(f"Warning: {path}:{line_num}: unrecognized line, skipping: {line!r}", file=sys.stderr)

    return tones


def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input.tone> <output.cpp> <array_name>", file=sys.stderr)
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]
    array_name = sys.argv[3]

    if not os.path.exists(input_path):
        print(f"Error: input file not found: {input_path}", file=sys.stderr)
        sys.exit(1)

    tones = parse_tone_file(input_path)

    input_rel = os.path.relpath(input_path)
    lines = [
        f"// Auto-generated from {input_rel}",
        f"// {len(tones)} tone(s)",
        f'#include "platform/interfaces/IAudio.h"',
        "",
        f'extern "C" const handheld::Tone {array_name}[] = {{',
    ]

    for freq, dur in tones:
        lines.append(f"\t{{{freq}, {dur}}},")

    lines.append("};")
    lines.append(f'extern "C" const uint32_t {array_name}_count = {len(tones)};')
    lines.append("")

    with open(output_path, "w") as f:
        f.write("\n".join(lines))

    print(f"Generated {len(tones)} tone(s) → {output_path}")


if __name__ == "__main__":
    main()
