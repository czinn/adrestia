#ifndef CGIPP_EXCEPTION_H
#define CGIPP_EXCEPTION_H

#include <stdexcept>

namespace cgipp
{

class cgipp_exception : public std::runtime_error
{
public:
    cgipp_exception (const string & msg)
        : runtime_error (msg)
    {}
};


class no_session : public cgipp_exception
{
public:
    no_session ()
        : cgipp_exception ("The received session ID does not correspond to an existing server session")
    {}
};


class invalid_session : public cgipp_exception
{
public:
    invalid_session ()
        : cgipp_exception ("Invalid session")
    {}
};


class session_expired : public cgipp_exception
{
public:
    session_expired ()
        : cgipp_exception ("Session expired")
    {}
};

class uninitialized_server_data : public cgipp_exception
{
public:
    uninitialized_server_data ()
        : cgipp_exception ("Attempt to read an uninitialized Server_data member (a valid session does not exist)")
    {}
};

}

#endif
