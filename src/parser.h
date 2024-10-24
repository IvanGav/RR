// Given a list of tokens, convert it into an Abstract Syntax Tree
// For format, refer to ../syntax.md

#pragma once

#include <string>
#include <vector>

#include "datatypes.h"
#include "rr_obj.h"
#include "tokenizer.h"
#include "environment.h"
#include "rr_error.h"

using namespace std;

/*
    Definitions
*/

enum ASTType {
    STATEMENT, // a singular statement, consisting of one or more nodes; return last child's return value
    LITERAL, // a node that directly contains the value returned by `eval`
    VAR,
    FUN,
    OP,
    IF,
    LOOP,
    FUN_DECL,
    RETURN,
    CSV, //comma separated values; acts similar to statement, but returns vector<RRObj> when evaluated, containing all childrens' return values
    EVALUATE, // evaluating a function means `fun(params...)`
    INDEX, // indexing into a collection means `arr[index]`
    LIST_BUILDER // list builder is invoked by `[comma, separated, elements]`
};

struct ASTNode;

/*
    Structs
*/
struct ASTNode {
    ASTType type;
    vector<ASTNode*> children;
    union {
        RRObj literal;
        string symbol;
    };

    ASTNode(ASTType type) {
        this->type = type;
        this->children = {};
    }
    ASTNode(ASTType type, RRObj rr_obj) {
        this->type = type;
        this->children = {};
        this->literal = rr_obj;
    }
    ASTNode(ASTType type, vector<ASTNode*> children) {
        this->type = type;
        this->children = children;
    }
    ASTNode(ASTType type, vector<ASTNode*> children, string& symbol_name) {
        this->type = type;
        this->children = children;
        new (&this->symbol) string(symbol_name);
    }
    ASTNode(ASTType type, string& symbol_name) {
        this->type = type;
        new (&this->symbol) string(symbol_name);
    }
    ~ASTNode() {
        switch(type) {
            case ASTType::LITERAL: literal.~RRObj(); break;
            case ASTType::FUN: symbol.~string(); break;
            case ASTType::OP: symbol.~string(); break;
            case ASTType::VAR: symbol.~string(); break;
            default: break; //STATEMENT
        }
    }
    ASTNode& operator=(const ASTNode& val) {
        // Nothing to do in case of self-assignment
        if (&val != this) {
            switch(type) {
                case ASTType::LITERAL: literal.~RRObj(); break;
                case ASTType::FUN: symbol.~string(); break;
                case ASTType::VAR: symbol.~string(); break;
                default: break; //STATEMENT
            }
            type = val.type;
            children = val.children;
            switch(val.type) {
                case ASTType::LITERAL:
                    literal = val.literal; break;
                case ASTType::FUN:
                case ASTType::VAR:
                    new (&this->symbol) string(val.symbol);
                    break;
                default: 
                    break; //STATEMENT
            }
        }

        return *this;
    }

