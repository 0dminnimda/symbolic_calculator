import re
import sys
import subprocess
from pathlib import Path
from glob import iglob

DIR = Path(__file__).parent

RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
CLEAR = "\033[0m"


def run_command(*args, input=None) -> subprocess.CompletedProcess:
    return subprocess.run(
        [str(arg) for arg in args],
        input=input,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )


def resolve_patterns(patterns: list[str]) -> list[Path]:
    include = set()
    ignore = set()

    for pattern in patterns:
        is_negative = pattern.startswith("!")
        if is_negative:
            pattern = pattern[1:]
        pattern = pattern.strip(" ")

        for match in iglob(pattern, recursive=True):
            path = Path(match).resolve()
            if is_negative:
                ignore.add(path)
            else:
                include.add(path)

    return sorted(include - ignore)


PREP_SEP = "|"
PREP_TABLE = {
    "+": PREP_SEP,
    "-": PREP_SEP + "-",
}
PREP_TRANS = str.maketrans(PREP_TABLE)


def unify(output: str) -> list[tuple[int, list[str]]]:
    output = re.sub(r"\s+", "", output)
    output = output.translate(PREP_TRANS)
    if output.startswith("|"):
        # any +/- will make |+/-, so the first | is meaningless
        output = output[1:]

    result = []
    for product in output.split(PREP_SEP):
        terms = product.split("*")
        if not terms:
            result.append((1, terms))
            continue  # empty product - erroneus form

        coefficient = 1
        if terms[0].startswith("-"):
            terms[0] = terms[0][1:]
            coefficient = -1
        elif terms[0].startswith("+"):
            terms[0] = terms[0][1:]
            pass # coefficient = 1

        try:
            coefficient *= int(terms[0])
            terms = terms[1:]
        except ValueError:
            pass

        result.append((coefficient, sorted(terms)))

    return sorted(result)


STR_LIM = 1000


def limit_str(s: str, limit: int) -> str:
    if len(s) >= limit:
        return s[:limit] + "..."
    return s


def compare_equation(relative_path: Path, expected_str: str, actual_str: str) -> bool:
    if expected_str != actual_str:
        first_line = f"=== Test failed: {relative_path} ==="
        print(RED + first_line + CLEAR)
        print(f"Expected:\n{YELLOW}{limit_str(expected_str, STR_LIM)}{CLEAR}")
        print(f"Received:\n{YELLOW}{limit_str(actual_str, STR_LIM)}{CLEAR}")
        print(RED + "="*len(first_line) + CLEAR + "\n")
        return False
    else:
        print(f"{GREEN}Test succeeded: {relative_path}{CLEAR}")
        return True


def compare_binary(relative_path: Path, expected_str: str, actual_str: str) -> bool:
    expected = unify(expected_str)
    actual = unify(actual_str)
    if expected != actual:
        first_line = f"=== Test failed: {relative_path} ==="
        print(RED + first_line + CLEAR)
        print(f"Expected:\n{YELLOW}{limit_str(expected_str, STR_LIM)}{CLEAR}")
        print(f"Received:\n{YELLOW}{limit_str(actual_str, STR_LIM)}{CLEAR}")
        expected = limit_str(str(expected), STR_LIM)
        actual = limit_str(str(actual), STR_LIM)
        print(f"Hint: {expected=} != {actual=}")
        print(RED + "="*len(first_line) + CLEAR + "\n")
        return False
    else:
        print(f"{GREEN}Test succeeded: {relative_path}{CLEAR}")
        return True


INPUT_SEP = "$$INPUT$$\n"
OUTPUT_SEP = "$$OUTPUT$$\n"


def do_test(exec_path: Path, path: Path, file: str, prepend_args: list[str]) -> bool:
    rest = file.partition(INPUT_SEP)[2]
    input, _, output = rest.partition(OUTPUT_SEP)
    result = run_command(*prepend_args, exec_path, input=input)
    relative_path = path.relative_to(Path.cwd())

    if input[0] == "=":
        return compare_equation(relative_path, output, result.stdout)
    else:
        return compare_binary(relative_path, output, result.stdout)



DEFAULT_PATTERNS = [str(DIR / "**/*.test")]

VALGRIND = False


def main(argv: list[str] = sys.argv) -> int:
    if len(argv) == 1:
        print(f"Usage: {argv[0]} <EXEC> <PATTERN ...>")
        return 1

    prepend_args = []
    if VALGRIND:
        prepend_args = ["valgrind", "--leak-check=full", "--quiet"]

    exec_path = Path(argv[1])
    paths = resolve_patterns(argv[2:] or DEFAULT_PATTERNS)

    failed = 0
    for path in paths:
        if not do_test(exec_path, path, path.read_text(), prepend_args):
            failed += 1

    if failed == 0:
        print(f"\n{GREEN}All {len(paths)} tests passed!{CLEAR}")
        return 0
    else:
        print(f"\n{RED}Some tests failed {failed}/{len(paths)}{CLEAR}")
        return 1


if __name__ == "__main__":
    try:
        exit(main())
    except KeyboardInterrupt:
        print()
        print(RED + "Got KeyboardInterrupt, exiting" + CLEAR)
        exit(1)
