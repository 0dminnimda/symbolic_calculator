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


variables: list[str] = []
products: list[tuple[str, ...]] = []


def generate_globals():
    global variables, products

    variables = [random_name(NAME_BOUNDS) for _ in range(random.randint(*VAR_BOUNDS))]

    products = list(
        {
            tuple(sorted(random.choices(variables, k=random.randint(*TERMS_BOUNDS))))
            for _ in range(random.randint(*PROD_BOUNDS))
        }
    )
    assert len(set(products)) == len(
        products
    ), "Got duplicate product, try again, good luck!"


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


def generate_coeff(lower, upper):
    # for coefficients we want 0 to be much more likely
    if random.random() < ZERO_PROB:
        return 0
    return random.randint(lower, upper)


def generate_add_sub(is_sub: bool):
    coefficients1 = [generate_coeff(*NUMBER_BOUNDS) for _ in range(len(products))]
    coefficients2 = [generate_coeff(*NUMBER_BOUNDS) for _ in range(len(products))]
    result_coeffs = [0] * len(products)

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
    coefficients1 = [generate_coeff(*NUMBER_BOUNDS) for _ in range(len(products))]
    if equal:
        coefficients2 = coefficients1
    else:
        coefficients2 = [generate_coeff(*NUMBER_BOUNDS) for _ in range(len(products))]

    input_s: list[str] = []
    input_s.append("=")
    input_s.append("\n")

    for coefficients in (coefficients1, coefficients2):
        print_coefficients(input_s, coefficients)

    output_s: list[str] = []
    output_s.append("equal" if equal else "not equal")
    output_s.append("\n")

    return "".join(input_s), "".join(output_s)


def generate_multiplication():
    import sympy

    var_map = dict(zip(variables, sympy.symbols(variables)))

    sym_products = []
    for product in products:
        new_product = sympy.S.One
        for var in product:
            new_product *= var_map[var]
        sym_products.append(new_product)

    coefficients1 = [generate_coeff(*NUMBER_BOUNDS) for _ in range(len(products))]
    coefficients2 = [generate_coeff(*NUMBER_BOUNDS) for _ in range(len(products))]
    poly1 = sum((p * c for c, p in zip(coefficients1, sym_products)), sympy.S.Zero)
    poly2 = sum((p * c for c, p in zip(coefficients2, sym_products)), sympy.S.Zero)

    print("Multiplying")
    result = sympy.expand(poly1 * poly2)

    def expand_to_text(expr):
        if isinstance(expr, sympy.Pow):
            return "*".join([expand_to_text(expr.base)] * int(expr.exp))
        elif isinstance(expr, sympy.Mul):
            return '*'.join(expand_to_text(term) for term in expr.args)
        elif isinstance(expr, sympy.Add):
            return ' + '.join(expand_to_text(term) for term in expr.args)
        else:
            return str(expr)

    print("Dumping strings")
    input_s: list[str] = []
    input_s.append("*")
    input_s.append("\n")

    for coefficients in (coefficients1, coefficients2):
        print_coefficients(input_s, coefficients)

    output_s: list[str] = []
    output_s.append(expand_to_text(result))
    output_s.append("\n")

    return "".join(input_s), "".join(output_s)


WHITESPACE = " "
ZERO_PROB = 0.3
SPACE_BOUNDS = (0, 4)
NAME_BOUNDS = (3, 6)
TERMS_BOUNDS = (20, 50)
VAR_BOUNDS = (50, 70)
PROD_BOUNDS = (50, 70)
NUMBER_BOUNDS = (int(-1e-2), int(1e2))


generate_globals()


# input_s, output_s = generate_add_sub(True)
# input_s, output_s = generate_equality(True)
input_s, output_s = generate_multiplication()

if len(sys.argv) <= 1:
    print(f"USAGE: {sys.argv[0]} <file>")
    exit(1)

Path(sys.argv[1]).with_suffix(".input").write_text(input_s)
Path(sys.argv[1]).with_suffix(".output").write_text(output_s)
