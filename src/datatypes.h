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
vector<string> datatypes = {"Bool", "Int", "Float", "Str", "Pair", "Set", "Vec", "Map", "List", "Fn", "Any"};
vector<int> datatype_template_params = {0,  0,      0,      0,     2,      1,      1,     2,     0,     0,    0 };
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