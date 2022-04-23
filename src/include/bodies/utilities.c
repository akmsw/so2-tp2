/**
 * @file utilities.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Librería con funciones útiles generales adaptada
 *        para el laboratorio #2 de Sistemas Operativos II.
 * @version 4.9.5
 * @since 2021-08-18
 */

#include "../headers/utilities.h"

/**
 * @brief Esta función muestra el mensaje solicitado en pantalla
 *        y termina la ejecución del programa si es necesario.
 *
 * @param pid ID del proceso que mostrará el error.
 * @param source Fuente del error.
 * @param protocol Protocolo de la fuente del error.
 * @param msg_type Tipo de mensaje.
 * @param details Mensaje a mostrar.
 */
void show_msg(int pid, char *source, char *protocol, int msg_type, char *details)
{
    try_write(((msg_type == _INFO_) ? STDOUT_FILENO : STDERR_FILENO), mk_msg(pid, source, protocol, msg_type, details));

    if (msg_type == _FATAL_ERR_)
        exit(EXIT_FAILURE);
}

/**
 * @brief Esta función muestra un mensaje de ayuda para
 *        iniciar tanto un cliente como un servidor.
 */
void show_examples()
{
    char *h_msg = "///////////////////////////////////////////////////////////////////   E X A M P L E S   //////////////////////////////////////////////////////////////////\n\n\
These examples are provided assuming the correct project compilation, standing in the project's root folder.\n\n\
<SERVER>\n\n\
    ./bin/srv my_socket 2222 5000\n\n\
<CLIENT>\n\n\
    ./bin/cln local my_socket\n\
    ./bin/cln ipv4 localhost 2222\n\
    ./bin/cln ipv4 [$HOSTNAME] 2222\n\
    ./bin/cln ipv4 127.0.0.1 2222\n\
    ./bin/cln ipv4 [IPv4 address] 2222\n\
    ./bin/cln ipv6 ::1 lo 5000\n\
    ./bin/cln ipv6 [IPv6 address] [interface] 5000\n\n\
For more help, run this program with '-h', '--help', or '?'.\n\n\
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n";

    try_write(STDOUT_FILENO, h_msg);
}

/**
 * @brief Esta función muestra un mensaje de ayuda acerca
 *        de los argumentos esperados para inicializar
 *        tanto el servidor como los clientes.
 */
void show_help()
{
    char *h_msg = "///////////////////////////////////////////////////////////////////////   H E L P   //////////////////////////////////////////////////////////////////////\n\n\
<SERVER>\n\
    In order to setup the server correctly, the user must provide the following arguments:\n\n\
        First argument:\n\
            Local TCP/IP connection socket file name.\n\
        Second argument:\n\
            TCP/IPv4 port number.\n\
        Third argument:\n\
            TCP/IPv6 port number.\n\n\
<CLIENT>\n\
    In order to setup the client correctly, the user must provide the following arguments:\n\n\
        First argument:\n\
            Client's connection protocol.\n\
                Valid protocols:\n\
                    local: The client will be connected to the server via local TCP/IP.\n\
                    ipv4: The client will be connected to the server via TCP/IPv4.\n\
                    ipv6: The client will be connected to the server via TCP/IPv6.\n\
        Second argument:\n\
            If the client is connected via local TCP/IP, the second argument must be the socket file name, and no more arguments are needed.\n\
            If the client is connected via TCP/IPvX, the second argument must be the server IPvX address.\n\
        Third argument:\n\
            If the client is connected via TCP/IPv4, the third argument must be the IPv4 port used for the connection, and no more arguments are needed.\n\
            If the client is connected via TCP/IPv6, the third argument must be the IPv6 interface.\n\
        If the client is connected via TCP/IPv6, the fourth argument must be:\n\
            The IPv6 port used for the connection, and no more arguments are needed.\n\n\
For execution examples, run this program with '-e', '--examples', or '!'.\n\n\
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n";

    try_write(STDOUT_FILENO, h_msg);
}

