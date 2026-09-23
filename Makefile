CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

SRC_DIR = src
TEST_DIR = tests

SRCS = $(SRC_DIR)/protocol.c
TEST_SRCS = $(TEST_DIR)/test_protocol.c

# Windows icin target uzantisi .exe
TARGET = test_runner.exe

all: test

# Test executable'ini derleme
$(TARGET): $(SRCS) $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(TEST_SRCS)

# Testleri calistirma
test: $(TARGET)
	./$(TARGET)

# Derlenmis dosyalari temizleme
clean:
	del /Q $(TARGET) 2>nul || rm -f $(TARGET)
