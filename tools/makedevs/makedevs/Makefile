.PHONY: all

TARGET = makedevs
GCC = gcc
CFLAGS = -Wall -Werror -O2

all: $(TARGET)

clean:
	@rm -rf $(TARGET)

.c.o:
	$(GCC) $(CFLAGS) -c $< -o $@


