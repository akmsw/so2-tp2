/**
 * @file utilities.h
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Header de librería con funciones útiles generales
 *        adaptada para el laboratorio #2 de Sistemas Operativos II.
 * @version 5.2
 * @since 2021-08-18
 */

#ifndef __UTILITIES__
#define __UTILITIES__

/* ---------- Librerías a utilizar -------------- */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

/* ---------- Definición de constantes ---------- */

#define _NORM_ERR_ 0
#define _FATAL_ERR_ 1
#define _INFO_ 2

#define _CLIENT_SRC_ "CLIENT"
#define _SERVER_SRC_ "SERVER"
#define _GENERAL_SRC_ "GENERAL"

#define _EOT_MSG_ "STOP" // Mensaje de end-of-transmission
#define _IPV4_ "IPv4"
#define _IPV6_ "IPv6"
#define _LOCAL_ "LOCAL"

#define _MAX_BUFF_SIZE_ 10000
#define _DATA_CHUNK_SIZE_ 1024

/* ---------- Prototipado de funciones ---------- */

void show_examples(void);
void show_help(void);
void show_msg(int, char *, char *, int, char *);
void try_kill(int, int);
void try_write(int, char *);

char *mk_msg(int, char *, char *, int, char *);
char *strtrim(char *);

#endif