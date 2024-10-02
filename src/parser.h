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
};

struct ASTNode;
void apply_operands(ASTNode* fun, ASTNode* child);

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
                //funny thing is that `=(ten, 10)` is not a legal assignment function call
                if(symbol == "=") {
                    return env.assign_var(children[0]->symbol, children[1]->eval(env));
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
            default: break;
        }
        parse_error(string("Invalid statement encountered: ")+to_string(type));
        return RRObj();
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
        ASTNode* root = nullptr;
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::T_DELIM: {
                    if(tokens[at_elem].t == "}") {
                        //assume a block statement ends without a newline
                        return root;
                    } else if(tokens[at_elem].t == ")") {
                        //assume this call has been for a parentheses expression
                        // a definitive end of statement
                        at_elem++;
                        return new ASTNode(ASTType::STATEMENT, {root});
                    } else if(tokens[at_elem].t == "(") {
                        at_elem++;
                        root = insert_op_into_ast(root, parse_line(env), env);
                    } else if(tokens[at_elem].t == ",") {
                        //code here...
                    } else {
                        parse_error("Invalid delimiter is found");
                    }
                }; break;
                case TokenType::T_SYMBOL: {
                    //after the initial expression, should only be infix operators
                    if(root == nullptr) {
                        root = parse_next_expression(env);
                    } else if(env.is_op(tokens[at_elem].t)) {
                        ASTNode* new_node = new ASTNode(ASTType::OP, {}, tokens[at_elem].t); //read an operator
                        at_elem++;
                        root = insert_op_into_ast(root, new_node, env);
                    } else {
                        // root = insert_into_ast(root, parse_next_expression(env));
                        parse_error("Expected operator but found a symbol");
                    }
                }; break;
                case TokenType::T_LITERAL: {
                    if(root == nullptr) {
                        root = parse_next_expression(env);
                    } else {
                        parse_error("Expected operator but found a literal");
                    }
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
                } else if(tokens[at_elem].t == "}") {
                    //expression must not start with a `}`
                    parse_error("Reached end of statement when expected an expression");
                } else if(tokens[at_elem].t == ")") {
                    //expression must not start with a `)`
                    parse_error("Reached end of statement when expected an expression");
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
                parse_error("Trying to insert a literal, variable, statement or a function call");
            }; break;
            case ASTType::IF: break;
            case ASTType::LOOP: break;
            case ASTType::FUN_DECL: break;
            case ASTType::RETURN: break;
            case ASTType::CSV: break;
        }
        return nullptr;
    }
};

/*
    Functions
*/

//a funny lil function
void apply_operands(ASTNode* fun, ASTNode* child) {
    //if csv OR a statement with 1 non-operator child
    //for not don't bother to unwrap more than 1 layer of redundant statements
    if(
        (child->type == ASTType::CSV) ||
        (child->type == ASTType::STATEMENT && child->children.size() == 1 && child->children[0]->type != ASTType::OP)
    ) {
        //expand all values
        for(int i = 0; i < child->children.size(); i++)
            fun->children.push_back(child->children[i]);
        delete child;
    } else {
        fun->children.push_back(child);
    }
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