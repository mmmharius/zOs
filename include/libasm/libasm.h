#ifndef LIBASM_H
#define LIBASM_H

#include <stddef.h>
#include <sys/types.h>

size_t  ft_strlen(const char *s);
ssize_t ft_write(int fd, const char *buf, size_t nbyte); // dont use call errno
ssize_t ft_read(int fd, void *buf, size_t count);
char    *ft_strcpy(char *dst, const char *src);
int     ft_strcmp(const char *s1, const char *s2);
char    *ft_strdup(const char *s); // dont use until own malloc

#endif
