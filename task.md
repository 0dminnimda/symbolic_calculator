# Задание

Рекомендуется уделить внимание отладке вашего решения:

- продумайте все краевые случаи и подготовьте соответствующие входные данные
- подготовьте ~10 файлов с различными входными данными
  и столько же с эталонной выдачей, напишите скрипт,
  автоматизирующий проверку работы вашей программы
  на этих входных данных
- в некоторых случаях можно написать простой генератор случайных входных данных;
- протестируйте работу вашей программы, собранной в отладочном режиме
  с AddressSanitizer (есть в gcc и clang)
  и MemorySanitizer (есть только в clang)
- проверьте, что время работы программы укладывается в указанные в условиях
  временные рамки на входных данных максимального размера

При проверке вашей программы имеет значение не только формальная
корректность решения задачи, но и качество кода:

- аккуратная расстановка отступов
- грамотно выбранные названия функций и переменных
- грамотно написанные комментарии (не тривиальные,
  но и не избыточно подробные, нужна золотая середина)

Небрежно написанный код может стать причиной отказа в принятии решения.


## Условия задачи

В оптимизирующем компиляторе нередко возникает
необходимость представить вычисления в символьном виде.
Для этого используются PS-формы (PSF - Product-Summ Form),
которые позволяют представить арифметическое выражение в виде
суммы произведений. Пример PS-формы:

```
8 + x - 5*y + 3*x*y
```

где `x` и `y` - некоторые переменные.

В PS-формах используется только три арифметических оператора:
`+`, `-` и `*`

Возведения в степень нет, вместо него используется многократное
умножение, например `x*x*x`.
Скобки не используются.

Мы будем рассматривать только PS-формы в каноническом виде, которые обладают следующими свойствами:

- константные множители в слагаемых могут находиться только
  в начале слагаемого; как следствие, в слагаемом может быть
  только один константный множитель;
- PS-форма не упрощается, т.е. в ней нельзя сократить
  какие-либо слагаемые; например:
  `x + y ` -- канонический вид
  `2*x + y - x` -- НЕ канонический вид (можно упростить до `x + y`)

За исключением указанного выше ограничения, порядок слагаемых
и множителей внутри слагаемого не имеют значения.
Например, следующие PS-формы:

```
x + y*z
y*z + x
z*y + x
```

эквивалентны.


Необходимо реализовать простые операции над парой PS-форм:

- сложение
- вычитание
- умножение
- сравнение на равенство


На вход программе подаются три строки:

1) знак операции, которую необходимо выполнить над двумя PS-формами (`+-*=`)
2) первая PS-форма (в каноническом виде)
3) вторая PS-форма (в каноническом виде)


Все переменные однобуквенные. Константные множители целочисленные,
и могут быть как положительными, так и отрицательными.
Между слагаемыми и множителями может быть произвольное количество пробелов.

Программа должна выполнить указанное действие над парой PS-форм
и затем упростить полученный результат (привести к каноническому виду).
На выходе программа должна напечатать на экран результирующую
PS-форму (в случае `+-*`) либо результат сравнения:
`"equal"` или `"not equal"` (в случае `=`).

### Тестовые данные

Пример входного потока (1):

```
*
x + x*y + y
x - y
```

Вывод программы (1):

```
x*x + x*x*y - x*y*y - y*y
```

Пример входного потока (2):

```
-
a + 3*y - b
-1*x + 3*y - 5*z
```

Вывод программы (2):

```
a - b + x + 5*z
```

Пример входного потока (3):

```
=
-1*x + y*z
z * y - x
```

Вывод программы (3):

```
equal
```

Пример входного потока (4):

```
=
x + y
y - x
```

Вывод программы (4)
```
not equal
```

### Дополнительные требования

1) Программа должна быть написана на языке C.
2) PS-форма должна быть реализована с помощью списка списков:
   сама PS-форма - это список слагаемых, а каждое слагаемое в свою очередь -
   это список множителей.
3) На вход программе будут подаваться не очень большие PS-формы
   (до ~100 слагаемых, до 26 множителей в каждом), поэтому можно использовать
   алгоритмы с квадратичной алгоритмической сложностью.
4) Программа должна работать не дольше одной секунды.
5) Программа должна собираться компилятором gcc и работать
   под операционной системой linux.
6) Выкладывать ваше решение на github - плохая идея, так вы поможете
   другим претендентам, а они сейчас являются вашими конкурентами.