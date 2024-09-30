#pragma once

#include <string>
#include <iostream>

using namespace std;

void runtime_error(string error_message) {
    cout << "--RR: Runtime error: " << error_message << endl << "Aborting" << endl;
    exit(1);
}

void parse_error(string error_message) {
    cout << "--RR: Error while parsing: " << error_message << endl << "Aborting" << endl;
    exit(1);
}

void warning(string warning_message) {
    cout << "--RR: Warning: " << warning_message << endl;
}