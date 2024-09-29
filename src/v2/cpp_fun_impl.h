#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

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