/* pc.h - terminal I/O for GNU info for OS/2
   and other PC support functions
   Kai Uwe Rommel, 1990-1993
 */

extern void pc_init (void);
extern void pc_exit (void);
extern int pc_getc (int wait);

#ifdef getc
#undef getc
#endif
#define getc(s)      pc_getc(1)