    RRObj eval(Env& env) {
        switch (type) {
            case ASTType::STATEMENT: {
                for(int i = 0; i < children.size()-1; i++) {
                    children[i]->eval(env);
                }
                return children.back()->eval(env);
            }; break;
            case ASTType::LITERAL: {
                return literal;
            }; break;
            case ASTType::VAR: {
                return env.get_var(symbol);
            }; break;
            case ASTType::FUN: {
                vector<RRObj> args;
                vector<RRDataType> types;
                for(int i = 0; i < children.size(); i++) {
                    args.push_back(children[i]->eval(env));
                    types.push_back(args[i].type);
                }
                RRFun* fun = env.get_fun(symbol, types);
                return RRObj(fun);
            }; break;
            case ASTType::OP: {
                if(children.size() == 0) {
                    //it's a fun-like op call
                    // `=` is illegal for now
                    vector<RRObj> args;
                    vector<RRDataType> types;
                    for(int i = 0; i < children.size(); i++) {
                        args.push_back(children[i]->eval(env));
                        types.push_back(args[i].type);
                    }
                    RRFun* fun = env.get_fun(symbol, types);
                    return RRObj(fun);
                } else {
                    //it's a regular op
                    if(symbol == "=") {
                        // return env.assign_var(children[0]->symbol, children[1]->eval(env));
                        RRObj& obj = children[0]->eval_mut(env);
                        obj = children[1]->eval(env);
                        return obj;
                    } else {
                        vector<RRObj> args;
                        vector<RRDataType> types;
                        for(int i = 0; i < children.size(); i++) {
                            args.push_back(children[i]->eval(env));
                            types.push_back(args[i].type);
                        }
                        RRFun* fun = env.get_fun(symbol, types);
                        return fun->cpp_fun(args);
                    }
                }
            }; break;
            case ASTType::IF: {
                //TODO: THIS ONLY WORKS FOR 1 IF 1 ELSE, REDO LATER
                if(children[0]->eval(env).data_bool) {
                    return children[1]->eval(env);
                } else {
                    return children[2]->eval(env);
                }
            }; break;
            case ASTType::CSV: {
                //return a list RRObj
                RRObj list_obj = RRObj(new vector<RRObj>());
                for(int i = 0; i < children.size(); i++) {
                    list_obj.data_list->push_back(children[i]->eval(env));
                }
                return list_obj;
            }; break;
            case ASTType::LIST_BUILDER: {
                if(children.size() != 1) rr_runtime_error("List Builder doesn't have exactly 1 child");
                if(children[0]->type != ASTType::CSV) rr_runtime_error("List Builder doesn't have a CSV child");
                return children[0]->eval(env);
            }; break;
            case ASTType::EVALUATE: {
                //evaluate a function call
                if(children.size() != 2) rr_runtime_error("Evaluate node doesn't have exactly 2 children");
                RRObj fn_ptr = children[0]->eval(env);
                if(!(fn_ptr.type == RRDataType("Fn"))) rr_runtime_error("Trying to evaluate a non-function");
                //assume that second child is a CSV node
                RRObj params = children[1]->eval(env);
                fn_ptr.data_fn->cpp_fun(*params.data_list);
            }; break;
        }
        rr_runtime_error(string("Invalid statement encountered: ")+to_string(type));
        exit(1);
    }

    RRObj& eval_mut(Env& env) {
        switch (type) {
            case ASTType::STATEMENT: {
                for(int i = 0; i < children.size()-1; i++) {
                    children[i]->eval(env);
                }
                return children[children.size()-1]->eval_mut(env);
            }; break;
            case ASTType::VAR: {
                return env.get_var_or_new_mut(symbol); //allow the variables not to be previously created
            }; break;
            default: rr_runtime_error("Cannot mutably reference a non-variable");
        }
        rr_runtime_error(string("Invalid statement encountered (mutably): ")+to_string(type));
        exit(1);
    }

    friend std::ostream& operator<<(std::ostream& os, const ASTNode& node) {
        switch(node.type) {
            case ASTType::LITERAL: {
                os << "ASTNode<Literal>(" << node.literal << ") with " << node.children.size() << " children:" << endl;
            }; break;
            case ASTType::FUN: {
                os << "ASTNode<Fun>(" << node.symbol << ") with " << node.children.size() << " children:" << endl;
            }; break;
            case ASTType::OP: {
                os << "ASTNode<Op>(" << node.symbol << ") with " << node.children.size() << " children:" << endl;
            }; break;
            case ASTType::STATEMENT: {
                os << "ASTNode<Statement> with " << node.children.size() << " children:" << endl;
            }; break;
            case ASTType::VAR: {
                os << "ASTNode<Var>(" << node.symbol << ") with " << node.children.size() << " children:" << endl;
            }; break;
            case ASTType::IF: {
                os << "ASTNode<If> with " << node.children.size() << " children:" << endl;
            }; break;
            default: {
                os << "ASTNode<NotHandled> with " << node.children.size() << " children:" << endl;
            }; break;
        }
        for(int i = 0; i < node.children.size(); i++) {
            os << *(node.children[i]);
        }
        return os;
    }
};

