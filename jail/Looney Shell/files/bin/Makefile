CC  := gcc

HELLO_SRC := hello.c
HELLO_BIN := $(HELLO_SRC:%.c=%)
FLAG_SRC  := flag.c
FLAG_BIN  := $(FLAG_SRC:%.c=%)

all: $(HELLO_BIN) $(FLAG_BIN)

$(HELLO_BIN): $(HELLO_SRC)
	$(CC) $^ -z execstack -m32 -o $@ 

$(FLAG_BIN): $(FLAG_SRC)
	$(CC) $^ -DFLAG=$(FLAG) -o $@ 

clean:
	$(RM) $(HELLO_BIN) $(FLAG_BIN)

.PHONY: all clean

