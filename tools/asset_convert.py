#!/usr/bin/env python3
"""
asset_convert.py — 将任意二进制文件转换为 alignas(4) 的 C++ const 数组。

用法:
  asset_convert.py <input_file> <output_cpp> <var_name>

输出:
  - var_name[]   : alignas(4) 的 const unsigned char 数组
  - var_name_len : 数据长度（已对齐到 4 字节边界）
"""

import os
import sys


def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input_file> <output_cpp> <var_name>", file=sys.stderr)
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]
    var_name = sys.argv[3]

    with open(input_path, "rb") as f:
        data = f.read()

    # 补齐到 4 字节对齐（ARM Cortex-M 需要）
    while len(data) % 4 != 0:
        data += b"\x00"

    input_rel = os.path.relpath(input_path)
    lines = [
        f"// Auto-generated from {input_rel}",
        f"// Source size: {os.path.getsize(input_path)} bytes, padded: {len(data)} bytes",
        f'#include <cstdint>',
        f'',
        f'extern "C" alignas(4) const unsigned char {var_name}[] = {{',
    ]

    for i in range(0, len(data), 16):
        chunk = data[i : i + 16]
        hex_bytes = ", ".join(f"0x{b:02x}" for b in chunk)
        lines.append(f"\t{hex_bytes},")

    lines.append("};")
    lines.append(f'extern "C" const unsigned int {var_name}_len = {len(data)};')
    lines.append("")

    with open(output_path, "w") as f:
        f.write("\n".join(lines))


if __name__ == "__main__":
    main()
