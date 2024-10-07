#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cmath>

#include "datatypes.h"
#include "tokenizer.h"
#include "rr_obj.h"
#include "environment.h"

/*
    add operators
    <type>_add_<type>
*/

// int/int `+` operator
RRObj int_add_int(vector<RRObj> args) {
    args[0].data_int += args[1].data_int;
    return args[0];
}

// float/float `+` operator
RRObj float_add_float(vector<RRObj> args) {
    args[0].data_float += args[1].data_float;
    return args[0];
}

// float/int `+` operator
RRObj float_add_int(vector<RRObj> args) {
    args[0].data_float += (double) args[1].data_int;
    return args[0];
}

// float/int `+` operator
RRObj int_add_float(vector<RRObj> args) {
    args[1].data_float += (double) args[0].data_int;
    return args[1];
}

// str/str `+` operator
RRObj str_add_str(vector<RRObj> args) {
    RRObj new_str = RRObj(RRDataType("Str"));
    new_str.data_str = new string(*(args[0].data_str) + *(args[1].data_str));
    return new_str;
}

// str/int `+` operator
RRObj str_add_int(vector<RRObj> args) {
    RRObj new_str = RRObj(RRDataType("Str"));
    new_str.data_str = new string(*(args[0].data_str) + to_string(args[1].data_int));
    return new_str;
}

// int/int `*` operator
RRObj int_multiply_int(vector<RRObj> args) {
    args[0].data_int *= args[1].data_int;
    return args[0];
}

// str/int `repeat` operator
RRObj str_repeat_int(vector<RRObj> args) {
    RRObj new_str = RRObj(RRDataType("Str"));
    new_str.data_str = new string();
    for(int i = 0; i < args[1].data_int; i++)
        *(new_str.data_str) += *(args[0].data_str);
    return new_str;
}

// float `round` operator
RRObj round_float(vector<RRObj> args) {
    args[0].data_int = llround(args[0].data_float);
    args[0].type = RRDataType("Int");
    return args[0];
}

// int/int `max` function
RRObj max_int_int(vector<RRObj> args) {
    args[0].data_int = max(args[0].data_int, args[1].data_int);
    return args[0];
}

// int/int `==` operator
RRObj int_eq_int(vector<RRObj> args) {
    RRObj comp_res = RRObj(RRDataType("Bool"));
    comp_res.data_bool = args[0].data_int == args[1].data_int;
    return comp_res;
}

// any `print` function
RRObj print_any(vector<RRObj> args) {
    cout << args[0] << endl;
    return args[0];
}

RRObj concat_list_str(vector<RRObj> args) {
    vector<RRObj>* vec = args[0].data_list;
    string glue = *args[1].data_str;
    string* str = new string();
    //all but last element - with glue
    for(int i = 0; i < vec->size()-1; i++) {
        RRObj obj = (*vec)[i];
        if(obj.type == RRDataType("Str"))
            (*str) += *obj.data_str;
        else if(obj.type == RRDataType("Int"))
            (*str) += to_string(obj.data_int);
        //ignore non-str/int because it's not even final, i don't really care
        (*str) += glue;
    }
    //last list element - without glue
    RRObj obj = (*vec)[vec->size()-1];
    if(obj.type == RRDataType("Str"))
        (*str) += *obj.data_str;
    else if(obj.type == RRDataType("Int"))
        (*str) += to_string(obj.data_int);
    RRObj to_return(RRDataType("Str"));
    to_return.data_str = str;
    return to_return;
}