CC      ?= clang
CFLAGS  ?= -O2 -Iinclude
O       ?= out
TARGET  := warnet
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:src/%.c=$(O)/%.o)

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(O)/%.o: src/%.c $(wildcard include/*.h) | $(O)
	$(CC) $(CFLAGS) -c $< -o $@

$(O):
	mkdir -p $@ 

clean:
	rm -rf $(O) $(TARGET) $(TARGET).exe sesi.dat

.PHONY: all clean
