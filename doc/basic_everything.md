# Basic everything.
Short introduction to Wayward Programming Language.

## Function declaration.
```
func name(const_param_name: param_type, variable_param_name: var param_type) {
    // Body.
}
```

## Function pointer.
```
let func_ptr: func(param1: type1, param2: var type2);
```

## Lambda.
```
let lambda: func(...) {
    ...
};
```

## Variables / constants declaration.
```
let a: int = 10;    // Constant.
var b: int = 20;    // Variable.
```

## Primitive types.
```
int     // 64-bit on 64-bit platform, 32-bit on 32-bit platform.
int8
int16
int32
int64
uint    // 64-bit on 64-bit platform, 32-bit on 32-bit platform.
uint8
uint16
uint32
uint64
float32
float64

