#include "ex_rehlds_api.h"

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

void reverse(char* s)
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

char* __itoa(int n, char* s)
{
	int i, sign;

	if (n == -2147483647-1) { // INT_MIN
		s[0] = '-'; s[1] = '2'; s[2] = '1'; s[3] = '4'; s[4] = '7';
		s[5] = '4'; s[6] = '8'; s[7] = '3'; s[8] = '6'; s[9] = '4';
		s[10] = '8'; s[11] = 0;
		return s;
	}

	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = 0;
	reverse(s);
	return s;
}

char* trimbuf(char *str) {
	char *ibuf;

	if (!str) return nullptr;
	for (ibuf = str; *ibuf && (byte)(*ibuf) < (byte)0x80 && isspace(*ibuf); ++ibuf)
		;

	int i = strlen(ibuf);
	if (str != ibuf)
		memmove(str, ibuf, i);

	while (--i >= 0) {
		if (!isspace(str[i]))
			break;
	}
	str[i + 1] = '\0';
	return str;
}

char* chrreplace(char *str, char a, char b) {
	for (char *p = str; *p != 0; p++) {
		if(*p == a) *p = b;
	}
	return str;
}

unsigned long host2ip(const char *hostname) {
	struct addrinfo hints, *info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(hostname, 0, &hints, &info)) return 0;
	unsigned long ip = *(unsigned long*)(info->ai_addr->sa_data+2);
	freeaddrinfo(info);
	return ip;
}

void ServerPrintf(char *fmt, ...) {
	va_list args;
	char buffer[512];

	va_start(args, fmt);
	int i = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	if (i <= 0) return;

	buffer[(i < 511) ? i : 511] = 0;

	// Print to server console
	SERVER_PRINT(buffer);
}

FILE* cfg_open(const char *fname, const char *fmode) {
	char path[MAX_PATH];
	const char* ppath = gpMetaUtilFuncs->pfnGetPluginPath(PLID);

	if (ppath) {
		strncpy(path, ppath, sizeof(path) - 1);
		path[sizeof(path) - 1] = 0;

		char* s = strrchr(path, '/');
		if (s) {
			size_t maxlen = sizeof(path) - 1 - (s + 1 - path);
			strncpy(s + 1, fname, maxlen);
			path[sizeof(path) - 1] = 0;

			FILE *fl = fopen(path, fmode);
			if (fl) return fl;
		}
	}

	char gamedir[MAX_PATH];
	g_engfuncs.pfnGetGameDir(gamedir);
	snprintf(path, sizeof(path), "./%s/%s", gamedir, fname);
	FILE *fl = fopen(path, fmode);
	if (fl) return fl;

	snprintf(path, sizeof(path), "./%s", fname);
	return fopen(path, fmode);
}
