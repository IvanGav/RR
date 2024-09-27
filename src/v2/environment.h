#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "datatypes.h"

using namespace std;

//types stored in place:
// Int, Float, Bool
//types stored behind pointers:
// Str, FnPtr, Vec, Set, Map, List, Pair

/*
"" = str
[] = vec = can store several elements of the *exact same type*
|| = list = can store several elements of *different types*
v:w = pair of (v,w)

Any = not a type that can be assigned to a variable, but can be used for "template" arguments
So, list is basically a vector of Any elements

operators `Vec`, `Set`, `List` can be used to convert between the types
*/

/*
    Definitions
*/

/*
    Functions
*/

/*
    Structs
*/

struct RRDataType {
    int type;

    static RRDataType from_str(string str) {
        return RRDataType { {single_type_of(str)} };
    }

    bool operator==(RRDataType const& rhs) {
        return this->type == rhs.type;
    }
    //return true if this object is *equivalent* to rhs
    //rhs may include datatype `Any`
    bool equivalent_to(RRDataType const& rhs) {
        return (*this == rhs) || rhs.type == DATATYPE_ANY;
    }
};

//takes ownership of the data
struct RRObj {
    vector<int> type;
    void* data;
};

struct RRFun {
    vector<int> params;
    void* cpp_fun;
    void* rr_fun;
};

struct Env {

};