#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "datatypes.h"
#include "rr_obj.h"
#include "cpp_fun_impl.h"

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
    op_order["repeat"] = OP_LOW_PRI-2;
    op_order["+"] = OP_HIGH_PRI+5;
    op_order["*"] = OP_HIGH_PRI+4;
}

/*
    Structs
*/

struct Env {
    unordered_map<string, RRObj> vars;
    unordered_map<string, vector<RRFun>> funs;

    Env() {
        vars = {};
        funs = {};
        funs["+"].push_back(RRFun({RRDataType("Int"), RRDataType("Int")}, int_add_int));
        funs["*"].push_back(RRFun({RRDataType("Int"), RRDataType("Int")}, int_multiply_int));
        funs["+"].push_back(RRFun({RRDataType("Float"), RRDataType("Float")}, float_add_float));
        funs["+"].push_back(RRFun({RRDataType("Float"), RRDataType("Int")}, float_add_int));
        funs["+"].push_back(RRFun({RRDataType("Str"), RRDataType("Str")}, str_add_str));
        funs["+"].push_back(RRFun({RRDataType("Str"), RRDataType("Int")}, str_add_int));
        funs["repeat"].push_back(RRFun({RRDataType("Str"), RRDataType("Int")}, str_repeat_int));
    }

    RRObj get_var(string name) {
        if(vars.find(name) == vars.end()) {
            return vars[name]; //DEAL WITH THIS CASE SOMEHOW
        }
        return vars[name];
    }
    RRFun get_fun(string name, vector<RRDataType> arg_types) {
        if(funs.find(name) == funs.end()) {
            return funs[name][0]; //DEAL WITH THIS CASE SOMEHOW
        }
        for(RRFun f : funs[name]) {
            //check if `f.params` vector is equal to `arg_types` vector
            if(f.params == arg_types) {
                return f;
            }
        }
        return funs[name][0]; //DEAL WITH THIS CASE SOMEHOW
    }
    RRObj assign_var(string name, RRObj obj) {
        vars[name] = obj;
        return obj;
    }
};