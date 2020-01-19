#pragma once

#include <exception>

class bad_trace_index : public std::exception {
public:
    bad_trace_index() {}

    virtual ~bad_trace_index() {}
    virtual const char* what() {
        return "";
    }
};

class end_of_file : public std::exception {
public:
    end_of_file() {}

    virtual ~end_of_file() {}
    virtual const char* what() {
        return "";
    }
};