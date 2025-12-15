#ifndef IO_H
#define IO_H

#include <stdio.h>

#define MSH_BUFSIZE 1024
#define MSH_PROMPTSIZE 1024

/*
**  ANSI Color codes for terminal output formatting:
**  RST - Reset to default color
**  K - Black
**  R - Red
**  Y - Yellow
**  B - Blue
**  M - Magenta
**  C - Cyan
**  W - White
**  Prefix B - Bright colors
**  Prefix BG - Background colors
*/

#define RST   "\033[0m" 

#define K   "\033[30m"
#define R   "\033[31m"
#define G   "\033[32m"
#define Y   "\033[33m"
#define B   "\033[34m"
#define M   "\033[35m"
#define C   "\033[36m"
#define W   "\033[37m"

#define BK  "\033[90m"
#define BR  "\033[91m"
#define BG  "\033[92m"
#define BY  "\033[93m"
#define BB  "\033[94m"
#define BM  "\033[95m"
#define BC  "\033[96m"
#define BW  "\033[97m"

#define BGK    "\033[40m"
#define BGR    "\033[41m"
#define BGG    "\033[42m"
#define BGY    "\033[43m"
#define BGB    "\033[44m"
#define BGM    "\033[45m"
#define BGC    "\033[46m"
#define BGW    "\033[47m"

#define MSH_perror(msg, ...) printf(R msg RST, ##__VA_ARGS__)

char *MSH_read_line(void);

#endif
