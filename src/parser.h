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
    STATEMENT, //a singular statement, consisting of one or more nodes
    LITERAL,
    VAR,
    FUN,
    OP,
    IF,
    LOOP,
    FUN_DECL,
    RETURN,
    CSV, //comma separated values
    LIST_BUILDER
};

struct ASTNode;
ASTNode* apply_operands(ASTNode* fun, ASTNode* child);

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
                case ASTType::LITERAL: literal = val.literal; break;
                case ASTType::FUN: symbol = val.symbol; break;
                case ASTType::VAR: symbol = val.symbol; break;
                default: break; //STATEMENT
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
                return children[children.size()-1]->eval(env);
            }; break;
            case ASTType::LITERAL: {
                return literal;
            }; break;
            case ASTType::VAR: {
                return env.get_var(symbol);
            }; break;
            case ASTType::FUN:
            case ASTType::OP: {
                //funny thing is that `=(ten, 10)` is now a legal assignment function call
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
                    RRFun fun = env.get_fun(symbol, types);
                    return fun.cpp_fun(args);
                }
            }; break;
            case ASTType::IF: {
            }; break;
            case ASTType::LOOP: {
            }; break;
            case ASTType::FUN_DECL: {
            }; break;
            case ASTType::RETURN: {
            }; break;
            case ASTType::CSV: {
                rr_runtime_error("Reached a CSV Node");
            }; break;
            case ASTType::LIST_BUILDER: {
                RRObj list_obj = RRObj(new vector<RRObj>());
                for(int i = 0; i < children.size(); i++) {
                    list_obj.data_vec->push_back(children[i]->eval(env));
                }
                return list_obj;
            }; break;
        }
        rr_runtime_error(string("Invalid statement encountered: ")+to_string(type));
        return RRObj();
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

    ASTNode* last_node = nullptr; //TODO: SHOULD BE CHANGED LATER ON

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
                        return new ASTNode(ASTType::STATEMENT, {root});
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
                        parse_error("Expected operator but found '('");
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
                        parse_error("Expected operator but found a symbol");
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
        return nullptr;
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
        return nullptr;
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
                    return apply_operands(new ASTNode(ASTType::LIST_BUILDER), parse_line(env));
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
                //takes care of: unary ops, function-like op calls, functions, variables
                if(env.is_op(tokens[at_elem].t)) {
                    /*
                        The reason I create a FUN not OP is:
                        for expression `!1 + 2` where ! is a unary op, should i allow it to be interpreted like this - `!(1+2)`?
                        if yes, there's a problem. the way how i parse the language, i can't distinguish between:
                        - whether an operator is unary or not (aka prefix/function-like op call: `+(1,2)`)
                        - whether this: `!(1)` is a function-like op call or a use of unary operator 
                        so my answer is no, that's illegal
                        effectively turning `!var` into a function call `!(var)`
                        while it's possible to define all of that, for the time being i just declare that
                        you **cannot** distribute operators into a unary operator (see first line)
                        which also means unary operators always have the highest priority,
                        practically making them just a function call, not really an op
                    */
                    ASTNode* new_node = new ASTNode(ASTType::FUN, {}, tokens[at_elem].t); //read an operator
                    at_elem++;
                    apply_operands(new_node, parse_next_expression(env)); //put the next expression as its only operand
                    return new_node;
                } else if(env.is_fun(tokens[at_elem].t)) {
                    ASTNode* new_node = new ASTNode(ASTType::FUN, {}, tokens[at_elem].t); //read a function
                    at_elem++;
                    apply_operands(new_node, parse_next_expression(env)); //put the next expression as its only operand
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
        return nullptr;
    }

    
    //Insert infix op `to_insert` into `root`
    //Note that it will automatically read the next expression and insert it as rhs for `to_insert`
    ASTNode* insert_op_into_ast(ASTNode* root, ASTNode* to_insert, Env& env) {
        if(root == nullptr) return to_insert; //should not be needed anymore
        switch (to_insert->type) {
            case ASTType::OP: {
                if(root->type == ASTType::OP) {
                    if(env.op_priority_higher(root->symbol, to_insert->symbol)) {
                        root->children[root->children.size()-1] = insert_op_into_ast(root->children[root->children.size()-1], to_insert, env);
                        return root;
                    } else if(root->type == ASTType::CSV) {
                        //always skip a CSV Node
                        root->children[root->children.size()-1] = insert_op_into_ast(root->children[root->children.size()-1], to_insert, env);
                        return root;
                    } else {
                        to_insert->children.push_back(root);
                        //an operator (to_insert) will take the next expression as right side argument
                        apply_operands(to_insert, parse_next_expression(env));
                        return to_insert;
                    }
                } else {
                    //assume a value
                    //an operator (to_insert) will take the next expression as right side argument
                    to_insert->children.push_back(root);
                    apply_operands(to_insert, parse_next_expression(env));
                    return to_insert;
                }
            }; break;
            case ASTType::LITERAL:
            case ASTType::VAR:
            case ASTType::STATEMENT:
            case ASTType::FUN: {
                parse_error("Trying to insert a literal, variable, statement or a function call instead of an operator");
            }; break;
            case ASTType::IF:
            case ASTType::LOOP:
            case ASTType::FUN_DECL:
            case ASTType::RETURN:
            case ASTType::CSV: {
                parse_error("Trying to insert an illegal expression instead of an operator");
            }; break;
        }
        return nullptr;
    }
};

/*
    Functions
*/

//a funny lil function
ASTNode* apply_operands(ASTNode* fun, ASTNode* child) {
    //if csv OR a statement with 1 non-operator child, unwrap
    //unwrap until can't any longer
    while(child->type == ASTType::STATEMENT && child->children.size() == 1 && child->children[0]->type != ASTType::OP) {
        //can have multiple nested layers of statements
        ASTNode* temp = child;
        child = child->children[0];
        delete temp;
    }
    if(child->type == ASTType::CSV) {
        //expand all values
        for(int i = 0; i < child->children.size(); i++)
            fun->children.push_back(child->children[i]);
        delete child;
    } else {
        fun->children.push_back(child);
    }
    return fun;
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