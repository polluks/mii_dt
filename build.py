#!/usr/bin/env python3
"""
Build helper for mii.datatype Amiga 12-colour picture datatype.

Generates IFF descriptor files in devs/datatypes/
On Amiga, run smakefile to build the datatype binary.
"""

import struct
import os

PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))


def make_iff_descriptor(name, magic_bytes, datatype_name, superclass=b"pictpgm\x00"):
    buf = bytearray()

    buf += b"FORM"
    buf += struct.pack(">I", 0)
    buf += b"NDTYP"

    name_data = name.encode("ascii")
    buf += b"NAME"
    buf += struct.pack(">I", len(name_data))
    buf += name_data
    if len(name_data) % 2:
        buf += b"\x00"

    dthd = bytearray()
    dthd += struct.pack(">I", 0)
    dthd += struct.pack(">I", 28)
    dthd += struct.pack(">I", len(magic_bytes))
    dthd += struct.pack(">I", 0)
    dthd += superclass[:8].ljust(8, b"\x00")
    dthd += struct.pack(">H", 0)
    dthd += struct.pack(">B", 0)
    dthd += struct.pack(">B", 0)
    dthd += magic_bytes

    dt_name = datatype_name.encode("ascii") + b"\x00"
    dthd += dt_name
    if len(dthd) % 2:
        dthd += b"\x00"

    buf += b"DTHD"
    buf += struct.pack(">I", len(dthd))
    buf += dthd

    form_size = len(buf) - 8
    buf[4:8] = struct.pack(">I", form_size)

    return bytes(buf)


def main():
    desc_dir = os.path.join(PROJECT_DIR, "devs", "datatypes")
    os.makedirs(desc_dir, exist_ok=True)

    desc = make_iff_descriptor("Mii", b"RNCD", "mii.datatype")
    with open(os.path.join(desc_dir, "Mii"), "wb") as f:
        f.write(desc)
    print(f"Created Mii descriptor ({len(desc)} bytes) - matches RNCD magic")

    desc_db = make_iff_descriptor("Mii", b"RNOD", "mii.datatype")
    with open(os.path.join(desc_dir, "Mii_DB"), "wb") as f:
        f.write(desc_db)
    print(f"Created Mii_DB descriptor ({len(desc_db)} bytes) - matches RNOD magic")

    print("\nTo build on Amiga with SAS/C:")
    print("  cd dt_source")
    print("  smake")
    print("\nThen copy to system:")
    print("  Copy devs/datatypes/Mii devs:datatypes/Mii")
    print("  Copy devs/datatypes/Mii_DB devs:datatypes/Mii_DB")
    print("  Copy dt_source/mii.datatype SYS:Classes/Datatypes/mii.datatype")


if __name__ == "__main__":
    main()
