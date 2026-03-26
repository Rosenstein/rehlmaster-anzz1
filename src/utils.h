#pragma once

#ifndef _WIN32
#define stricmp strcasecmp
#endif

extern void reverse(char* s);
extern char* __itoa(int n, char* s);
extern char* trimbuf(char *str);
extern char* chrreplace(char *str, char a, char b);
extern unsigned long host2ip(const char *hostname);
extern void ServerPrintf(char *fmt, ...);
extern FILE* cfg_open(const char *fname, const char *fmode);
