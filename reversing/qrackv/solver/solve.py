import itertools

offset = 0x1edb00
memdata = open('./qemu-riscv64', 'rb').read()[offset:offset+1024]
#memdata = bytes.fromhex('9e1fc5b8108125c15fd335bbf85463a818805c4d3fcb6cb1a00b32b474d03e80d8484d3fd68c963411d43fb82affd40a7fc64482be2c6702421e58cef102d321ef92de1923388650ba8ecac6e79c1fc4f97f24c70b2034d53952b734358824e4a1efc63bf5934a1013cff3cfc299299180c48b99d08b1778b8b2ffb8734c7d7e5d6e5164b80ba8cbe95ca26c76cfbbe5f52d179ce7bc7b08c7a0e9ee4f30cb237edebbb60491c0ca4de52ec75ffed6ee2f154cdd2ed830c327a11a818f22550bd235d8454148cbc84a38a1e99d682df6a71c30767035fef238bd5b4b06b901b7d330af68499318442f6275696c642f71')

mem = [int.from_bytes(memdata[i:i+8], 'little') for i in range(0, len(memdata), 8)]

def func1(state):
    a = 0x09282F38FD9DE6BB
    c = 0x09A10A8B923AC8BF
    m = 0xFFFFFFFFFFFFFFC5  # 2^64 - 59
    # モジュラ逆数を計算
    a_inv = pow(a, -1, m)
    # 逆変換を適用
    x_n = (a_inv * (state - c)) % m
    return x_n

def char_check(v):
    chars = 'SECON{0123456789abcdef}'
    return all(map(lambda c: chr(c) in chars, v.to_bytes(8, 'little')))

n = 3
flag = b''
for i in range(0x48 // 8):
    print(hex(mem[n]))

    v = mem[n]
    v = func1(v)
    for x in list(set(itertools.permutations(v.to_bytes(8, 'little')))):
        v2 = int.from_bytes(bytes(x), 'little')
        v2 = func1(v2)

        if char_check(v2):
            flag += v2.to_bytes(8, 'little')

    n = n * 3 % 29

print(flag.decode())
