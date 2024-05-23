CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -fPIC -shared

SRC_DIR = src
OUT_DIR = out

libtmalloc.so:
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC_DIR)/tmalloc/*.c -o $(OUT_DIR)/$@

main.out: libtmalloc.so
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(OUT_DIR)/$@ -Lout -ltmalloc -Wl,-rpath='$$ORIGIN'

all: libtmalloc.so main.out

run: main.out
	./out/$^

clean:
	rm -rf $(OUT_DIR)

.PHONY: all run clean
