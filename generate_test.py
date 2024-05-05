import sys
import string
import random
from pathlib import Path


def random_string(
    bounds: tuple[int, int] = (5, 25), alphabet: str = string.printable
) -> str:
    return "".join(random.choice(alphabet) for _ in range(random.randint(*bounds)))


NAME_FILLER = string.digits + string.ascii_letters


def random_name(bounds: tuple[int, int] = (5, 25)) -> str:
    return random_string((1, 1), string.ascii_letters) + random_string(
        (bounds[0] - 1, bounds[1] - 1), NAME_FILLER
    )


SPACE_BOUNDS = (0, 25)
NAME_BOUNDS = (5, 25)
TERMS_BOUNDS = (0, 100)
VAR_BOUNDS = (50, 100)
PROD_BOUNDS = (50, 100)
NUMBER_BOUNDS = (int(-1e-10), int(1e10))


variables = [random_name(NAME_BOUNDS) for _ in range(random.randint(*VAR_BOUNDS))]

products = list({
    tuple(sorted(random.choices(variables, k=random.randint(*TERMS_BOUNDS))))
    for _ in range(random.randint(*PROD_BOUNDS))
})


def print_coefficients(result: list[str], coefficients: list[int]):
    for c, terms in zip(coefficients, products):
        result.append(random_string(SPACE_BOUNDS, WHITESPACE))
        if c == 0:
            continue
        result.append(f"{c:+}")
        shuffled_terms = list(terms)
        random.shuffle(shuffled_terms)
        for term in shuffled_terms:
            result.append(random_string(SPACE_BOUNDS, WHITESPACE))
            result.append("*")
            result.append(random_string(SPACE_BOUNDS, WHITESPACE))
            result.append(term)
    result.append("\n")


def generate_add_sub(is_sub: bool):
    coefficients1 = [random.randint(*NUMBER_BOUNDS) for _ in range(len(products))]
    coefficients2 = [random.randint(*NUMBER_BOUNDS) for _ in range(len(products))]
    result_coeffs = [0]*len(products)

    for i, (a, b) in enumerate(zip(coefficients1, coefficients2)):
        if is_sub:
            result_coeffs[i] = a - b
        else:
            result_coeffs[i] = a + b

    input_s: list[str] = []
    input_s.append("-" if is_sub else "+")
    input_s.append("\n")

    for coefficients in (coefficients1, coefficients2):
        print_coefficients(input_s, coefficients)

    output_s: list[str] = []
    print_coefficients(output_s, result_coeffs)

    return "".join(input_s), "".join(output_s)


def generate_equality(equal: bool):
    coefficients1 = [random.randint(*NUMBER_BOUNDS) for _ in range(len(products))]
    if equal:
        coefficients2 = coefficients1
    else:
        coefficients2 = [random.randint(*NUMBER_BOUNDS) for _ in range(len(products))]

    input_s: list[str] = []
    input_s.append("=")
    input_s.append("\n")

    for coefficients in (coefficients1, coefficients2):
        print_coefficients(input_s, coefficients)

    output_s: list[str] = []
    output_s.append("equal" if equal else "not equal")
    output_s.append("\n")

    return "".join(input_s), "".join(output_s)


WHITESPACE = " "

# input_s, output_s = generate_add_sub(True)
input_s, output_s = generate_equality(True)

if len(sys.argv) <= 1:
    print(f"USAGE: {sys.argv[0]} <file>")
    exit(1)

Path(sys.argv[1]).with_suffix(".input").write_text(input_s)
Path(sys.argv[1]).with_suffix(".output").write_text(output_s)
