#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "datatypes.h"
#include "rr_obj.h"
#include "cpp_fun_impl.h"
#include "rr_error.h"

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
        env.funs["+"].push_back(RRFun({RRDataType("Float"), RRDataType("Float")}, float_add_float));
        env.funs["+"].push_back(RRFun({RRDataType("Float"), RRDataType("Int")}, float_add_int));
        env.funs["+"].push_back(RRFun({RRDataType("Int"), RRDataType("Float")}, int_add_float));
        env.funs["+"].push_back(RRFun({RRDataType("Str"), RRDataType("Str")}, str_add_str));
        env.funs["+"].push_back(RRFun({RRDataType("Str"), RRDataType("Int")}, str_add_int));
        env.funs["*"].push_back(RRFun({RRDataType("Int"), RRDataType("Int")}, int_multiply_int));
        env.funs["repeat"].push_back(RRFun({RRDataType("Str"), RRDataType("Int")}, str_repeat_int));
        env.funs["round"].push_back(RRFun({RRDataType("Float")}, round_float));
        env.funs["max"].push_back(RRFun({RRDataType("Int"), RRDataType("Int")}, max_int_int));
        // env.funs["print"].push_back(RRFun({RRDataType("Str")}, print_str));
        // env.funs["print"].push_back(RRFun({RRDataType("Int")}, print_int));
        env.funs["print"].push_back(RRFun({RRDataType("Any")}, print_any));
        //init op_order
        env.op_order["="] = OP_LOW_PRI; //both sides get evaluated first
        env.op_order["repeat"] = OP_LOW_PRI+2;
        env.op_order["+"] = OP_HIGH_PRI-5;
        env.op_order["*"] = OP_HIGH_PRI-4;
    }

    RRObj get_var_or_new(string& name) {
        return vars[name];
    }
    RRObj& get_var_or_new_mut(string& name) {
        return vars[name];
    }
    RRObj get_var(string& name) {
        if(vars.find(name) == vars.end()) {
            rr_runtime_error("Couldn't find a variable '"s + name + "'");
        }
        return vars[name];
    }
    RRObj& get_var_mut(string& name) {
        if(vars.find(name) == vars.end()) {
            rr_runtime_error("Couldn't find a variable '"s + name + "'");
        }
        return vars[name];
    }
    RRFun get_fun(string& name, vector<RRDataType> arg_types) {
        if(funs.find(name) != funs.end()) {
            for(RRFun f : funs[name]) {
                //check if `f.params` vector is equal to `arg_types` vector
                //and yes, it's important that function params are on the **right** (i know it's not a good practice)
                if(arg_types == f.params) {
                    return f;
                }
            }
        }
        //print an error
        string arg_str = "";
        for(int i = 0; i < arg_types.size()-1; i++) {
            arg_str += single_type_of(arg_types[i].type);
            arg_str += ",";
        }
        arg_str += single_type_of(arg_types[arg_types.size()-1].type);
        rr_runtime_error("Couldn't find a function '"s + name + "<" + arg_str + ">'");
        return RRFun();
    }
    RRObj assign_var(string& name, RRObj obj) {
        vars[name] = obj;
        return obj;
    }

    //check whether the function list contains this name
    bool is_fun(string& name) {
        return funs.find(name) != funs.end();
    }
    //if an operator order has been established for this name, it's an operator
    bool is_op(string& name) {
        return op_order.find(name) != op_order.end();
    }

    //return true if right operator has higher priority than left operator
    bool op_priority_higher(string& lop, string& rop) {
        return op_order[rop] > op_order[lop];
    }
};