CC = gcc
CFLAGS = -Wall -Wextra -std=c11

INPUT_FILES = in01.txt in02.txt in03.txt in04.txt in05.txt
OUTPUT_FILES = out01.txt out02.txt out03.txt out04.txt out05.txt

.PHONY: all clean

all: mlfq

mlfq: mlfq.c
	$(CC) $(CFLAGS) -o mlfq mlfq.c

test: mlfq $(INPUT_FILES) $(OUTPUT_FILES)
	@for i in $(INPUT_FILES); do \
		./mlfq < $$i > output.txt; \
		diff -u output.txt $${i/in/out}; \
	done

clean:
	rm -f mlfq output.txt