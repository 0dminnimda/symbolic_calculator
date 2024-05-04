import string
import random


WHITESPACE = " \t"


def random_string(
    bounds: tuple[int, int] = (5, 25), alphabet: str = string.printable
) -> str:
    return "".join(random.choice(alphabet) for _ in range(random.randint(*bounds)))


NAME_FILLER = string.digits + string.ascii_letters


def random_name(bounds: tuple[int, int] = (5, 25)) -> str:
    return random_string((1, 1), string.ascii_letters) + random_string(
        (bounds[0] - 1, bounds[1]), NAME_FILLER
    )


SPACE_BOUNDS = (0, 25)
TERMS_BOUNDS = (0, 100)
VAR_BOUNDS = (0, 100)
PROD_BOUNDS = (0, 100)
NUMBER_BOUNDS = (int(-1e-10), int(1e10))


variables = [random_name() for _ in range(random.randint(*VAR_BOUNDS))]

products = list({
    tuple(sorted(random.choices(variables, k=random.randint(*TERMS_BOUNDS))))
    for _ in range(random.randint(*PROD_BOUNDS))
})

def generate_add_sub(is_sub: bool):
    coefficients1 = [random.randint(*NUMBER_BOUNDS) for _ in range(len(products))]
    coefficients2 = [random.randint(*NUMBER_BOUNDS) for _ in range(len(products))]
    result_coeffs = [0]*len(products)

    for i, (a, b) in enumerate(zip(coefficients1, coefficients2)):
        if is_sub:
            result_coeffs[i] = a - b
        else:
            result_coeffs[i] = a + b

    result: list[str] = []
    result.append("$$INPUT$$\n")
    result.append("-" if is_sub else "+")
    result.append("\n")

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

    for coefficients in (coefficients1, coefficients2):
        print_coefficients(result, coefficients)

    result.append("$$OUTPUT$$\n")
    print_coefficients(result, result_coeffs)

    return "".join(result)


print(generate_add_sub(True))
