// Get a string and return an iterator over whitespace separated tokens.
// Newline is returned as a separate token ('\n' character is returned when encountering any number of newlines).

#include <string>
#include <vector>

// #include <iostream>

using namespace std;

/*
    Constatnts
*/
const string NO_TOKENS = "";

/*
    Definitions
*/
bool is_delimiter(char c);

/*
    Tokenizer struct
*/

struct tokenizer {
    vector<string> raw;
    int at_line;
    int at_char;

    static tokenizer new_empty() {
        return tokenizer { vector<string>(0), 0, 0 };
    }
    static tokenizer new_from_str(string str) {
        vector<string> vec;
        vec.push_back(str);
        return tokenizer { vec, 0, 0 };
    }
    static tokenizer new_from_vec(vector<string> vec) {
        return tokenizer { vec, 0, 0 };
    }

    //for now, split around whitespace and treat newlines as a token
    //return empty string when nothing else to read
    string next() {
        string str;
        // cout << "-- starting..." << endl;
        //if nothing else to read, return
        if(at_line == raw.size()) return NO_TOKENS;
        int size = raw[at_line].size();
        // cout << "-- at_char = " << at_char << ", size = " << size << endl;
        //ignore whitespace
        for(int i = at_char; i < size; i++) {
            char c = raw[at_line][at_char];
            if(c != ' ' && c != '\t') break;
            // cout << "-- skipping char " << c << endl;
            at_char++;
        }
        //check if at the end of a string
        if(at_char == size) {
            at_line++;
            at_char = 0;
            // cout << "-- at the end of a string" << endl;
            return this->next();
        }
        //check for a delimiter
        if(is_delimiter(raw[at_line][at_char])) {
            // cout << "-- at a delimiter " << (int) raw[at_line][at_char] << endl;
            char c = raw[at_line][at_char];
            at_char++;
            return string() + c;
        }
        for(int i = at_char; i < size; i++) {
            char c = raw[at_line][at_char];
            if(c == ' ' || is_delimiter(c)) break;
            // cout << "-- adding char " << c << endl;
            str += c;
            at_char++;
        }
        return str;
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