#!/bin/sh

LENGTH=16
STRENGTH=26
challenge=`dd bs=32 count=1 if=/dev/urandom 2>/dev/null | base64 | tr +/ _. | cut -c -$LENGTH`
echo hashcash -mb$STRENGTH $challenge

echo -n "hashcash token: "
read token
if [ `expr "$token" : "^[a-zA-Z0-9\_\+\.\:\/]\{52\}$"` -eq 52 ]; then
    hashcash -cdb$STRENGTH -f /tmp/hashcash.sdb -r $challenge $token 2> /dev/null
    if [ $? -eq 0 ]; then
        echo "[+] Correct"
    else
        echo "[-] Wrong"
        exit
    fi
else
    echo "[-] Invalid token"
    exit
fi

exec /home/pwn/run.sh
