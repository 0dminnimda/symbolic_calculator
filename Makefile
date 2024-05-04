CC = "$(shell (which clang 2>/dev/null || which gcc 2>/dev/null) | head -1)"
PYTHON = "$(shell (which python3 2>/dev/null || which python 2>/dev/null) | head -1)"

ifdef DEBUG
	FLAGS = -gdwarf-4  # -fsanitize=address
else
	FLAGS =
endif

COMPATS = src/compatibility/read_line.c src/compatibility/string.c
SRCS = src/main.c src/sum_of_product.c
DATA = 

.PHONY: com
com:
	$(CC) -o src/main.out $(SRCS) $(COMPATS) $(FLAGS)


.PHONY: valgrind
valgrind:
	printf "$(DATA)" | valgrind -s --leak-check=full ./src/main.out


.PHONY: test
test:
	$(PYTHON) tests/run.py src/main.out


.PHONY: clean
clean:
	rm main.out
