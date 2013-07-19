/ _merlin_log1p.s (emx+gcc) -- Copyright (c) 1994 by Klaus Gebhardt

#define _LIBM_H
#include <errno.h>

#define _xam fxam; fstsw %ax; andb $0x45, %ah

#define j_nan cmpb $0x01, %ah; je
#define j_inf cmpb $0x05, %ah; je

#if defined (LONG_DOUBLE)
#define FLD fldt
#define FUN(X) __##X##l
#define DEF(X) __##X##l:
#define CONV(X)
#else
#define FLD fldl
#define FUN(X) _##X
#define DEF(X) _##X##:
#define CONV(X) fstpl X;fldl X
#endif

#if !defined (__MT__)

#define SETERRNO(x) \
        movl    x, _errno

#else

#define SETERRNO(x) \
        call    __errno_fun ;\
        movl    x, (%eax)

#endif

	.globl  FUN(_merlin_log1p)

        .text

        .align  2, 0x90

/ double _merlin_log1p (double x)

#define x       4(%esp)

DEF(_merlin_log1p)
        fldl2e
        fdivrl  _merlin_const_ONE
        FLD     x
	fabs
	fcoml	_merlin_const_ONE
	fnstsw	%ax
	sahf
	fstp	%st(0)
	FLD	x
	jc	ss
	faddl	_merlin_const_ONE
	fyl2x
        _xam
        j_nan   1f
        j_inf   1f
	ret
ss:
	fyl2xp1
        _xam
        j_nan   1f
        j_inf   1f
        ret
	
        .align  2, 0x90
1:      SETERRNO($EDOM)
        ret
