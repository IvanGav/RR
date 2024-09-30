// For format, refer to ../syntax.md

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

/*
    Definitions
*/

enum TokenType {
    T_NEWLINE,
    T_DELIM,
    T_LITERAL, //L_
    T_SYMBOL, //S_
    T_NONE
};

enum TokenInfo {
    L_INT, L_FLOAT, L_BOOL, L_STR,
    S_LETTER, S_SPECIAL
};

enum CharType {
    C_LETTER,
    C_NUMBER,
    C_DELIM,
    C_WHITESPACE,
    C_NEWLINE,
    C_STR, //quotation marks `"`
    C_SPECIAL, //anything else: `+=-*/<>~!@#$%^&|` etc
};

/*
    Structs
*/

struct Token {
    string t;
    TokenType type;
    TokenInfo info;
};

struct CharClassifier {
    unordered_map<char, CharType> chars;

    CharClassifier() {
        //add letters
        for(char c = 'a'; c <= 'z'; c++)
            chars[c] = CharType::C_LETTER;
        for(char c = 'A'; c <= 'Z'; c++)
            chars[c] = CharType::C_LETTER;
        chars['_'] = CharType::C_LETTER; //yes, underscore **is** a letter
        //add numbers
        for(char c = '0'; c <= '9'; c++)
            chars[c] = CharType::C_NUMBER;
        //add delimiters
        chars['('] = CharType::C_DELIM;
        chars[')'] = CharType::C_DELIM;
        chars['['] = CharType::C_DELIM;
        chars[']'] = CharType::C_DELIM;
        chars['{'] = CharType::C_DELIM;
        chars['}'] = CharType::C_DELIM;
        chars['.'] = CharType::C_DELIM;
        chars[','] = CharType::C_DELIM;
        //add whitespaces
        chars[' '] = CharType::C_WHITESPACE;
        chars['\t'] = CharType::C_WHITESPACE;
        chars['\r'] = CharType::C_WHITESPACE;
        //add newlines
        chars[';'] = CharType::C_NEWLINE;
        chars['\n'] = CharType::C_NEWLINE;
        //add string marker
        chars['"'] = CharType::C_STR;
        chars['\''] = CharType::C_STR;
        //add symbols
        chars['*'] = CharType::C_SPECIAL;
        chars['+'] = CharType::C_SPECIAL;
        chars['='] = CharType::C_SPECIAL;
        chars['-'] = CharType::C_SPECIAL;
        chars['/'] = CharType::C_SPECIAL;
        chars['|'] = CharType::C_SPECIAL;
        chars['\\'] = CharType::C_SPECIAL;
        chars['&'] = CharType::C_SPECIAL;
        chars['^'] = CharType::C_SPECIAL;
        chars['%'] = CharType::C_SPECIAL;
        chars['#'] = CharType::C_SPECIAL;
        chars['!'] = CharType::C_SPECIAL;
        chars['`'] = CharType::C_SPECIAL;
        chars[':'] = CharType::C_SPECIAL;
    }

    CharType type_of(char c) {
        return chars[c];
    }
};

struct Tokenizer {
    string source; //should always end with a newline
    int at_char;
    CharClassifier cc;

    static Tokenizer empty() {
        return Tokenizer { "\n", 0, CharClassifier() };
    }
    static Tokenizer from_source(string source) {
        return Tokenizer { source.append(1, '\n'), 0, CharClassifier() };
    }

    //reset this tokenizer to have new source (and start from the beginning)
    void set_source(string source) {
        this->source = source.append(1, '\n');
        this->at_char = 0;
    }

    //return the next token
    Token next() {
        //check for eof
        if(done()) return Token { "", TokenType::T_NONE };
        //skip whitespace
        while(cc.type_of(source[at_char]) == CharType::C_WHITESPACE) at_char++;
        //identify the first character to look at
        CharType ctype = cc.type_of(source[at_char]);
        //if found a newline, get it
        if(ctype == CharType::C_NEWLINE) {
            at_char++;
            return Token { "\n", TokenType::T_NEWLINE };  //it's ok to treat `;` as `\n`
        }
        //if found a delimiter, get it
        if(ctype == CharType::C_DELIM) {
            at_char++;
            return Token { string() + prev_char(), TokenType::T_DELIM }; //delimiters are **always** 1 character long
        }
        string token_str;
        //if found string or number, read a literal
        if(ctype == CharType::C_STR) {
            at_char++;
            while(source[at_char] != '"') {
                token_str += source[at_char];
                at_char++;
            }
            at_char++;
            return Token { token_str, TokenType::T_LITERAL, TokenInfo::L_STR };
        }
        if(ctype == CharType::C_NUMBER) {
            bool is_float = false;
            do {
                if(source[at_char] == '.') is_float = true;
                token_str += source[at_char];
                at_char++;
                ctype = cc.type_of(source[at_char]);
            } while(ctype == CharType::C_NUMBER || (source[at_char] == '.' && !is_float));
            return Token { token_str, TokenType::T_LITERAL, is_float ? TokenInfo::L_FLOAT : TokenInfo::L_INT };
        }
        //if found a letter, read a symbol until non-letter/number
        if(ctype == CharType::C_LETTER) {
            do {
                token_str += source[at_char];
                at_char++;
                ctype = cc.type_of(source[at_char]);
            } while(ctype == CharType::C_LETTER || ctype == CharType::C_NUMBER);
            // at_char++;
            return Token { token_str, TokenType::T_SYMBOL, TokenInfo::S_LETTER };
        }
        //read a symbol until non-special
        if(ctype == CharType::C_SPECIAL) {
            do {
                token_str += source[at_char];
                at_char++;
                ctype = cc.type_of(source[at_char]);
            } while(ctype == CharType::C_SPECIAL);
            return Token { token_str, TokenType::T_SYMBOL, TokenInfo::S_SPECIAL };
        }
        return Token {"", TokenType::T_NONE};
    }

    //return all of the remaining tokens in a vector
    vector<Token> tokenize() {
        vector<Token> ts;
        ts.push_back(next());
        while(ts[ts.size()-1].type != TokenType::T_NONE) {
            ts.push_back(next());
        }
        return ts;
    }

    //return whether read the whole string or not
    bool done() {
        return at_char >= source.size();
    }
    char get_char() {
        return source[at_char];
    }
    char prev_char() {
        return source[at_char-1];
    }
};