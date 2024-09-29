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

enum CharType {
    LETTER,
    NUMBER,
    C_DELIM,
    WHITESPACE,
    C_NEWLINE,
    STR, //quotation marks `"`
    SPECIAL, //anything else: `+=-*/<>~!@#$%^&|` etc
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

    static CharClassifier empty() {
        CharClassifier cc;
        //add letters
        for(char c = 'a'; c <= 'z'; c++)
            cc.chars[c] = CharType::LETTER;
        for(char c = 'A'; c <= 'Z'; c++)
            cc.chars[c] = CharType::LETTER;
        cc.chars['_'] = CharType::LETTER; //yes, underscore **is** a letter
        //add numbers
        for(char c = '0'; c <= '9'; c++)
            cc.chars[c] = CharType::NUMBER;
        //add delimiters
        cc.chars['('] = CharType::C_DELIM;
        cc.chars[')'] = CharType::C_DELIM;
        cc.chars['['] = CharType::C_DELIM;
        cc.chars[']'] = CharType::C_DELIM;
        cc.chars['{'] = CharType::C_DELIM;
        cc.chars['}'] = CharType::C_DELIM;
        cc.chars['.'] = CharType::C_DELIM;
        cc.chars[','] = CharType::C_DELIM;
        //add whitespaces
        cc.chars[' '] = CharType::WHITESPACE;
        cc.chars['\t'] = CharType::WHITESPACE;
        cc.chars['\r'] = CharType::WHITESPACE;
        //add newlines
        cc.chars[';'] = CharType::C_NEWLINE;
        cc.chars['\n'] = CharType::C_NEWLINE;
        //add string marker
        cc.chars['"'] = CharType::STR;
        cc.chars['\''] = CharType::STR;
        //add symbols
        cc.chars['*'] = CharType::SPECIAL;
        cc.chars['+'] = CharType::SPECIAL;
        cc.chars['='] = CharType::SPECIAL;
        cc.chars['-'] = CharType::SPECIAL;
        cc.chars['/'] = CharType::SPECIAL;
        cc.chars['|'] = CharType::SPECIAL;
        cc.chars['\\'] = CharType::SPECIAL;
        cc.chars['&'] = CharType::SPECIAL;
        cc.chars['^'] = CharType::SPECIAL;
        cc.chars['%'] = CharType::SPECIAL;
        cc.chars['#'] = CharType::SPECIAL;
        cc.chars['!'] = CharType::SPECIAL;
        cc.chars['`'] = CharType::SPECIAL;
        cc.chars[':'] = CharType::SPECIAL;
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
        return Tokenizer { "\n", 0, CharClassifier::empty() };
    }
    static Tokenizer from_source(string source) {
        return Tokenizer { source.append(1, '\n'), 0, CharClassifier::empty() };
    }

    //reset this tokenizer to have new source (and start from the beginning)
    void set_source(string source) {
        this->source = source.append(1, '\n');
        this->at_char = 0;
    }

    //return the next token
    Token next() {
        //check for eof
        if(done()) return Token { "", TokenType::NONE };
        //skip whitespace
        while(cc.type_of(source[at_char]) == CharType::WHITESPACE) at_char++;
        //identify the first character to look at
        CharType ctype = cc.type_of(source[at_char]);
        //if found a newline, get it
        if(ctype == CharType::C_NEWLINE) {
            at_char++;
            return Token { "\n", TokenType::NEWLINE };  //it's ok to treat `;` as `\n`
        }
        //if found a delimiter, get it
        if(ctype == CharType::C_DELIM) {
            at_char++;
            return Token { string() + prev_char(), TokenType::DELIM }; //delimiters are **always** 1 character long
        }
        string token_str;
        //if found string or number, read a literal
        if(ctype == CharType::STR) {
            at_char++;
            while(source[at_char] != '"') {
                token_str += source[at_char];
                at_char++;
            }
            at_char++;
            return Token { token_str, TokenType::LITERAL, TokenInfo::L_STR };
        }
        if(ctype == CharType::NUMBER) {
            bool is_float = false;
            do {
                if(source[at_char] == '.') is_float = true;
                token_str += source[at_char];
                at_char++;
                ctype = cc.type_of(source[at_char]);
            } while(ctype == CharType::NUMBER || (source[at_char] == '.' && !is_float));
            at_char++;
            return Token { token_str, TokenType::LITERAL, is_float ? TokenInfo::L_FLOAT : TokenInfo::L_INT };
        }
        //if found a letter, read a symbol until non-letter/number
        if(ctype == CharType::LETTER) {
            do {
                token_str += source[at_char];
                at_char++;
                ctype = cc.type_of(source[at_char]);
            } while(ctype == CharType::LETTER || ctype == CharType::NUMBER);
            at_char++;
            return Token { token_str, TokenType::SYMBOL, TokenInfo::S_LETTER };
        }
        //read a symbol until non-special
        if(ctype == CharType::SPECIAL) {
            do {
                token_str += source[at_char];
                at_char++;
                ctype = cc.type_of(source[at_char]);
            } while(ctype == CharType::SPECIAL);
            at_char++;
            return Token { token_str, TokenType::SYMBOL, TokenInfo::S_SPECIAL };
        }
    }

    //return all of the remaining tokens in a vector
    vector<Token> tokenize() {
        vector<Token> ts;
        while((*(ts.end()-1)).type != TokenType::NONE) {
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