CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Werror
TARGET := np355v4c-fanctl

$(TARGET): np355v4c-fanctl.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: clean
