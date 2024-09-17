#pragma once

#include <string>
#include <vector>
#include "tokenizer.h"

#include <iostream>

using namespace std;

/*
    Nodes
*/

struct TokenNode {
    Token token;
    int chnum;
    TokenNode* next;
};

/*
    Parser
*/

struct Parser {
    Tokenizer t;
    
    static Parser new_empty() {
        return Parser { Tokenizer::new_empty() };
    }
    static Parser new_from_tokenizer(Tokenizer t) {
        return Parser { t };
    }

    //using the given tokenizer, parse an AST for the next line
    TokenNode* next() {
        TokenNode* root = nullptr;
        Token token = t.next();
        while(token.type != TokenType::DELIM) {
            switch(token.type) {
                case TokenType::NONE: return root;
                case TokenType::VAL: {
                    cout << "-- parse value" << endl;
                    //replace root with a number
                    root = new TokenNode { token, 0, nullptr };
                }; break;
                case TokenType::FUN: {
                    cout << "-- parse function" << endl;
                    //put in an operator and read the next operand
                    Token rop = t.next();
                    TokenNode* lop_node = root;
                    root = new TokenNode { token, 2, new TokenNode[2] };
                    root->next[0] = *lop_node;
                    root->next[1] = TokenNode { rop, 0, nullptr };
                }; break;
                default: break;
            }
            token = t.next();
        }
        return root;
    }
};

/*
    Helper Functions
*/

void delete_tree_recur(TokenNode* root) {
    for(int i = 0; i < root->chnum; i++) {
        delete_tree_recur(&root->next[i]);
    }
    if(root->next != nullptr) {
        delete[] root->next;
    }
}

void delete_tree(TokenNode* root) {
    delete_tree_recur(root);
    delete root;
}