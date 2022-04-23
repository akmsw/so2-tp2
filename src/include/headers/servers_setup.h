/**
 * @file servers_setup.h
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Header de librería con funciones de creación de servidores de
 *        distintos protocolos para el laboratorio #2 de Sistemas Operativos II.
 * @version 0.9.5
 * @since 2022-03-23
 */

#ifndef __SERVERS__
#define __SERVERS__

/* ---------- Librerías a utilizar -------------- */

#include "sqlite3.h"
#include "utilities.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>

/* ---------- Definición de constantes ---------- */

#define _SV_PARAMS_ 4

#define _DB_NAME_ "persons.db"
#define _DB_CONNECTIONS_ 5

/* ----------- Definición de variables --------- */

char buffer[_MAX_BUFF_SIZE_];
char query_result[_MAX_BUFF_SIZE_];

/* ---------- Prototipado de funciones ---------- */

void create_tables_db(sqlite3 *);
void log_query(const char *, sqlite3 *);
void populate_db(sqlite3 *);
void send_file(int);
void startup_db(const char *);
void startup_ipv4_sv(uint16_t, sqlite3 **);
void startup_ipv6_sv(uint16_t, sqlite3 **);
void startup_local_sv(char *, sqlite3 **);

#endif