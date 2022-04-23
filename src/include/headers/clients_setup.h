/**
 * @file clients_setup.h
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Header de librería con funciones de creación de clientes de
 *        distintos protocolos para el laboratorio #2 de Sistemas Operativos II.
 * @version 0.7.2
 * @since 2022-03-23
 */

#ifndef __SERVERS__
#define __SERVERS__

/* ---------- Librerías a utilizar -------------- */

#include "utilities.h"

#include <arpa/inet.h>
#include <net/if.h>

/* ---------- Definición de constantes ---------- */

#define _QUERY_ "SELECT * FROM PERSONS"

/* ---------- Definición de variables ----------- */

int socket_fd;

/* ---------- Prototipado de funciones ---------- */

void handler(int);
void receive_file(char *);
void run_ipv4_cl(char *, uint16_t);
void run_ipv6_cl(char *, char *, uint16_t, char *);
void run_local_cl(char *);

#endif