struct Parser {
    vector<Token> tokens;
    int at_elem;
    bool done;

    static Parser from_source(string source) {
        Tokenizer t = Tokenizer::from_source(source);
        return Parser { t.tokenize(), 0, false };
    }
    static Parser from_tokens(vector<Token> tokens) {
        return Parser { tokens, 0, false };
    }

    //parse the whole token list; return a single statement node that holds all code
    ASTNode* parse(Env& env) {
        return parse_block_statement(env);
    }

    //parse until reached newline or `)`; return the resulting AST
    ASTNode* parse_line(Env& env) {
        ASTNode* root = parse_next_expression(env);
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::T_DELIM: {
                    if(tokens[at_elem].t == "}") {
                        //assume a block statement ends without a newline
                        //cannot happen when `root` is null
                        return root;
                    } else if(tokens[at_elem].t == ")") {
                        //assume this call has been for a parentheses expression
                        // a definitive end of statement
                        at_elem++;
                        return new ASTNode(ASTType::STATEMENT, {root});
                    } else if(tokens[at_elem].t == "]") {
                        //assume this call has been for a list builder/index
                        // a definitive end of statement
                        at_elem++;
                        return root;
                    } else if(tokens[at_elem].t == ",") {
                        at_elem++;
                        if(root->type != ASTType::CSV) {
                            root = new ASTNode(ASTType::CSV, {root});
                        }
                        root->children.push_back(parse_next_expression(env));
                    } else if(tokens[at_elem].t == "[") {
                        //TODO: SHOULD BE LEGAL
                        parse_error("Expected operator but found '[': THIS IS A TEMPORARY ERROR");
                    } else if(tokens[at_elem].t == "(") {
                        //assume evaluation of previous item
                        if(tokens[at_elem+1].t == ")") {
                            //evaluate with no parameters - `f()`
                            root = apply_evaluate_with_args(root, new ASTNode(ASTType::CSV));
                            at_elem += 2;
                        } else {
                            ASTNode* args = parse_line(env);
                            root = apply_evaluate_with_args(root, new ASTNode(ASTType::CSV));
                        }
                    } else if(tokens[at_elem].t == "{") {
                        parse_error("Expected operator but found '{'");
                    } else {
                        parse_error("Invalid delimiter is found: "s + tokens[at_elem].t);
                    }
                }; break;
                case TokenType::T_SYMBOL: {
                    //after the initial expression, should only be infix operators
                    if(env.is_op(tokens[at_elem].t)) {
                        ASTNode* new_node = new ASTNode(ASTType::OP, {}, tokens[at_elem].t); //read an operator
                        at_elem++;
                        root = insert_op_into_ast(root, new_node, env);
                    } else {
                        // root = insert_into_ast(root, parse_next_expression(env));
                        parse_error("Expected operator but found a symbol: "s + tokens[at_elem].t);
                    }
                }; break;
                case TokenType::T_LITERAL: {
                    parse_error("Expected operator but found a literal");
                }; break;
                case TokenType::T_NEWLINE: {
                    //when reading a line, newline is the definitive end of statement
                    at_elem++;
                    return root;
                }; break;
                case TokenType::T_NONE: {
                    //theoretically should not happen
                    done = true;
                    return root;
                }; break;
            }
        }
        //should never be reached
        parse_error("Reached an unreachable part of 'parse_line'");
        exit(1);
    }

    //parse until `}` is reached; return the resulting AST
    //expect **not** to see `{` as current element
    ASTNode* parse_block_statement(Env& env) {
        ASTNode* root = new ASTNode(ASTType::STATEMENT, vector<ASTNode*>());
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::T_DELIM: {
                    if(tokens[at_elem].t == "}") {
                        //when reading a block statement, closing brace is the definitive end of statement
                        at_elem++;
                        return root;
                    }
                    ASTNode* line = parse_line(env);
                    root->children.push_back(line);
                }; break;
                case TokenType::T_LITERAL: 
                case TokenType::T_SYMBOL: {
                    ASTNode* line = parse_line(env);
                    root->children.push_back(line);
                }; break;
                case TokenType::T_NEWLINE: {
                    at_elem++; //ignore
                }; break;
                case TokenType::T_NONE: {
                    done = true;
                    return root;
                }; break;
            }
        }
        //should never be reached
        parse_error("Reached an unreachable part of 'parse_block_statement'");
        exit(1);
    }

    //parse and return just the next expression
    //expression is an AST that is independent from any other code: literal, variable, function call, 
    // () or {} expression, unary operator + other expression etc.
    //do not try to look ahead; as soon as it can stop parsing, it will (`a + b` is *not* a single expression, but `+(a,b)` is)
    ASTNode* parse_next_expression(Env& env) {
        switch(tokens[at_elem].type) {
            case TokenType::T_DELIM: {
                if(tokens[at_elem].t == "(") {
                    at_elem++;
                    return parse_line(env);
                } else if(tokens[at_elem].t == "{") {
                    at_elem++;
                    return parse_block_statement(env);
                } else if(tokens[at_elem].t == "[") {
                    //when parsing `[` as an expression, assume it's a list builder, not collection index
                    at_elem++;
                    ASTNode* elements = parse_line(env);
                    if(elements->type != ASTType::CSV) elements = new ASTNode(ASTType::CSV, {elements});
                    return new ASTNode(ASTType::LIST_BUILDER, {elements});
                } else if(tokens[at_elem].t == "}") {
                    //expression must not start with a `}`
                    parse_error("Reached end of statement ('}') when expected an expression");
                } else if(tokens[at_elem].t == ")") {
                    //expression must not start with a `)`
                    parse_error("Reached end of statement (')') when expected an expression");
                } else if(tokens[at_elem].t == "]") {
                    //expression must not start with a `)`
                    parse_error("Reached end of statement (']') when expected an expression");
                } else {
                    parse_error("Encountered an unknown delimiter");
                }
            }; break;
            case TokenType::T_LITERAL: {
                at_elem++;
                return new ASTNode(ASTType::LITERAL, RRObj(tokens[at_elem-1]) );
            }; break;
            case TokenType::T_SYMBOL: {
                //takes care of: unary ops, function-like op calls, functions, variables, if/else
                if(tokens[at_elem].t == "if") {
                    at_elem++; //skip `if`
                    ASTNode* if_statement = new ASTNode(ASTType::IF);
                    if_statement->children.push_back(parse_next_expression(env)); //the condition
                    if_statement->children.push_back(parse_next_expression(env)); //the if block
                    if(tokens[at_elem].t != "else") {
                        parse_error("'If' must have an 'else' clause: THIS IS A TEMPORARY ERROR");
                    }
                    at_elem++; //skip `else`
                    if_statement->children.push_back(parse_next_expression(env)); //the else block
                    return if_statement;
                } else if(tokens[at_elem].t == "else") {
                    parse_error("Cannot read 'else' without 'if'");
                } else if(env.is_op(tokens[at_elem].t)) {
                    ASTNode* new_node = new ASTNode(ASTType::OP, tokens[at_elem].t); //read an operator
                    at_elem++;
                    if(tokens[at_elem].t != "(") {
                        //it's indeed a unary operator usage
                        new_node->children.push_back(parse_next_expression(env));
                    }
                    //else it's a function-like op call
                    return new_node;
                } else if(env.is_fun(tokens[at_elem].t)) {
                    ASTNode* new_node = new ASTNode(ASTType::FUN, tokens[at_elem].t); //read a function
                    at_elem++;
                    // don't assume evaluation
                    return new_node;
                } else {
                    //assume a variable
                    at_elem++;
                    return new ASTNode(ASTType::VAR, tokens[at_elem-1].t);
                }
            }; break;
            case TokenType::T_NEWLINE: {
                parse_error("Reached end of line when expected an expression");
            }; break;
            case TokenType::T_NONE: {
                parse_error("Reached end of file when expected an expression");
            }; break;
        }
        //should never be reached
        parse_error("Reached an unreachable part of 'parse_next_expression'");
        exit(1);
    }

    
    //Insert infix op `to_insert` into `root`
    //Note that it will automatically read the next expression and insert it as rhs for `to_insert`
    ASTNode* insert_op_into_ast(ASTNode* root, ASTNode* to_insert, Env& env) {
        if(root == nullptr) return to_insert; //should not be needed anymore
        if(to_insert->type == ASTType::OP) {
            if(root->type == ASTType::OP) {
                if(env.op_priority_higher(root->symbol, to_insert->symbol)) {
                    root->children.back() = insert_op_into_ast(root->children.back(), to_insert, env);
                    return root;
                } else {
                    //an operator (to_insert) will take the next expression as right side argument
                    to_insert->children.push_back(root);
                    to_insert->children.push_back(parse_next_expression(env));
                    return to_insert;
                }
            } else if(root->type == ASTType::CSV) {
                //always skip a CSV Node
                root->children.back() = insert_op_into_ast(root->children.back(), to_insert, env);
                return root;
            } else {
                //an operator (to_insert) will take the next expression (root) as right side argument
                to_insert->children.push_back(root);
                to_insert->children.push_back(parse_next_expression(env));
                return to_insert;
            }
        }
        parse_error("Trying to insert an illegal expression instead of an operator");
        exit(1);
    }
};

