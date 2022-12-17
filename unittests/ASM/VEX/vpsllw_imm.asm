%ifdef CONFIG
{
  "HostFeatures": ["AVX"],
  "RegData": {
    "XMM2": ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM3": ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM4": ["0x4200440046004800", "0x5200540056005800", "0x0000000000000000", "0x0000000000000000"],
    "XMM5": ["0xC2C4C6C8CACCCED0", "0xE2E4E6E8EAECEEF0", "0x0000000000000000", "0x0000000000000000"],
    "XMM6": ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM7": ["0x0000000000000000", "0x0000000000000000", "0x0000000000000000", "0x0000000000000000"],
    "XMM8": ["0x4200440046004800", "0x5200540056005800", "0x4200440046004800", "0x5200540056005800"],
    "XMM9": ["0xC2C4C6C8CACCCED0", "0xE2E4E6E8EAECEEF0", "0xC2C4C6C8CACCCED0", "0xE2E4E6E8EAECEEF0"]
  },
  "MemoryRegions": {
    "0x100000000": "4096"
  }
}
%endif

lea rdx, [rel .data]

vmovapd ymm0, [rdx]
vmovapd ymm1, [rdx + 32]

vpsllw xmm2, xmm0, 32
vpsllw xmm3, xmm1, 16
vpsllw xmm4, xmm0, 8
vpsllw xmm5, xmm1, 1

vpsllw ymm6, ymm0, 32
vpsllw ymm7, ymm1, 16
vpsllw ymm8, ymm0, 8
vpsllw ymm9, ymm1, 1

hlt

align 32
.data:
dq 0x4142434445464748
dq 0x5152535455565758
dq 0x4142434445464748
dq 0x5152535455565758

dq 0x6162636465666768
dq 0x7172737475767778
dq 0x6162636465666768
dq 0x7172737475767778