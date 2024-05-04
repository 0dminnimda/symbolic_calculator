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
    if output.startswith("+"):
        output = output[1:]

    result = []
    for product in output.translate(PREP_TRANS).split(PREP_SEP):
        terms = product.split("*")
        if not terms:
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


def compare_equation(relative_path: Path, expected_str: str, actual_str: str) -> bool:
    if expected_str != actual_str:
        print(f"\n{RED}=== Test failed: {relative_path} ==={CLEAR}")
        print(f"Expected:\n{YELLOW}{expected_str}{CLEAR}")
        print(f"Received:\n{YELLOW}{actual_str}{CLEAR}")
        return False
    else:
        print(f"{GREEN}Test succeeded: {relative_path}{CLEAR}")
        return True


def compare_binary(relative_path: Path, expected_str: str, actual_str: str) -> bool:
    expected = unify(expected_str)
    actual = unify(actual_str)
    if expected != actual:
        print(f"\n{RED}=== Test failed: {relative_path} ==={CLEAR}")
        print(f"Expected:\n{YELLOW}{expected_str}{CLEAR}")
        print(f"Received:\n{YELLOW}{actual_str}{CLEAR}")
        print(f"Hint {expected=} != {actual=}")
        return False
    else:
        print(f"{GREEN}Test succeeded: {relative_path}{CLEAR}")
        return True


INPUT_SEP = "$$INPUT$$\n"
OUTPUT_SEP = "$$OUTPUT$$\n"


def do_test(exec_path: Path, path: Path, file: str) -> bool:
    rest = file.partition(INPUT_SEP)[2]
    input, _, output = rest.partition(OUTPUT_SEP)
    result = run_command(exec_path, input=input)
    relative_path = path.relative_to(Path.cwd())

    if input[0] == "=":
        return compare_equation(relative_path, output, result.stdout)
    else:
        return compare_binary(relative_path, output, result.stdout)



DEFAULT_PATTERNS = [str(DIR / "*.test")]


def main(argv: list[str] = sys.argv) -> None:
    if len(argv) == 1:
        print(f"Usage: {argv[0]} <EXEC> <PATTERN ...>")
        exit(1)

    exec_path = Path(argv[1])
    paths = resolve_patterns(argv[2:] or DEFAULT_PATTERNS)

    failed = 0
    for path in paths:
        if not do_test(exec_path, path, path.read_text()):
            failed += 1

    if failed == 0:
        print(f"\n{GREEN}All {len(paths)} tests passed!{CLEAR}")
    else:
        print(f"\n{RED}Some tests failed {failed}/{len(paths)}{CLEAR}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print()
        print(RED + "Got KeyboardInterrupt, exiting" + CLEAR)
