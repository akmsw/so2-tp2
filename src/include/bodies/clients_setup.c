/**
 * @file clients_setup.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Librería con funciones de creación de clientes de distintos
 *        protocolos para el laboratorio #2 de Sistemas Operativos II.
 * @version 1.1.1
 * @since 2022-03-23
 */

#include "../headers/clients_setup.h"

/**
 * @brief Esta función se encarga de recibir del socket abierto
 *        el archivo de base de datos utilizado por el servidor,
 *        almacenando una copia local.
 *
 * @param file_name Nombre con el que se guardará el archivo
 *                  a recibir (debe incluir la extensión '.db').
 */
void receive_file(char *file_name)
{
    FILE *f = NULL;

    if (!(f = fopen(file_name, "wb")))
        show_msg(getpid(), _CLIENT_SRC_, _IPV6_, _FATAL_ERR_, "Failed creating file for transfer");

    int bytes_received = 0;

    char recv_buff[_DATA_CHUNK_SIZE_];

    while ((bytes_received = (int)read(socket_fd, recv_buff, _DATA_CHUNK_SIZE_)) > 0)
    {
        fwrite(recv_buff, 1, (size_t)bytes_received, f);

        if (bytes_received < _DATA_CHUNK_SIZE_)
            break;
    }

    if (bytes_received == -1)
        show_msg(getpid(), _CLIENT_SRC_, _IPV6_, _FATAL_ERR_, "Failed reading file on transfer");
}

/**
 * @brief Creación y ejecución de cliente con conexión
 *        TCP/IPv4 (cliente tipo 'B').
 *
 * @details Se realiza la conexión TCP/IPv4 y se espera
 *          que el usuario ingrese una consulta SQL.
 *          La misma será enviada al servidor, el cual
 *          se encargará de validarla y realizarla.
 *          Si la query es válida y se realiza de forma
 *          exitosa, el cliente recibirá el resultado
 *          listo para imprimirlo en pantalla.
 *
 * @param address Dirección del host a conectarse.
 * @param port Puerto del host a conectarse.
 */
