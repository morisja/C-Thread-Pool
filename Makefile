.PHONY: file-writer

file-writer: file-writer.c
	gcc file-writer.c thpool.c -D THPOOL_DEBUG -pthread -o fw

