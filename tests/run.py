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


INPUT_SEP = "$$INPUT$$\n"
OUTPUT_SEP = "$$OUTPUT$$\n"


def do_test(exec_path: Path, path: Path, file: str) -> bool:
    rest = file.partition(INPUT_SEP)[2]
    input, _, output = rest.partition(OUTPUT_SEP)
    result = run_command(exec_path, input=input)

    relative_path = path.relative_to(Path.cwd())
    if result.stdout != output:
        print(f"{RED}Test failed: {relative_path}{CLEAR}")
        print(f"Expected:\n{YELLOW}{output}{CLEAR}")
        print(f"Received:\n{YELLOW}{result.stdout}{CLEAR}")
        return False
    else:
        print(f"{GREEN}Test succeeded: {relative_path}{CLEAR}")
        return True


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
        print(f"{GREEN}All {len(paths)} tests passed!{CLEAR}")
    else:
        print(f"{RED}Some tests failed {failed}/{len(paths)}{CLEAR}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print()
        print(RED + "Got KeyboardInterrupt, exiting" + CLEAR)
