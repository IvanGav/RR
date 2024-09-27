// Given a list of tokens, convert it into an Abstract Syntax Tree
// For format, refer to ../syntax.md

#pragma once

#include <string>
#include <vector>

#include "tokenizer.h"

using namespace std;

/*
    Definitions
*/



/*
    Structs
*/

struct ASTNode {

};

struct Parser {
    vector<Token> tokens;
    int at_elem;

    static Parser from_source(string source) {
        Tokenizer t = Tokenizer::from_source(source);
        return Parser { t.tokenize(), 0 };
    }
};

/*

purpose:

for any Expression, determine order of operations

convert SYMBOLs into:
- recognized keywords
- function name (defaults to a variable)
- variable name

no nodes for: newlines, delimiters

*/