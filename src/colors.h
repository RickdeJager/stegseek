
/*
 Originally written by @gon1332

 https://github.com/gon1332/fort320/blob/master/include/Utils/colors.h

*/

#ifndef _COLORS_
#define _COLORS_

#include "Arguments.h"

/* FOREGROUND */
#define RST "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

// Checked Color functions. Only print colors if they're enabled.
// Used to prevent ugly escape codes in stream redirection
#define CRED(x) (Args.Color.getValue() ? FRED(x) : x)
#define CGRN(x) (Args.Color.getValue() ? FGRN(x) : x)
#define CYEL(x) (Args.Color.getValue() ? FYEL(x) : x)
#define CBLU(x) (Args.Color.getValue() ? FBLU(x) : x)
#define CMAG(x) (Args.Color.getValue() ? FMAG(x) : x)
#define CCYN(x) (Args.Color.getValue() ? FCYN(x) : x)
#define CWHT(x) (Args.Color.getValue() ? FWHT(x) : x)

#endif /* _COLORS_ */