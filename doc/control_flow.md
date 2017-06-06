# Control flow

## If expression

```
var max = a;
if a < b {
    max = b;
}

var max: int;
if a > b {
    max = a;
} else {
    max = b;
}
```

## For loops

```
for var i = 0; i < 10; ++i {
    do_things();
}
```

## While loops

```
var x = 10;
while x > 0 {
    x--
}

do {
    let y = get_data();
} while y != null;
```

## Break and continue in loops

Tez supports traditional `break` and `continue` statements.

