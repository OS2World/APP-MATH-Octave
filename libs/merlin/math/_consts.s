/ consts.s (emx+gcc) -- Copyright (c) 2001 Klaus Gebhardt

        .globl  _merlin_const_NAN
        .globl  _merlin_const_ONE
        .globl  _merlin_const_ZERO
        .globl  _merlin_const_TWO
        .globl  _merlin_const_HALF
        .globl  _merlin_const_M_ONE

_merlin_const_NAN:    .long   0xffffffff, 0x7fffffff
_merlin_const_ONE:    .long   0x00000000, 0x3ff00000
_merlin_const_ZERO:   .long   0x00000000, 0x00000000
_merlin_const_TWO:    .long   0x00000000, 0x40000000
_merlin_const_HALF:   .long   0x00000000, 0x3fe00000
_merlin_const_M_ONE:  .long   0x00000000, 0xbff00000
