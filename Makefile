CC = "$(shell (which clang 2>/dev/null || which gcc 2>/dev/null) | head -1)"
PYTHON = "$(shell (which python3 2>/dev/null || which python 2>/dev/null) | head -1)"
O = 3

ifeq ($(DEBUG),2)
    FLAGS = -gdwarf-4 -DDEBUG_MAIN -DDEBUG_PARSER -DDEBUG_SOP
else ifeq ($(DEBUG),1)
    FLAGS = -gdwarf-4
else
    FLAGS =
endif

# -fsanitize=memory
# -fsanitize=address

COMPATS = src/compatibility/read_line.c src/compatibility/string.c
SRCS = src/main.c src/sum_of_product.c src/sized_string.c
DATA = 

.PHONY: com
com:
	$(CC) -o src/main.out -pedantic -Wall -Wextra -O$(O) $(SRCS) $(COMPATS) $(FLAGS)


.PHONY: valgrind
valgrind:
	printf "$(DATA)" | valgrind -s --leak-check=full ./src/main.out


.PHONY: test_small
test_small:
	$(PYTHON) tests/run.py src/main.out tests/**/*.input !tests/**/big*_char.input


.PHONY: test
test:
	$(PYTHON) tests/run.py src/main.out


.PHONY: clean
clean:
	rm main.out
