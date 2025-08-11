#!/usr/bin/env python3

from __future__ import annotations
from splat.segtypes.common.header import CommonSegHeader

# the only reason this exists is because the zero bytes past the sony string are not zero 
# which breaks the vanilla psx header parser

def get_lines(typ, data, comment):
    dstr = ""
    for i in range(len(data)):
        dstr += "0x" + hex(data[i])[2:].upper() + ("" if (i == (len(data) - 1)) else ", ")
    
    dstr = dstr.ljust(20 - len(typ))
    return f".{typ} {dstr} /* {comment} */"



class PSXSegHeaderproto(CommonSegHeader):
    # little endian so reverse words, TODO: use struct.unpack("<i",...) ?
    # breakdown from https://psx-spx.consoledev.net/cdromdrive/#filenameexe-general-purpose-executable
    def parse_header(self, rom_bytes):
        header_lines = []
        header_lines.append(".section .data\n")
        header_lines.append(
            self.get_line("ascii", rom_bytes[0x00:0x08], "Magic number")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x08:0x0C][::-1], ".text vram address")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x0C:0x10][::-1], ".data vram address")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x10:0x14][::-1], "Initial PC")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x14:0x18][::-1], "Initial $gp/r28")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x18:0x1C][::-1], ".text start")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x1C:0x20][::-1], ".text size")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x20:0x24][::-1], ".data start")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x24:0x28][::-1], ".data size")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x28:0x2C][::-1], ".bss start")
        )
        header_lines.append(
            self.get_line("word", rom_bytes[0x2C:0x30][::-1], ".bss size")
        )
        header_lines.append(
            self.get_line(
                "word", rom_bytes[0x30:0x34][::-1], "Initial $sp/r29 & $fp/r30 base"
            )
        )
        header_lines.append(
            self.get_line(
                "word", rom_bytes[0x34:0x38][::-1], "Initial $sp/r29 & $fp/r30 offset"
            )
        )
        header_lines.append(self.get_line("word", rom_bytes[0x38:0x3C], "Reserved"))
        header_lines.append(self.get_line("word", rom_bytes[0x3C:0x40], "Reserved"))
        header_lines.append(self.get_line("word", rom_bytes[0x40:0x44], "Reserved"))
        header_lines.append(self.get_line("word", rom_bytes[0x44:0x48], "Reserved"))
        header_lines.append(self.get_line("word", rom_bytes[0x48:0x4C], "Reserved"))
        header_lines.append(
            self.get_line("ascii", rom_bytes[0x4C:0x7C], "Sony Inc")
        )
        assert isinstance(self.rom_end, int)
        header_lines.append(get_lines("byte", rom_bytes[0x7C : self.rom_end], "Reserved"))

        header_lines.append("")

        return header_lines
