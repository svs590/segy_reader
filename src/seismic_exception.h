#pragma once

#include <exception>

#ifdef _MSC_VER
#define __WINDOWS_FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAME__ (std::strrchr(__WINDOWS_FILENAME__, '/') ? std::strrchr(__WINDOWS_FILENAME__, '/') + 1 : __WINDOWS_FILENAME__)
#else
#define __FILENAME__ (std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define SR_THROW( ex, message ) {                                       \
        std::ostringstream log;                                         \
        log << __FILENAME__ << ":"                                      \
            << __LINE__ << ": "                                         \
            << message;                                                 \
        throw ex(log.str().c_str());                                    \
}

class bad_trace_index : public std::exception {
public:
    bad_trace_index() {}

    virtual ~bad_trace_index() {}
    virtual const char* what() {
        return "bad trace index";
    }
};

class end_of_file : public std::exception {
public:
    end_of_file() {}

    virtual ~end_of_file() {}
    virtual const char* what() {
        return "end of file";
    }
};