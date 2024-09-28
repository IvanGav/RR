// Given a list of tokens, convert it into an Abstract Syntax Tree
// For format, refer to ../syntax.md

#pragma once

#include <string>
#include <vector>

#include "datatypes.h"
#include "rr_obj.h"
#include "tokenizer.h"
#include "environment.h"

using namespace std;

/*
    Definitions
*/

enum ASTType {
    STATEMENT, //a singular statement, consisting of one or more nodes
    IF,
    LOOP,
    LITERAL,
    VAR,
    FUN,
    FUN_DECL,
    RETURN,
};

/*
    Structs
*/
struct ASTNode {
    ASTType type;
    vector<ASTNode> children;
    void* data;

    RRObj eval(Env& env) {
        switch (type) {
            case ASTType::STATEMENT: {
                for(int i = 0; i < children.size()-1; i++) {
                    children[i].eval(env);
                }
                return children[children.size()-1].eval(env);
            }; break;
            case ASTType::IF: {
            }; break;
            case ASTType::LOOP: {
            }; break;
            case ASTType::LITERAL: {
                return *((RRObj*) data);
            }; break;
            case ASTType::VAR: {
                return env.get_var(*((string*) data));
            }; break;
            case ASTType::FUN: {
            }; break;
            case ASTType::FUN_DECL: {
            }; break;
            case ASTType::RETURN: {
            }; break;
            default: break;
        }
        cout << "--RR: Invalid statement encountered: " << type << endl;
        exit(1);
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
    ASTNode* parse_line() {
        ASTNode* root = nullptr;
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::NEWLINE: {
                    //when reading a line, newline is the definitive end of statement
                    at_elem++;
                    return root;
                }; break;
                case TokenType::DELIM: {
                }; break;
                case TokenType::LITERAL: {
                }; break;
                case TokenType::SYMBOL: {
                }; break;
                case TokenType::NONE: {
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
        ASTNode* root = new ASTNode { ASTType::STATEMENT, {}, nullptr };
        while(!done) {
            switch(tokens[at_elem].type) {
                case TokenType::NEWLINE: {
                    //ignore
                    at_elem++;
                }; break;
                case TokenType::DELIM: {
                    if(tokens[at_elem].t == "}") {
                        //when reading a block statement, closing brace is the definitive end of statement
                        at_elem++;
                        return root;
                    }
                    ASTNode* line = parse_line();
                    root->children.push_back(*line);
                }; break;
                case TokenType::LITERAL: {
                    ASTNode* line = parse_line();
                    root->children.push_back(*line);
                }; break;
                case TokenType::SYMBOL: {
                    ASTNode* line = parse_line();
                    root->children.push_back(*line);
                }; break;
                case TokenType::NONE: {
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