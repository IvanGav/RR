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

//higher number means higher priority [0,16)
//0 - lowest priority
//15 - highest priority
const int OP_HIGH_PRI = 15;
const int OP_LOW_PRI = 0;

/*
    Functions
*/

/*
    Structs
*/

struct Env {
    unordered_map<string, RRObj> vars;
    unordered_map<string, vector<RRFun>> funs;
    unordered_map<string, int> op_order;

    static void init_with_default(Env& env) {
        //init funs
        env.funs["+"].push_back(RRFun({RRDataType("Int"), RRDataType("Int")}, int_add_int));
        env.funs["*"].push_back(RRFun({RRDataType("Int"), RRDataType("Int")}, int_multiply_int));
        env.funs["+"].push_back(RRFun({RRDataType("Float"), RRDataType("Float")}, float_add_float));
        env.funs["+"].push_back(RRFun({RRDataType("Float"), RRDataType("Int")}, float_add_int));
        env.funs["+"].push_back(RRFun({RRDataType("Str"), RRDataType("Str")}, str_add_str));
        env.funs["+"].push_back(RRFun({RRDataType("Str"), RRDataType("Int")}, str_add_int));
        env.funs["repeat"].push_back(RRFun({RRDataType("Str"), RRDataType("Int")}, str_repeat_int));
        //init op_order
        env.op_order["="] = OP_LOW_PRI; //both sides get evaluated first
        env.op_order["repeat"] = OP_LOW_PRI+2;
        env.op_order["+"] = OP_HIGH_PRI-5;
        env.op_order["*"] = OP_HIGH_PRI-4;
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

    //check whether the function list contains this name
    bool is_fun(string name) {
        return funs.find(name) == funs.end();
    }
    //if an operator order has been established for this name, it's an operator
    bool is_op(string name) {
        return op_order.find(name) != op_order.end();
    }
};