#!/usr/bin/env python

import EfiCompressor

with open("stage2.efi", "rb") as fh:
    data = bytearray(fh.read())
    data_len = len(data)
    breakpoint
    for i in range(data_len):
        data[i] ^= 0x71

    compressed_data = EfiCompressor.UefiCompress(data, data_len)

with open("rom", "wb") as f:
    f.write(compressed_data)
