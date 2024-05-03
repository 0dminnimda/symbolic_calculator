CC = "$(shell (which clang 2>/dev/null || which gcc 2>/dev/null) | head -1)"

ifdef DEBUG
	FLAGS = -g -fsanitize=address
else
	FLAGS =
endif


.PHONY: com
com:
	$(CC) -o src/main.out src/main.c src/compatibility/read_line.c $(FLAGS)


.PHONY: clean
clean:
	rm main.out
