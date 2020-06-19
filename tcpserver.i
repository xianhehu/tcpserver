/* File: tcpserver.i */
%include <typemaps.i>
%apply int *OUTPUT { int *fd };
/*%apply char *INOUT { char *buf };*/

%module tcpserver

%{
#include "../tcpserver/include/tcpserver.h"
%}

%include "./include/tcpserver.h"
