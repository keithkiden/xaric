/*
**  version.c -- Version Information for xaric (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _VERSION_C_AS_HEADER_

#ifndef _VERSION_C_
#define _VERSION_C_

#define XVERSION 0x00B202

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
    0x00B202,
    "0.11.2",
    "0.11.2 (13-Nov-2000)",
    "This is xaric, Version 0.11.2 (13-Nov-2000)",
    "xaric 0.11.2 (13-Nov-2000)",
    "xaric/0.11.2",
    "@(#)xaric 0.11.2 (13-Nov-2000)",
    "$Id$"
};

#endif /* _VERSION_C_AS_HEADER_ */
