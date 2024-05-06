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

ifdef PROF
	FLAGS += -fprofile-instr-generate -fcoverage-mapping -g
endif


# -fsanitize=memory
# -fsanitize=address

COMPATS = src/compatibility/read_line.c src/compatibility/string.c
SRCS = src/main.c src/sum_of_product.c src/sized_string.c
DATA = 
PROF_DIR = profile

.PHONY: com
com:
	$(CC) -o src/main.out -pedantic -Wall -Wextra -O$(O) $(SRCS) $(COMPATS) $(FLAGS)

.PHONY: prof
prof:
	LLVM_PROFILE_FILE="$(PROF_DIR)/run_tests.profraw" src/main.out
	llvm-profdata merge -sparse $(PROF_DIR)/run_tests.profraw -o $(PROF_DIR)/merged.profdata
	llvm-profdata show $(PROF_DIR)/merged.profdata --all-functions --counts --memop-sizes -ic-targets > $(PROF_DIR)/report.txt
	llvm-cov report ./src/main.out -instr-profile=$(PROF_DIR)/merged.profdata
	llvm-cov show ./src/main.out -instr-profile=$(PROF_DIR)/merged.profdata > $(PROF_DIR)/report_code.txt


.PHONY: valgrind
valgrind:
	printf "$(DATA)" | valgrind -s --leak-check=full ./src/main.out


.PHONY: test_small
test_small:
	$(PYTHON) tests/run.py src/main.out tests/**/*.input !tests/**/big*.input


.PHONY: test
test:
	$(PYTHON) tests/run.py src/main.out


.PHONY: clean
clean:
	rm main.out
