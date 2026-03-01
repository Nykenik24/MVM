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
| CGE  | Checks if `reg0` >= `reg1` and stores the result at `CND`             | `reg0`, `reg1`         |
| CLE  | Checks if `reg0` <= `reg1` and stores the result at `CND`             | `reg0`, `reg1`         |
| CEQ  | Checks if `reg0` == `reg1` and stores the result at `CND`             | `reg0`, `reg1`         |
| JMP  | Jumps to `label`                                                      | `label`                |
| JT   | Jumps to `label` **IF** `CND` is true                                 | `label`                |
| JF   | Jumps to `label` **IF** `CND` is false                                | `label`                |
| JZ   | Jumps to `label` **IF** `FLAG` is `Z` (Zero)                          | `label`                |
| JNZ  | Jumps to `label` **IF** `FLAG` is **NOT** `Z` (Zero)                  | `label`                |
| JN   | Jumps to `label` **IF** `FLAG` is `N` (Negative)                      | `label`                |
| JNN  | Jumps to `label` **IF** `FLAG` is **NOT** `N` (Negative)              | `label`                |
| PUTN | Output number at `reg`                                                | `reg`                  |
| PUTS | Output string of len `strlen`. Will read `strlen` characters in code. | `strlen`, `char1`, ... |
| LBL  | Define a new label. `label_name` is an identifier.                    | `label_name`           |

## Notes:

- `PUTS` expects you to write each singular character with the format `'<char>'` after inputting the length.
- In `LBL` and `JMP` (and all of it's derivatives) labels must be lowercase.
