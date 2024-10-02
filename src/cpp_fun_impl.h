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

// int/int `+` operator
RRObj int_add_int(vector<RRObj> args) {
    args[0].data_int += args[1].data_int;
    return args[0];
}

// int/int `*` operator
RRObj int_multiply_int(vector<RRObj> args) {
    args[0].data_int *= args[1].data_int;
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