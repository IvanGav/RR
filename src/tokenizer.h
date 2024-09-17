// Get a string and return an iterator over whitespace separated tokens.
// Newline is returned as a separate token ('\n' character is returned when encountering any number of newlines).

#pragma once

#include <string>
#include <vector>

// #include <iostream>

using namespace std;

/*
    Definitions
*/
bool is_delimiter(char c);
bool is_builtin(string& str);
bool is_numeric(string& str);
bool is_fun(string& str);
struct Token;
struct Tokenizer;

/*
    Data types
*/

enum TokenType {
    VAL /* represents a literal of any integral type */,
    SYMBOL,
    DELIM, /* specifier doesn't matter */
    NONE
};

enum TokenTypeSpecifier {
    T_INT, T_FLOAT, T_BOOL, T_STR,
    VAR,
    FUN, OP,
    BUILTIN, TYPE,
    UNDEF
};

struct Token {
    TokenType type;
    TokenTypeSpecifier type_spec;
    string data;
};

/*
    Constatnts
*/
const Token NO_TOKENS = Token { TokenType::NONE, TokenTypeSpecifier::UNDEF, "" };
const Token NEWLINE_TOKEN = Token { TokenType::DELIM, TokenTypeSpecifier::UNDEF, "\n" };

/*
    Tokenizer struct
*/

struct Tokenizer {
    vector<string> raw;
    int at_line;
    int at_char;

    static Tokenizer new_empty() {
        return Tokenizer { vector<string>(0), 0, 0 };
    }
    static Tokenizer new_from_str(string str) {
        vector<string> vec;
        vec.push_back(str);
        return Tokenizer { vec, 0, 0 };
    }
    static Tokenizer new_from_vec(vector<string> vec) {
        return Tokenizer { vec, 0, 0 };
    }

    //for now, split around whitespace and treat newlines as a token
    //return empty string when nothing else to read
    Token next() {
        string str;
        //if nothing else to read, return
        if(at_line == raw.size()) return NO_TOKENS;
        int size = raw[at_line].size();
        //ignore whitespace
        for(int i = at_char; i < size; i++) {
            char c = raw[at_line][at_char];
            if(c != ' ' && c != '\t') break;
            at_char++;
        }
        //check if at the end of a string
        if(at_char == size) {
            at_line++;
            at_char = 0;
            return NEWLINE_TOKEN;
            // return this->next();
        }
        //check for a delimiter
        if(is_delimiter(raw[at_line][at_char])) {
            char c = raw[at_line][at_char];
            at_char++;
            return Token { TokenType::DELIM, TokenTypeSpecifier::UNDEF, string() + c };
        }
        //read into `str`
        for(int i = at_char; i < size; i++) {
            char c = raw[at_line][at_char];
            if(c == ' ' || is_delimiter(c)) break;
            str += c;
            at_char++;
        }
        //determine the type of token that is the resulting `str` and return
        if(is_value(str)) {
            return Token { TokenType::VAL, TokenTypeSpecifier::UNDEF, str }; //change TokenTypeSpecifier::UNDEF to a known type
        } else {
            return Token { TokenType::SYMBOL, TokenTypeSpecifier::UNDEF, str };
        }
    }
    //add a string to the queue of strings to be tokenized
    void add_str(string str) {
        raw.push_back(str);
    }
};

/*
    Helper Functions
*/

bool is_delimiter(char c) {
    return c == '\n' || c == '(' || c == ')' || c == '{' || c == '}';
}

bool is_value(string& str) {
    if(str == string("true") || str == string("false")) {
        return true;
    }
    string::const_iterator it = str.begin();
    while (it != str.end() && isdigit(*it)) ++it;
    return !str.empty() && it == str.end();
}