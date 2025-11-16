CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

UI_SRC := src/ui/pc_room.c src/ui/login.c src/ui/charge.c src/ui/Guest.c src/ui/newUser.c
CORE_SRC := src/core/auth.c
STORAGE_SRC := src/storage/storage.c
CONFIG_SRC := src/config/config.c

SRCS := $(UI_SRC) $(CORE_SRC) $(STORAGE_SRC) $(CONFIG_SRC)
OBJS := $(SRCS:.c=.o)

TARGET := pc_room

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

src/ui/%.o: src/ui/%.c
	$(CC) $(CFLAGS) -c $< -o $@

src/core/%.o: src/core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

src/storage/%.o: src/storage/%.c
	$(CC) $(CFLAGS) -c $< -o $@

src/config/%.o: src/config/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)


