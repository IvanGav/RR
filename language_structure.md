Delimiters: `.,[](){}:;"'`
Variable characters: `a-z`, `A-Z`, `_`, `0-9`
Op symbols: `/<>!@#$%^&*-=+|`

Variables and Type Names:
- Start with a letter or `_`
- Can contain numbers after the first character

Functions:
- The ones invoked like this: `fname(param1, param2)` or this `param1.fname(param2, param3)`
- Naming rules are *the same* as for variables

Operators:
- The ones used like this: `lparam opname rparam` or this: `opname rparam`
- ~~If an operator consists of *just* op symbols, they *may* be used without spaces with operands~~
- If an oprtator name contains any *variable characters*, they *must* be separated by spaces with operands
- Two operators *must* be separated by spaces

*How it works*:
- Read a symbol. If not a (fun, op, var) name symbol, split around the first continuous op symbol string. Read as an operator.