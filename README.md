# RR
RR programming language interpreter

- R Recreated
- R Remodeled
- R Redesigned
- R Renovated
- R Refactored
- R Revanced
- R Rebalanced
- R but betterR
- etc

# What is it?
I hate R. I also want to make a programming language. Well, here is my first (scuffed and slow) programming language - RR.

It's designed to be used for math, discrete math, statistics, data science, etc. Of course it's never going to be good enough for any practical uses, but I think it's a cool language (it's going to be, trust!).

Now that it's barely operational, I decided to make it a little more official.

```rust
greetings = "Welcome to "
print(greetings + ("R" repeat 2))
```
```
Str: Welcome to RR
```

# Syntax
## Refer to 'syntax.md'
At this point, this interpreter is very incomplete, so a lot of required features are not here.

It's basically a fancy calculator in this state.

## Data types
- `Int` - 64bit signed integer
- `Float` - 64bit floating point number
- `Bool` - true or false
- `Str` - a string of arbitrary length
- That's it... *for now*

## Dynamically typed
It felt silly to prohibit dynamic typing, when it's literally built into the interpreter design. **I MIGHT CONSIDER MAKING IT STATICALLY TYPED FOR OPTIMIZATION**. I might also consider making it into a compiled language, though it's quite a big desicion too.

But for now it's not statically typed. Though it's planned to let users specify the parameter data types when defining your own function (or just say `Any` and it'll accept whatever)... Whenever defining your own functions is implemented.

## Cool features (?)
Every statement returns a value. Typing `2+2` on a line will have a return value of `4`. Assignment returns the assigned value (`var = 10` will return `10`).

Block statements will return the last line:
```rust
var = {
    10
    2
    "i like cats"
    13
}
var
```
```
Int: 13
```

Ifs are a single statement, meaning you can use them like this:
```rust
var = if (10 == 10) "I know how to compare integers!" else "I don't know how to compare integers =("
var
```
```
Str: I know how to compare integers!
```

You get the point.

Next: Operators. One of the defining features of RR. Everything can be an operator as long as you want it to be. Or a function call. Works both ways! (. notation is planned to be supported)

```rust
2 + 3
+(2, 3)
2.+(3)
```

They are all legal and will be `5`.

Later on, whenever custom functions in RR are supported, this will be the syntax to create an operator vs. a function:

```rust
fn cool_function(a: Int, b: Int) {
    a + b
}
op cool_op(a: Int, b: Int) {
    a + b
}

cool_function(2, 3) //legal
2 cool_function 3   //illegal
cool_op(2, 3)       //legal
2 cool_op 3         //legal
```

In actual interpreter, the only difference between `op`s and `fn`s, is that `op` names have a "operator priority number" attached to them, letting them be destributed as needed (order of operations!).

To make a list, just use the "json" notation for lists:

```rust
list = [1,2,"i like cats",true,1]
```

Lists can hold any amount of arbitrary data. No indexing into lists yet, though =(

## Questionable design choices
- Last line of source code is its "output" => what gets printed (or use `print` function).
- To define a variable, just assign a name to a symbol. No need to declare. As a result, no scopes (besides function calls, in the future). Everything gets stored into the main scope.
- A language is modeled after R. ***R***. Even if I'm trying to fix all the wrongdoings of R, it's still going to be bad. Besides it's just going to be a generic uninteresting language (every programmer made a few).
- Many others that I will change too often to talk about them here =P

# I love it, how do I use it? (<-me btw, probably not you)

- `$ git clone <url>`
- `$ cd RR`
- `$ make`
- `$ ./a.out < <rr_source_file>`
  - or just type a single line of input into stdin
- enjoy the output
- if you want to look at a cool wall of text, use **ANY AMOUNT OF ARBITRARY** arguments to `a.out`
  - Example: `$ ./a.out R should not exist R should not exist R should not exist < examples/block_statement.rr`