void run_ipv4_cl(char *address, uint16_t port)
{
    struct sockaddr_in struct_sv;

    struct hostent *server;

    // Validación de dirección IPv4
    if (!(server = gethostbyname(address)))
        show_msg(getpid(), _CLIENT_SRC_, _IPV4_, _FATAL_ERR_, "Failed resolving IPv4 address");

    // Creación del socket para el cliente
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        show_msg(getpid(), _CLIENT_SRC_, _IPV4_, _FATAL_ERR_, "Failed in socket creation");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));
    memset(&struct_sv.sin_zero, '\0', 8);

    bcopy((char *)server->h_addr_list[0], (char *)&struct_sv.sin_addr.s_addr, (size_t)server->h_length);

    struct_sv.sin_family = AF_INET;
    struct_sv.sin_port = (in_port_t)htons(port);

    // Conexión cliente-servidor
    if (connect(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_msg(getpid(), _CLIENT_SRC_, _IPV4_, _FATAL_ERR_, "Failed connecting to socket");

    char buffer[_MAX_BUFF_SIZE_];

    while (1)
    {
        memset(buffer, 0, _MAX_BUFF_SIZE_);

        char info_msg[70];

        sprintf(info_msg, "[PID: %d] <CLIENT@IPv4> [[ INFO ]] : Enter SQL query > ", getpid());

        try_write(STDOUT_FILENO, info_msg);

        fgets(buffer, _MAX_BUFF_SIZE_, stdin);

        buffer[strlen(buffer) - 1] = '\0';

        if (send(socket_fd, buffer, strlen(buffer), 0) == -1)
            show_msg(getpid(), _CLIENT_SRC_, _IPV4_, _FATAL_ERR_, "Failed sending message");

        memset(buffer, 0, _MAX_BUFF_SIZE_);

        if (read(socket_fd, buffer, _MAX_BUFF_SIZE_) == -1)
            show_msg(getpid(), _CLIENT_SRC_, _IPV4_, _FATAL_ERR_, "Failed receiving server's response");

        try_write(STDOUT_FILENO, "\n>>>>>>>>>> QUERY RESULT <<<<<<<<<<\n\n");
        try_write(STDOUT_FILENO, buffer);
    }
}

/**
 * @brief Creación y ejecución de cliente con conexión
 *        TCP/IPv6 (cliente tipo 'C').
 *
 * @details Se realiza la conexión TCP/IPv6 y se descarga el
 *          archivo de la base de datos recibido mediante el
 *          socket.
 *
 * @param address Dirección del host a conectarse.
 * @param interface Interfaz del host a conectarse.
 * @param port Puerto del host a conectarse.
 * @param file_name Nombre con el que se guardará el archivo a
 *                  recibir (debe incluir la extensión '.db').
 */
void run_ipv6_cl(char *address, char *interface, uint16_t port, char *file_name)
{
    struct sockaddr_in6 struct_sv;

    // Creación del socket para el cliente
    if ((socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
        show_msg(getpid(), _CLIENT_SRC_, _IPV6_, _FATAL_ERR_, "Failed in socket creation.");

    // Inicialización de la estructura del servidor
    struct_sv.sin6_family = AF_INET6;
    struct_sv.sin6_port = (in_port_t)htons(port);
    struct_sv.sin6_scope_id = if_nametoindex(interface);

    // Validación de dirección IPv6
    if (inet_pton(AF_INET6, address, &struct_sv.sin6_addr) != 1)
        show_msg(getpid(), _CLIENT_SRC_, _IPV6_, _FATAL_ERR_, "Failed resolving IPv6 address.");

    // Conexión cliente-servidor
    if (connect(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_msg(getpid(), _CLIENT_SRC_, _IPV6_, _FATAL_ERR_, "Failed connecting socket.");

    receive_file(file_name);

    close(socket_fd);

    exit(EXIT_SUCCESS);
}

/**
 * @brief Creación y ejecución de cliente con conexión
 *        TCP local (cliente tipo 'A').
 *
 * @details Se realiza la conexión TCP local y se envía de manera
 *          constante la misma query para testear la estabilidad
 *          del servidor.
 *
 * @param socket_filename Nombre del archivo local a usar como
 *                        socket entre servidor y cliente.
 */
void run_local_cl(char *socket_filename)
{
    socklen_t sv_len;

    struct sockaddr_un struct_sv;

    // Creación del socket para el cliente
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        show_msg(getpid(), _CLIENT_SRC_, _LOCAL_, _FATAL_ERR_, "Failed in socket creation");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));

    struct_sv.sun_family = AF_UNIX;

    strcpy(struct_sv.sun_path, socket_filename);

    sv_len = (socklen_t)(strlen(struct_sv.sun_path) + sizeof(struct_sv.sun_family));

    // Conexión cliente-servidor
    if (connect(socket_fd, (struct sockaddr *)&struct_sv, sv_len) == -1)
        show_msg(getpid(), _CLIENT_SRC_, _LOCAL_, _FATAL_ERR_, "Failed connecting socket");

    char buffer[_MAX_BUFF_SIZE_];

    while (1)
    {
        if (send(socket_fd, _QUERY_, strlen(_QUERY_), 0) == -1)
            show_msg(getpid(), _CLIENT_SRC_, _LOCAL_, _FATAL_ERR_, "Failed sending message");

        memset(buffer, 0, _MAX_BUFF_SIZE_);

        if (read(socket_fd, buffer, _MAX_BUFF_SIZE_) == -1)
            show_msg(getpid(), _CLIENT_SRC_, _LOCAL_, _FATAL_ERR_, "Failed receiving server's response");

        try_write(STDOUT_FILENO, "\n>>>>>>>>>> QUERY RESULT <<<<<<<<<<\n");
        try_write(STDOUT_FILENO, buffer);
    }
}