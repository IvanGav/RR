// Given a list of tokens, convert it into an Abstract Syntax Tree
// For format, refer to ../syntax.md

#pragma once

#include <string>
#include <vector>
#include <variant>

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
    FUN, //== operator
    IF,
    LOOP,
    FUN_DECL,
    RETURN,
    CSV, //comma separated values
};

struct ASTNode;

ASTNode* insert_into_ast(ASTNode* root, ASTNode* to_insert);

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
    ASTNode(ASTType type, vector<ASTNode*> children, string symbol_name) {
        this->type = type;
        this->children = children;
        // this->symbol;
        // this->symbol = string(symbol_name);
        new (&this->symbol) string(symbol_name);
    }
    ~ASTNode() {
        // cout << "--destructor for ASTNode is called" << endl;
        switch(type) {
            case ASTType::LITERAL: literal.~RRObj(); break;
            case ASTType::FUN: symbol.~string(); break;
            case ASTType::VAR: symbol.~string(); break;
            default: break; //STATEMENT
        }
    }
    ASTNode& operator=(const ASTNode& val) {
        // cout << "--operator= for ASTNode is called" << endl;
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
            case ASTType::IF: {
            }; break;
            case ASTType::LOOP: {
            }; break;
            case ASTType::LITERAL: {
                return literal;
            }; break;
            case ASTType::VAR: {
                return env.get_var(symbol);
            }; break;
            case ASTType::FUN: {
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
            case ASTType::STATEMENT: {
                os << "ASTNode<Statement> with " << node.children.size() << " children:" << endl;
            }; break;
            case ASTType::VAR: {
                os << "ASTNode<Var>(" << node.symbol << ") with " << node.children.size() << " children:" << endl;
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
    ASTNode* parse() {
        return parse_block_statement();
    }

    //parse until reached newline; return the resulting AST
    //REVISIT LATER
    ASTNode* parse_line() {
        // cout << "----parsing a line" << endl;
        ASTNode* root = nullptr;
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::T_NEWLINE: {
                    // cout << "----encountered a newline" << endl;
                    //when reading a line, newline is the definitive end of statement
                    at_elem++;
                    return root;
                }; break;
                case TokenType::T_DELIM: {
                    // cout << "----parsing a delim" << endl;
                    if(tokens[at_elem].t == "}") {
                        //block statement ends without a newline
                        return root;
                    }
                    if(tokens[at_elem].t == ")") {
                        //assume this call has been for parentheses
                        at_elem++;
                        return new ASTNode(ASTType::STATEMENT, {root});
                    }
                    if(tokens[at_elem].t == "(") {
                        at_elem++;
                        root = insert_into_ast(root, parse_line());
                    }
                }; break;
                case TokenType::T_SYMBOL: {
                    // cout << "----parsing a symbol" << endl;
                    if(op_order.find(tokens[at_elem].t) == op_order.end()) {
                        // cout << "--variable " << tokens[at_elem].t << endl;
                        ASTNode* new_node = new ASTNode(ASTType::VAR, {}, tokens[at_elem].t);
                        root = insert_into_ast(root, new_node);
                    } else {
                        // cout << "--operator " << tokens[at_elem].t << endl;
                        //is an operator
                        ASTNode* new_node = new ASTNode(ASTType::FUN, {}, tokens[at_elem].t);
                        root = insert_into_ast(root, new_node);
                    }
                    at_elem++;
                }; break;
                case TokenType::T_LITERAL: {
                    // cout << "----parsing a litral" << endl;
                    if(root == nullptr) {
                        root = new ASTNode(ASTType::LITERAL, RRObj(tokens[at_elem]) );
                    } else {
                        root = insert_into_ast(root, new ASTNode(ASTType::LITERAL, RRObj(tokens[at_elem]) ));
                    }
                    at_elem++;
                }; break;
                case TokenType::T_NONE: {
                    done = true;
                    return root;
                }; break;
            }
        }
        //should never be reached
        return root;
    }

    //parse until `}` is reached; return the resulting AST
    //expect **not** to see `{` as current element
    ASTNode* parse_block_statement() {
        ASTNode* root = new ASTNode(ASTType::STATEMENT, vector<ASTNode*>());
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::T_NEWLINE: {
                    //ignore
                    at_elem++;
                }; break;
                case TokenType::T_DELIM: {
                    if(tokens[at_elem].t == "}") {
                        //when reading a block statement, closing brace is the definitive end of statement
                        at_elem++;
                        return root;
                    }
                    ASTNode* line = parse_line();
                    root->children.push_back(line);
                }; break;
                case TokenType::T_LITERAL: {
                    ASTNode* line = parse_line();
                    root->children.push_back(line);
                }; break;
                case TokenType::T_SYMBOL: {
                    ASTNode* line = parse_line();
                    root->children.push_back(line);
                }; break;
                case TokenType::T_NONE: {
                    done = true;
                    return root;
                }; break;
            }
        }
        //should never be reached
        return root;
    }
};

/*
    Functions
*/

//Insert `to_insert` into `root`
//REVISIT LATER
ASTNode* insert_into_ast(ASTNode* root, ASTNode* to_insert) {
    // cout << "----inserting into AST " << *to_insert << endl;
    if(root == nullptr) return to_insert;
    switch (to_insert->type) {
        case ASTType::LITERAL: {
            if(root->type == ASTType::FUN) {
                //if can go lower, do it
                if(root->children.size() == 2 && root->children[1]->type == ASTType::FUN) {
                    root->children[1] = insert_into_ast(root->children[1], to_insert);
                    return root;
                } else {
                    //if lowest, insert
                    root->children.push_back(to_insert);
                    return root;
                }
            }
        }; break;
        case ASTType::VAR: {
            if(root->type == ASTType::FUN) {
                //if can go lower, do it
                if(root->children.size() == 2 && root->children[1]->type == ASTType::FUN) {
                    root->children[1] = insert_into_ast(root->children[1], to_insert);
                    return root;
                } else {
                    //if lowest, insert
                    root->children.push_back(to_insert);
                    return root;
                }
            }
        }; break;
        case ASTType::STATEMENT: {
            if(root->type == ASTType::FUN) {
                //if can go lower, do it
                if(root->children.size() == 2 && root->children[1]->type == ASTType::FUN) {
                    root->children[1] = insert_into_ast(root->children[1], to_insert);
                    return root;
                } else {
                    //if lowest, insert
                    root->children.push_back(to_insert);
                    return root;
                }
            }
        }; break;
        case ASTType::FUN: {
            if(root->type == ASTType::FUN) {
                if(op_order[to_insert->symbol] >= op_order[root->symbol]) {
                    to_insert->children.push_back(root);
                    return to_insert;
                } else {
                    root->children[1] = insert_into_ast(root->children[1], to_insert);
                    return root;
                }
            } else {
                //assume a value
                to_insert->children.push_back(root);
                return to_insert;
            }
        }; break;
        case ASTType::IF: break;
        case ASTType::LOOP: break;
        case ASTType::FUN_DECL: break;
        case ASTType::RETURN: break;
        case ASTType::CSV: break;
    }
    return nullptr;
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