// for now assume all numbers are ints

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "tokenizer.h"
#include "parser.h"

using namespace std;

/*
    Definitions
*/
int to_num(string data);

/*
    Structs
*/
struct Interpreter {
    Parser p;

    static Interpreter new_empty() {
        return Interpreter { Parser::new_empty() };
    }
    static Interpreter new_from_parser(Parser p) {
        return Interpreter { p };
    }

    void run() {
        TokenNode* root = p.next();
        cout << recur_run(root) << endl;
        delete_tree(root);
    }

    private:
    int recur_run(TokenNode* root) {
        if(root->token.type == TokenType::VAL) {
            cout << "-- saw value " << root->token.data << endl;
            return to_num(root->token.data);
        } else if(root->token.type == TokenType::FUN) {
            string fun = root->token.data;
            cout << "-- saw function " << fun << endl;
            if(fun == "+") {
                return recur_run(&root->next[0]) + recur_run(&root->next[1]);
            } else if(fun == "-") {
                return recur_run(&root->next[0]) - recur_run(&root->next[1]);
            } else if(fun == "*") {
                return recur_run(&root->next[0]) * recur_run(&root->next[1]);
            } else if(fun == "/") {
                return recur_run(&root->next[0]) / recur_run(&root->next[1]);
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
};

int to_num(string data) {
    return stoi(data, nullptr);
}