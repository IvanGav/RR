# R++ Syntax

A dynamically typed language. Every symbol can consist of `a-z`, `A-Z`, `0-9`, `_`. Symbols don't need to be declared (like Py or R).

Every line is a statement. Every code block is a single statement. Every statement has a return value. Return value of a code block is the return value of the last line. `return <var/val>` can be used to instantly return from a code block with a specified return value.

Special statements:
- `if` statement.
  - Syntax: `if (<condition>) { ... } else if (<condition>) { ... } else { ... }`
  - Return value: return value of the executed branch.
- `while` loop.
  - Syntax: `while (<condition>) { ... }`
  - Return value: return value of the last loop.
- `for` loop.
  - Syntax: `for (<var> in <iter>) { ... }`
  - Return value: return value of the last loop.
- `loop` loop.
  - Syntax: `loop { ... }`
  - Return value: return value of the last loop.
- `switch` statement.
  - TODO

Special statements notes:
- The `{ ... }` represents a statement and `<condition>` represents a statement with `bool` return type
- Loops can be broken out of with `brake` or skipped to the next iteration with `continue`

Variables:
- Always global.

Functions:
- Can be defined with `fn <name> (<parameters>) { ... }`
- Parameters may have a type specified: `<name>: <type>`
- Operators can be defined with `op <name> (<parameters>) { ... }`
  - Operator name may contain **ONE OF**:
    - Letter characters ( and _ )
    - Special characters ( non-letters, non-delimiters: `+-=;:\*/!&^%$#@`, etc. )
  - Only `1` or `2` parameters are allowed, resulting in `prefix` or `infix` operators, respectively

Other special syntax:
- `=` - assignment

Built in Functions/Operators:
- Operators:
  - `+`

Integral Types:
- Int
- Float
- Bool
- Str (is heap allocated)