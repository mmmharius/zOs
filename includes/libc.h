#ifndef LIBC_H
#define LIBC_H
#include <stdint.h>

int    ft_strcmp(const char *s1, const char *s2);
int    ft_strlen(const char *s);
char  *ft_strcpy(char *dst, const char *src);

void  *ft_memset(void *b, int c, size_t len);
void   ft_bzero(void *s, size_t n);
void  *ft_memcpy(void *dst, const void *src, size_t n);
void  *ft_memmove(void *dst, const void *src, size_t n);
int    ft_atoi(const char *str);
char  *ft_strchr(const char *s, int c);
char  *ft_strrchr(const char *s, int c);
int    ft_strncmp(const char *s1, const char *s2, size_t n);
size_t ft_strlcpy(char *dst, const char *src, size_t dstsize);
size_t ft_strlcat(char *dst, const char *src, size_t dstsize);
int    ft_isalpha(int c);
int    ft_isdigit(int c);
int    ft_isalnum(int c);
int    ft_toupper(int c);
int    ft_tolower(int c);
void   ft_putbuff(char *buffer, int row, int col, const char *str);

#endif