CC := gcc
CFLAGS := -O2 -g
LDFLAGS := -lcrypto

sign-jetson-dtb: sign-jetson-dtb.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@
clean:
	rm -f sign-jetson-dtb