/*
    Functions
*/

//a funny lil function
ASTNode* apply_evaluate_with_args(ASTNode* root, ASTNode* args) {
    if(root->type != ASTType::OP) {
        return new ASTNode(ASTType::EVALUATE, {root, args});
    }
    ASTNode* head = root;
    //if an operator has no children, it can only mean that it's used as a function-like op call; meaning *do* apply args to it
    while(head->children.back()->type == ASTType::OP && head->children.back()->children.size() != 0) {
        head = head->children.back();
    }
    head->children.back() = new ASTNode(ASTType::EVALUATE, {head->children.back(), args});
    return root;
}

//oh hey, another funny lil function
ASTNode* apply_index(ASTNode* root, ASTNode* index) {
    if(root->type != ASTType::OP) {
        return new ASTNode(ASTType::INDEX, {root, index});
    }
    ASTNode* head = root;
    while(head->children.back()->type == ASTType::OP) {
        head = head->children.back();
        if(head->children.size() == 0) parse_error("Trying to index into an operator");
    }
    head->children.back() = new ASTNode(ASTType::INDEX, {head->children.back(), index});
    return root;
}

/*
struct Token {
    string t;
    TokenType type;
    TokenInfo info;
};

enum TokenType {
    NEWLINE,
    DELIM,
    LITERAL, //L_
    SYMBOL, //S_
    NONE
};

enum TokenInfo {
    L_INT, L_FLOAT, L_BOOL, L_STR,
    S_LETTER, S_SPECIAL
};
*/

