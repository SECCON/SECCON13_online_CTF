#!/bin/sh

make FLAG='\"flag{***REDACTED***}\"' -C bin
docker compose build