/**
 * @brief Esta función se encarga de intentar matar un proceso
 *        especificado mediante su ID. Si no se logra, se
 *        imprime un mensaje de error y se aborta el programa.
 *
 * @param pid ID del proceso a matar.
 * @param signal Señal a enviar al proceso.
 */
void try_kill(int pid, int signal)
{
    if (kill(pid, signal) == -1)
        show_msg(getpid(), _GENERAL_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to kill process");
}

/**
 * @brief Esta función se encarga de intentar escribir un
 *        mensaje en pantalla de manera thread-safe.
 *        Si no se logra, se imprime un mensaje de error
 *        y se aborta el programa.
 *
 * @param stdx Stream en el que se escribirá el mensaje.
 * @param msg Mensaje a mostrar.
 */
void try_write(int stdx, char *msg)
{
    if (write(stdx, msg, strlen(msg)) == -1)
    {
        fprintf(stderr, "\nFatal error on write instruction --- ABORTING\n");

        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Esta función se encarga de armar un mensaje personalizado.
 *
 * @details Se almacena memoria inicializada con ceros para evitar
 *          error con strcat. Se sobrescribe con el formato
 *          correspondiente para un mensaje de error o de información,
 *          y los detalles del mensaje, recibidos por parámetro.
 *
 * @param pid ID del proceso que mostrará el error.
 * @param source Fuente del error.
 * @param protocol Protocolo de la fuente del error.
 * @param msg_type Tipo de mensaje.
 * @param details Mensaje a mostrar.
 *
 * @return El mensaje de error formateado con el texto recibido por parámetro.
 */
char *mk_msg(int pid, char *source, char *protocol, int msg_type, char *details)
{
    char aux[10];

    char *msg;

    if (snprintf(aux, sizeof(aux), "%d", pid) < 0)
    {
        fprintf(stderr, "\nFatal snprintf error --- ABORTING\n");

        exit(EXIT_FAILURE);
    }

    if (msg_type == _INFO_)
        msg = (char *)calloc((strlen(source) + strlen(protocol) + strlen(details) + strlen(aux) + sizeof(NULL) + (sizeof(char) * 36)), sizeof(char));
    else if (msg_type == _FATAL_ERR_)
        msg = (char *)calloc((strlen(source) + strlen(protocol) + strlen(details) + strlen(aux) + sizeof(NULL) + (sizeof(char) * 43)), sizeof(char));
    else
        msg = (char *)calloc((strlen(source) + strlen(protocol) + strlen(details) + strlen(aux) + sizeof(NULL) + (sizeof(char) * 37)), sizeof(char));

    if (!msg)
        show_msg(getpid(), _GENERAL_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed in memory allocation");

    strcpy(msg, "[PID: ");

    strcat(msg, aux);
    strcat(msg, "] <");
    strcat(msg, source);
    strcat(msg, "@");
    strcat(msg, protocol);
    strcat(msg, "> [[ ");

    if (msg_type == _INFO_)
        strcat(msg, "INFO");
    else if (msg_type == _FATAL_ERR_)
        strcat(msg, "FATAL ERROR");
    else
        strcat(msg, "ERROR");

    strcat(msg, " ]] : ");
    strcat(msg, details);
    strcat(msg, "\n");

    return msg;
}

/**
 * @brief Esta función se encarga de quitar los
 *        espacios en los extremos de un string.
 *
 * @details Recorremos el string desde el comienzo hasta
 *          encontrar una letra. A partir de allí,
 *          recorremos el string desde el final hasta
 *          encontrar una letra. Antes de esta letra
 *          final, se coloca el caracter de fin de string.
 *          Si el string está completamente vacío, se lo
 *          retorna tal y como vino.
 *
 * @param str String a modificar.
 *
 * @return El string sin espacios ni tabulaciones
 *         ni al comienzo ni al final.
 */
char *strtrim(char *str)
{
    while ((*str == ' ') || (*str == '\t') || (*str == '\n'))
        str++;

    if (*str == 0)
        return str;

    char *end = str + strlen(str) - 1;

    while ((end > str) && ((*end == ' ') || (*end == '\t')))
        end--;

    end[1] = '\0';

    return str;
}