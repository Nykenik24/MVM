# Operations

| Name | Description                                                           | Params                 |
| ---- | --------------------------------------------------------------------- | ---------------------- |
| HALT | Stops execution                                                       | _None_                 |
| LD   | Loads a value from memory into a register                             | `reg`, `offset`        |
| LDI  | Loads an immediate value into a register                              | `reg`, `imm`           |
| ST   | Stores a register into memory                                         | `reg`                  |
| ADD  | Adds `reg0` and `reg1` and stores the value at `reg2`                 | `reg0`, `reg1`, `reg2` |
| ADDI | Adds `reg0` and `imm` and stores the value at `reg1`                  | `reg0`, `reg1`         |
| SUB  | Subtracts `reg1` from `reg0` and stores the value at `reg2`           | `reg0`, `reg1`, `reg2` |
| SUBI | Subtracts `imm` from `reg0` and stores the value at `reg1`            | `reg0`, `imm`, `reg1`  |
| MUL  | Multiplies `reg0` and `reg1` and stores the value at `reg2`           | `reg0`, `reg1`, `reg2` |
| MULI | Multiplies `reg0` and `imm` and stores the value at `reg1`            | `reg0`, `imm`, `reg1`  |
| DIV  | Divides `reg0` by `reg1` and stores the value at `reg2`               | `reg0`, `reg1`, `reg2` |
| DIVI | Divides `reg0` by `imm` and stores the value at `reg1`                | `reg0`, `imm`, `reg1`  |
| CGR  | Checks if `reg0` > `reg1` and stores the result at `CND`              | `reg0`, `reg1`         |
| CLO  | Checks if `reg0` < `reg1` and stores the result at `CND`              | `reg0`, `reg1`         |
| CEQ  | Checks if `reg0` == `reg1` and stores the result at `CND`             | `reg0`, `reg1`         |
| JMP  | Jumps to `dest`                                                       | `dest`                 |
| JT   | Jumps to `dest` **IF** `CND` is true                                  | `dest`                 |
| JF   | Jumps to `dest` **IF** `CND` is false                                 | `dest`                 |
| JZ   | Jumps to `dest` **IF** `FLAG` is `Z` (Zero)                           | `dest`                 |
| JNZ  | Jumps to `dest` **IF** `FLAG` is **NOT** `Z` (Zero)                   | `dest`                 |
| JN   | Jumps to `dest` **IF** `FLAG` is `N` (Negative)                       | `dest`                 |
| JNN  | Jumps to `dest` **IF** `FLAG` is **NOT** `N` (Negative)               | `dest`                 |
| PUTN | Output number at `reg`                                                | `reg`                  |
| PUTS | Output string of len `strlen`. Will read `strlen` characters in code. | `strlen`, `...`        |
| JPT  | Define a new jump point                                               | _None_                 |

<!--
## LD

LD will load a value from memory (`mem[USEMEM_START + offset]`) to a register.

## LDI

LDI will load an immediate value (second operand of the operation) to a register.

### ADD

ADD will add the values of `reg0` (first operand) and `reg1` (second operand) and store the result at the destination register (`reg2`, last operand).

For example, if we run:

```s
ADD 0, 1, 2
```

And

- `R0` is 5
- `R1` is 6

Then `R2` will, at the end of the operation, contain the value `11`.

### ADDI

ADDI will add the values of `reg0` (first operand) and an immediate value (`imm`, second operand) and store the result at the destination register (`reg1`, last operand)

For example, if we run:

```s
ADDI 0, 8, 2
```

And

- `R0` is 4

Then `R2` will, at the end of the operation, contain the value `12`.
-->
