#!/usr/bin/env python3
"""Check the real graphics tool using bounded input files and sanitizers."""
from pathlib import Path
import struct
import subprocess
import tempfile

root = Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix="pokered-gfx-") as directory:
    directory = Path(directory)
    executable = directory / "gfx"
    compiler = ["gcc", "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-pedantic",
                "-fsanitize=address,undefined", "-fno-sanitize-recover=all"]
    subprocess.run([*compiler, str(root / "gfx.c"), "-o", str(executable)], check=True)
    counter = 0

    def run(data, depth, arguments, width=None, valid=True, program=executable):
        global counter
        counter += 1
        source = directory / f"input-{counter}"
        output = directory / f"output-{counter}"
        source.write_bytes(data)
        command = [str(program), "--depth", str(depth), *arguments, "--out", str(output)]
        if width is not None:
            png = directory / f"width-{counter}.png"
            png.write_bytes(b"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR" + struct.pack(">I", width))
            command += ["--png", str(png)]
        result = subprocess.run(command + [str(source)], capture_output=True, text=True)
        assert result.returncode == (0 if valid else 1), result.stderr
        if not valid:
            assert not output.exists()
            assert "runtime error" not in result.stderr and "AddressSanitizer" not in result.stderr
            return b""
        return output.read_bytes()

    for depth in (1, 2):
        tile = depth * 8
        a, b = bytes([0x12]) * tile, bytes([0x34]) * tile
        assert run(a + a + b, depth, ["--remove-duplicates"]) == a + b
        assert run(a + bytes([0x48]) * tile, depth, ["--remove-xflip"]) == a
        for width_tiles in (1, 2, 3, 4):
            tiles = [bytes([index + 1]) * tile for index in range(width_tiles * 4)]
            expected = b"".join(tiles[row + column] + tiles[row + width_tiles + column]
                                for row in range(0, len(tiles), width_tiles * 2)
                                for column in range(width_tiles))
            assert run(b"".join(tiles), depth, ["--interleave"], width_tiles * 8) == expected
        for width in (0, 7, 9, 0xffffffff):
            run(a + b, depth, ["--interleave"], width, valid=False)
        run(a + b, depth, ["--interleave"], 16, valid=False)
        run(a + b + a, depth, ["--interleave"], 16, valid=False)
        run(a + b + a + b + b"x", depth, ["--interleave"], 16, valid=False)
    for depth in (0, 3, -1, "1junk"):
        run(bytes(32), depth, [], valid=False)
    assert run(b"", 2, ["--interleave"], 8) == b""

    # Exercise rejection before allocation/read without creating a huge file.
    measurement = directory / "measure.c"
    measurement.write_text("#include <stdio.h>\n#include <limits.h>\n"
                           "long __wrap_ftell(FILE *file) { (void)file; return INT_MAX; }\n")
    capped = directory / "gfx-capped"
    subprocess.run([*compiler, str(root / "gfx.c"), str(measurement), "-Wl,--wrap=ftell",
                    "-o", str(capped)], check=True)
    run(bytes(32), 2, [], valid=False, program=capped)
    print(f"gfx size checks passed: {counter} bounded CLI cases")
