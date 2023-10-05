CC = gcc
CFLAGS = -Wall -Wextra

TARGET = myshell

$(TARGET): myshell.c
	$(CC) $(CFLAGS) -o $(TARGET) myshell.c

.PHONY: clean

clean:
	rm -f $(TARGET)