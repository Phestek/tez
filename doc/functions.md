# Functions

Every Tez program have at least one function, the `main` function:
```
func main() -> int {
    return 0;
}
```

## Function declarations

Functions are declared using `func` keyword:
```
func example(x: int) -> int {
    return x + 2;
}
```
If `-> int` is omitted, function always returns void (nothing):
```
func example(x: int) {
}
// The same as:
func example(x: int) -> void {
}
```

## Function arguments.

Arguments are const by default. To allow their modifications, declare them with
`var` keyword:
```
func example(x: var int) {
}
```

## Function usage

```
val result = example(12);
```