/*

purpose:

for any Expression, determine order of operations

convert SYMBOLs into:
- recognized keywords
 - none for now
- function name (defaults to a variable)
- variable name

no nodes for: newlines, delimiters - those are 

if an operator is followed by another operator, the second operator ALWAYS gets to execute first (since it's unary on higher importance (assuming prefix unary op))

*/

/*

var/literal == same thing
fun call == same, but expect args
operator == connect expression and expression/(unary)operator

*/

/*

when no postfix operators are allowed, there are 4 types of fun calls:
- a op b
- op a
- fun(a,b,c)
- a.fun(b,c,d)
- a op (b,c,d) may be legal as well, but should not be aimed to be legal syntax

in all cases after an operator(function) there is an operand(parameters) provided
so, . indicates a function call where a calling object is the first parameter to the function
in a way, that's similar to having a op (b,c), just fun being labeled as a function
the difference between functions and operators is purely order of operations: operators' order of operations can be rearranged
for that reason i should treat op and fn differently at the parser
while they both execute things similarly, ops have an order/priority and fns don't
for that logic ops are distinctly different from fns when parsing the tree

*/

/*

so i have a *little* problem

let's say this is the program:
```rr
arr = [1,2,3,4]
arr[2] = arr[0]
```

when i parse the program and see `arr`, it's just a variable
so i store it as a variable

but then, i see that it has an index
so i need to attach that index somehow to the variable

but then i also need to do something similar with `fun(1,2,3)`, basically "injecting" the CSV into the function `fun`

but then...

i can have either of these:
```rr
fun(1,2,3)(4,5) // where `fun(1,2,3)` returns a function pointer, and i immideately call that function
fun(1,2,3)[4] //where `fun(1,2,3)` returns an array, and i index into it
```

now i need several layers of `calls` or `indexes`
and it can keep going, `fun(1)(3)(5)(7)...`, arbitrarily long
so i'd probably need to stop storing `1` as a child of `fun` in `fun(1)`
and make `fun` a child of `FunEval`
but i'd also need to store `1` as a child of `FunEval` ASTNode (because it could've been `fun(1,2)` or `fun(1+2)`)
and i'd also need to store indexing as `IndexInto` ASTNode
I'd either need to store `fun` as the first or last element of that indexing operation

but now i have 2 things to do:
- all functions need to, after .eval, be converted into an RRObj, from RRFun; meaning wrapping function pointers as an object
- i'd need to look for `(` or `[` as part of `parse_line` and emplace it instead of the last read thing; 
though that's probably as easy as going down the operators and finding the first right-most non-op

i also need to handle empty arguments! `fun()`

in general, i'll need to research how it's actually done
and ask Evan about how his datatypes work; i'll need that very soon

all in all, indexing into arrays is not making it into logsday log 4

*/

