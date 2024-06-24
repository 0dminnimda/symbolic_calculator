# mcst_enrollment

## Build

```console
make
```

## Run tests

```console
make tests
```

or also

```console
make tests_small
```

## Example

1.
    ```
    *
    x + x*y + y
    x - y
    ```

    ->

    ```
    x*x + x*x*y - x*y*y - y*y
    ```

2.
    ```
    -
    a + 3*y - b
    -1*x + 3*y - 5*z
    ```

    ->

    ```
    a - b + x + 5*z
    ```

3.
    ```
    =
    -1*x + y*z
    z * y - x
    ```

    ->

    ```
    equal
    ```

4.
    ```
    =
    x + y
    y - x
    ```

    ->

    ```
    not equal
    ```

