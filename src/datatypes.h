#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

using namespace std;

//types stored in place:
// Int, Float, Bool
//types stored behind pointers:
// Str, FnPtr, Vec, Set, Map, List, Pair

/*
    the order is the "importance":
    - disambiguations will be checked in this order
    - lower can be cast into latter, when needed; aka simple to complex
    
    while `Any` is not a legal datatype, it may be specified in function singitures
*/
vector<string> datatypes = {"Bool", "Int", "Float", "Str", "Pair", "Set", "Vec", "Map", "List", "Fn", "None", "Any"};
vector<int> datatype_template_params = {0,  0,  0,   0,     2,      1,     1,     2,     0,      0,    0,      0 };
unordered_map<string, int> datatypes_num;

const int DATATYPE_ANY = datatypes.size()-1;

//call once as part of program setup
void init_datatypes() {
    for(int i = 0; i < datatypes.size(); i++) {
        datatypes_num[datatypes[i]] = i;
    }
}

//get a string name of the single type number `i`
string single_type_of(int i) {
    return datatypes[i];
}
//get the number of the single type string `str`
int single_type_of(string i) {
    return datatypes_num[i];
}

struct RRDataType {
    int type;

    RRDataType() {
        this->type = single_type_of("None");
    }
    RRDataType(string str) {
        this->type = single_type_of(str);
    }
    RRDataType(Token literal) {
        switch (literal.info) {
            case TokenInfo::L_BOOL: type = single_type_of("Bool"); break;
            case TokenInfo::L_FLOAT: type = single_type_of("Float"); break;
            case TokenInfo::L_INT: type = single_type_of("Int"); break;
            case TokenInfo::L_STR: type = single_type_of("Str"); break;
            default: parse_error("Creating a DataType out of a non-literal token");
        }
    }

    friend bool operator==(const RRDataType& lhs, const RRDataType& rhs) {
        return lhs.equivalent_to(rhs);
    }
    //return true if this object is *equivalent* to rhs
    //rhs may include datatype `Any`
    bool equivalent_to(const RRDataType& rhs) const {
        return (type == rhs.type) || rhs.type == DATATYPE_ANY;
    }
};