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


.PHONY: show_test
show_test:
	- printf "*\nx + x*y + y\nx - y"              | ./src/main.out
	#
	- printf " - \na + 3*y - b\n-1*x + 3*y - 5*z" | ./src/main.out
	#
	- printf "=\n-1*x + y*z\nz * y - x"           | ./src/main.out
	#
	- printf "=\n-1*x + y*z\nz * y - x" 	      | ./src/main.out


.PHONY: clean
clean:
	rm main.out
