/*
**  version.c -- Version Information for xaric (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _VERSION_C_AS_HEADER_

#ifndef _VERSION_C_
#define _VERSION_C_

#define XVERSION 0x00C200

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} xversion_t;

extern xversion_t xversion;

#endif /* _VERSION_C_ */

#else /* _VERSION_C_AS_HEADER_ */

#define _VERSION_C_AS_HEADER_
#include "version.c"
#undef  _VERSION_C_AS_HEADER_

xversion_t xversion = {
    0x00C200,
    "0.12.0",
    "0.12.0 (20-Jun-2001)",
    "This is xaric, Version 0.12.0 (20-Jun-2001)",
    "xaric 0.12.0 (20-Jun-2001)",
    "xaric/0.12.0",
    "@(#)xaric 0.12.0 (20-Jun-2001)",
    "$Id$"
};

#endif /* _VERSION_C_AS_HEADER_ */

