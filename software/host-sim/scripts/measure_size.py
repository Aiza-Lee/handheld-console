#!/usr/bin/env python3
"""Measure game-core size after a build.

Usage:
    python3 scripts/measure_size.py
    python3 scripts/measure_size.py path/to/build/dir

Prints the per-TU flash estimate (.text + .rodata + .data) for the
game_core static library, plus the .a archive size and the top
symbols by size. Run this after every optimization phase to track
deltas.
"""

import os
import re
import subprocess
import sys


def measure(build_dir: str) -> None:
    obj_dir = os.path.join(build_dir, "game-core/CMakeFiles/game_core.dir/src")
    if not os.path.isdir(obj_dir):
        print(f"no object files in {obj_dir}", file=sys.stderr)
        sys.exit(1)
    obj_files = sorted(
        os.path.join(root, f)
        for root, _, files in os.walk(obj_dir)
        for f in files
        if f.endswith(".cpp.o")
    )
    totals = {"text": 0, "rodata": 0, "data": 0, "bss": 0}
    per_file = []
    for f in obj_files:
        out = subprocess.check_output(["size", "-A", f]).decode()
        sections = {"text": 0, "rodata": 0, "data": 0, "bss": 0}
        for line in out.splitlines():
            m = re.match(r"\.(text|rodata|data|bss)(\.[\w_]+)?\s+(\d+)", line)
            if m:
                sec = m.group(1)
                size = int(m.group(3))
                sections[sec] += size
                totals[sec] += size
        flash = sections["text"] + sections["rodata"] + sections["data"]
        per_file.append((flash, sections, f))
    per_file.sort(reverse=True)
    print("=== Per-TU flash (.text + .rodata + .data) ===")
    for flash, sections, f in per_file:
        print(
            f"  {flash:7d}  "
            f"(text={sections['text']:6d} rodata={sections['rodata']:6d} "
            f"data={sections['data']:4d})  {f}"
        )
    print("---")
    print(f"  Total .text    : {totals['text']:8d} ({totals['text'] / 1024:.1f} KB)")
    print(f"  Total .rodata  : {totals['rodata']:8d} ({totals['rodata'] / 1024:.1f} KB)")
    print(f"  Total .data    : {totals['data']:8d} ({totals['data'] / 1024:.1f} KB)")
    print(f"  Total .bss     : {totals['bss']:8d} (RAM, not flash)")
    flash_total = totals["text"] + totals["rodata"] + totals["data"]
    print(f"  flash estimate : {flash_total:8d} ({flash_total / 1024:.1f} KB)")
    ar_path = os.path.join(build_dir, "game-core/libgame_core.a")
    if os.path.exists(ar_path):
        print(f"  libgame_core.a : {os.path.getsize(ar_path):8d} (with archive overhead)")


if __name__ == "__main__":
    build_dir = sys.argv[1] if len(sys.argv) > 1 else "build/clang-release"
    if not os.path.isabs(build_dir):
        build_dir = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))), build_dir
        )
    measure(build_dir)
