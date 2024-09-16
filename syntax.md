# R++ Syntax

A dynamically typed language. Every symbol can consist of `a-z`, `A-Z`, `0-9`, `_` and `-`. Symbols don't need to be declared (like Py or R).

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

Notes:
- Loops can be broken out of with `brake` or skipped to the next iteration with `continue`

Functions:
- Can be defined with

- `=` - assignment
