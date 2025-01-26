import os

set_verbose(0)
os.environ['PWNLIB_NOTERM'] = '1'
os.environ['TERM'] = 'linux'

from Crypto.Cipher import AES
import secrets
from pwn import remote
from logger import logger

F.<a> = GF(2**128, modulus=x**128 + x**7 + x**2 + x + 1)

def to_poly(x):
    bs = Integer(int.from_bytes(x, "big")).bits()[::-1]
    return F([0] * (128 - len(bs)) + bs)

def to_bytes(x):
    v = int(bin(x.integer_representation())[2:].zfill(128)[::-1], 2)
    return v.to_bytes(128 // 8, "big")

def encrypt(number, plaintext):
    logger.info(io.recvuntil(">")) # summon or dualsummon
    io.sendline('1')
    logger.info(io.recvuntil(">")) # number
    io.sendline(str(number))
    logger.info(io.recvuntil(">")) # plaintext
    io.sendline(plaintext.hex())
    io.recvline()
    tag = bytes.fromhex(io.recvline().split(b"=")[1].strip().decode('utf-8'))
    return b"", tag

L = a^120
io = remote(os.getenv("SECCON_HOST"), int(os.getenv("SECCON_PORT")))
plaintext1 = b"a"*16
plaintext2 = b"a"*15 + b"b"
c1, t1 = encrypt(1, plaintext1)
c2, t2 = encrypt(1, plaintext2)
t1, t2 = to_poly(t1), to_poly(t2)
c1, c2 = to_poly(c1), to_poly(c2)
H1 = ((t1+t2) / (a^127 + a^126)).sqrt()
S1 = t1 + c1*H1*H1 + L*H1
k1 = to_poly(plaintext1) + c1

c1, t1 = encrypt(2, plaintext1)
c2, t2 = encrypt(2, plaintext2)
t1, t2 = to_poly(t1), to_poly(t2)
c1, c2 = to_poly(c1), to_poly(c2)
H2 = ((t1+t2) / (a^127 + a^126)).sqrt()
S2 = t1 + c1*H2*H2 + L*H2
k2 = to_poly(plaintext1) + c1

plaintext1 = to_poly(plaintext1)
plaintext2 = to_poly(plaintext2)
m = to_bytes((k2*H2*H2 + L*H2 + S2 + k1*H1*H1 + L*H1 + S1)/(H1*H1 + H2*H2))

io.recvuntil(">") 
io.sendline("2") # dual summon
io.sendline(m.hex())
io.recvline()
print(io.recvline())
