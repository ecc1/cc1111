#ifndef _SET_STDIO_H
#define _SET_STDIO_H

typedef char (*get_t)(void);
typedef void (*put_t)(char);

void set_stdio(get_t, put_t);

#endif /* _SET_STDIO_H */
