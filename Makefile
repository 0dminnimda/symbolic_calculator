CC = "$(shell (which clang 2>/dev/null || which gcc 2>/dev/null) | head -1)"

ifdef DEBUG
	FLAGS = -g -fsanitize=address
else
	FLAGS =
endif

COMPATS = src/compatibility/read_line.c src/compatibility/string.c
SRCS = src/main.c src/sum_of_product.c


.PHONY: com
com:
	$(CC) -o src/main.out $(SRCS) $(COMPATS) $(FLAGS)


.PHONY: clean
clean:
	rm main.out
