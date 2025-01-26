from ptrlib import *


flag = b"SECCON{UPX_s7ub_1s_a_g0od_pl4c3_f0r_h1din6_c0d3}\n"

print(len(flag))

key  = p64(0x49f9830000004ae8) + p64(0x374c8d4857534475)
key += p64(0x39482feb5b565efd) + p64(0x803cac5e563273ce)
key += p64(0x7e8006778f3c0a72) + p64(0x013ce82c06740ffe)
key += p64(0x561673ce3948e477) + p64(0xc80f5fdf75d028ad)

print(", ".join(map(lambda c: f"0x{c:02x}", key)))

out = xor(flag, key)
print(", ".join(map(lambda c: f"0x{c:02x}", out)))