/*

what do i do for this:

!(list)[1]

it's probably the same as:

!list[1]

would be useful to make it so that it does this:

!(list[1])

so when i find an evaluation (`symbol(...)`) or index (`symbol[...]`), i insert them under the unary op (because it's an op)

but what if i have this then:

function(list)[1]

when it's a function, it's distinctly taking `list` as an argument, before the index.

so i either want to check if something is a unary op or a function and go off of that,
or look if there are () around arguments, and then always treat it as a function call

but the second solution is bad too, because of this example:

!(list_from 5)[0]

it would make sense to interpret it as:

!((list_from 5)[0])

and not

(!(list_from 5))[0]

and doing this would be against rules as well:

!list_from 5[0]

and can be avoided by doing

!list_from(5)[0]

but that seems jank and unintuitive
but is this better?

!((list_from 5)[0])

ok but now this:

!true || false

is it
(!true) || false

or
!(true || false)

of course the correct answer is the first - `||` cannot be distributed into `!`
but now i need to do different things on encountering evalutation/index vs operator... which is probably ok

actually this seems ok all of a sudden:

!list_from(5)[0] -> !(list_from(5)[0]) can be done for clarification

but ! list_from 5 [0] is also clearly !list_from(5[0])

so in my first example:

!(list)[1]

is actually not
!list[1]

but
(!list)[1] <-> (!(list))[1]

ok
not what i expected to conclude, but ok

can be changed later anyway so it's whatever

so when i code, it means that whenever i see an operator, i read the next operand, as usual

but when i see a symbol that's an operator in parse_next_expression, i should not unwrap statements

arguments that are just arguments, are put as children for the unary op

arguments that are statements, should be interpreted as evaluating nodes, taking the unary op as the child
making it a function call

after that any other evaluations or indexings, will be done to the result, not the operand

wait that works out so beautifully actually
i don't even need to make a node "unary_operator" as i was planning to... that's cool actually

*/

/*
ASTNode (evaluate/index) n
n.children[0] will be the function/collection
- should return a function pointer object if an evaluate node
  - function nodes should evaluate to function pointer objects
- just return the collection as usual if index
n.children[1] will be the operands/index
- if index, just 1 element - n.children[1]
  - either make a switch based on collection or call an index function for that collection... which is probably the way to do it
- if evaluate, also have only 1 element - it will be a statement
  - statement with 0 children is just a simple f() call
  - statements with more children are: f(param1, param2, param3, ...)
*/