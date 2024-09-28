#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "datatypes.h"
#include "rr_obj.h"

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

//lower number means higher priority [0,16)
//0 - highest priority
//15 - lowest priority
unordered_map<string, int> op_order;
const int OP_HIGH_PRI = 0;
const int OP_LOW_PRI = 15;

/*
    Functions
*/

void init_op_order() {
    op_order["="] = OP_LOW_PRI; //both sides get evaluated first
    op_order["+"] = OP_HIGH_PRI+5;
    op_order["*"] = OP_HIGH_PRI+4;
}

/*
    Structs
*/

struct Env {
    RRObj get_var(string name) {
        return RRObj { RRDataType::from_str("Any"), nullptr };
    }
};