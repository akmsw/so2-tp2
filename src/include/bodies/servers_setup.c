/**
 * @file servers_setup.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Librería con funciones de creación de servidores de distintos
 *        protocolos para el laboratorio #2 de Sistemas Operativos II.
 * @version 1.5.1
 * @since 2022-03-23
 */

#include "../headers/servers_setup.h"

/**
 * @brief Handler para las queries de los clientes.
 *
 * @details Recorremos la tabla consultada y armamos el string 'aux' por cada
 *          registro. Luego, concatenamos 'aux' al string que contiene el
 *          resultado final completo de la query, que será enviado al cliente.
 *
 * @param ptr No se utiliza.
 * @param columnsAmount Cantidad de columnas de la tabla consultada.
 * @param cellValues Valores de las celdas de la tabla consultada.
 * @param columnNames Nombres de las columnas de la tabla consultada.
 *
 * @return 0 Si la query se realizó con éxito. De lo contrario, se
 *           aborta el programa.
 */
int callback(void *ptr, int columnsAmount, char **cellValues, char **columnNames)
{
    (void)ptr;

    char aux[_MAX_BUFF_SIZE_];

    for (int i = 0; i < columnsAmount; i++)
    {
        if (sprintf(aux, "%s: %s\n", columnNames[i], cellValues[i]) < 0)
            show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed making query result string");

        strcat(query_result, aux);

        memset(aux, 0, _MAX_BUFF_SIZE_);
    }

    strcat(query_result, "\n");

    return 0;
}

/**
 * @brief Se crean las tablas necesarias en la base
 *        de datos.
 *
 * @param db Base de datos a modificar.
 */
void create_tables_db(sqlite3 *db)
{
    char *create_person_table = "CREATE TABLE PERSONS ("
                                "ID INTEGER PRIMARY KEY NOT NULL, "
                                "NAME TEXT NOT NULL, "
                                "SURNAME TEXT NOT NULL, "
                                "AGE INT NOT NULL);";

    if (sqlite3_exec(db, create_person_table, callback, NULL, NULL) != SQLITE_OK)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to create tables into database");

    char *create_queries_table = "CREATE TABLE QUERIES ("
                                 "QUERY TEXT NOT NULL, "
                                 "DATE TEXT NOT NULL);";

    if (sqlite3_exec(db, create_queries_table, callback, NULL, NULL) != SQLITE_OK)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to create tables into database");
}

/**
 * @brief Se registra en la tabla correspondiente la query recibida
 *        por el usuario con la fecha y hora en la que fue realizada.
 *
 * @param protocol Protocolo utilizado por el servidor.
 * @param db Base de datos a modificar.
 */
void log_query(const char *protocol, sqlite3 *db)
{
    time_t current_time;

    char *current_time_str;

    char query[_MAX_BUFF_SIZE_];

    if ((current_time = time(NULL)) == ((time_t)-1))
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed getting current time for query log");

    if ((current_time_str = ctime(&current_time)) == NULL)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed converting current time to string for query log");

    current_time_str[strlen(current_time_str) - 1] = '\0';

    if (sprintf(query, "INSERT INTO QUERIES VALUES (\"") < 0)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed making query for query log");

    if (strcmp(protocol, _IPV6_) == 0)
        strcat(query, "Transferred database file to IPv6 client");
    else
        strcat(query, buffer);

    strcat(query, "\", \"");
    strcat(query, current_time_str);
    strcat(query, "\");");

    if (sqlite3_exec(db, query, callback, NULL, NULL) != SQLITE_OK)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _NORM_ERR_, "Failed trying to execute query for query log");
}

/**
 * @brief Se llena la base de datos con información para las consultas.
 *
 * @param db Base de datos a modificar.
 */
void populate_db(sqlite3 *db)
{
    char *query = "INSERT INTO PERSONS VALUES (1,  'FRANCISCO', 'BONINO',       23);"
                  "INSERT INTO PERSONS VALUES (2,  'LIHUÉ',     'LUNA',         25);"
                  "INSERT INTO PERSONS VALUES (3,  'MATEO',     'MERINO',       22);"
                  "INSERT INTO PERSONS VALUES (4,  'AXEL',      'COVACICH',     24);"
                  "INSERT INTO PERSONS VALUES (5,  'SOFÍA',     'AMALLO',       23);"
                  "INSERT INTO PERSONS VALUES (6,  'FRANCISCO', 'CIORDIA',      23);"
                  "INSERT INTO PERSONS VALUES (7,  'FEDERICO',  'CORONATI',     24);"
                  "INSERT INTO PERSONS VALUES (8,  'ANGELINA',  'CUELLO',       23);"
                  "INSERT INTO PERSONS VALUES (9,  'LUCAS',     'BOSSIO',       24);"
                  "INSERT INTO PERSONS VALUES (10, 'LEONEL',    'CESANO',       24);"
                  "INSERT INTO PERSONS VALUES (11, 'FELIX',     'GORDILLO',     24);"
                  "INSERT INTO PERSONS VALUES (12, 'AGUSTÍN',   'GUTIÉRREZ',    23);"
                  "INSERT INTO PERSONS VALUES (13, 'JULIÁN',    'LEIGUARDA',    25);"
                  "INSERT INTO PERSONS VALUES (14, 'ROCÍO',     'RODRÍGUEZ',    24);"
                  "INSERT INTO PERSONS VALUES (15, 'LARA',      'BAZÁN',        24);";

    if (sqlite3_exec(db, query, callback, NULL, NULL) != SQLITE_OK)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to populate database");
}

/**
 * @brief Esta función envía el archivo de base
 *        de datos al cliente conectado.
 *
 * @details Leemos el archivo en porciones de tamaño
 *          _DATA_CHUNK_SIZE_ bytes, y las enviamos.
 *          Cuando el envío es menor a _DATA_CHUNK_SIZE_
 *          bytes, chequeamos si efectivamente llegamos
 *          al EOF o si hubo un error.
 *
 * @param fd File descriptor para comunicación.
 */
void send_file(int fd)
{
    FILE *f = NULL;

    if (!(f = fopen(_DB_NAME_, "rb")))
        show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed opening file for transfer");

    while (1)
    {
        unsigned char chunk[_DATA_CHUNK_SIZE_] = {0};

        int bytes_read = (int)fread(chunk, 1, (size_t)_DATA_CHUNK_SIZE_, f);

        if (bytes_read > 0)
            write(fd, chunk, (size_t)bytes_read);

        if (bytes_read < _DATA_CHUNK_SIZE_)
        {
            if (feof(f))
                return;

            if (ferror(f))
                show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed reading file on transfer");
        }
    }
}

/**
 * @brief Se crea y llena una base de datos.
 *
 * @param dbname Nombre de la base de datos a levantar.
 */
void startup_db(const char *dbname)
{
    sqlite3 *db;

    if (sqlite3_open(dbname, &db) != SQLITE_OK)
    {
        if (sqlite3_close(db) != SQLITE_OK)
            show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to close database");

        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to open database");
    }

    create_tables_db(db);

    populate_db(db);

    if (sqlite3_close(db) != SQLITE_OK)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to close database");
}

/**
 * @brief Se inicializa la conexión TCP/IPv4.
 *
 * @details Se inicia la conexión TCP/IPv4 y por cada
 *          cliente que se conecte al servidor mediante
 *          este protocolo, se crea un proceso hijo para
 *          recibir las queries que el cliente envíe.
 *          Cuando un cliente TCP/IPv4 envía una query,
 *          se realiza la consulta contra la base de datos
 *          y se le envía al cliente el resultado de la misma.
 *          Además, se registra la query recibida en la tabla
 *          de log de queries.
 *
 * @param port Número de puerto a utilizar para la conexión.
 * @param db_connections Arreglo de conexiones a la base de datos.
 */
void startup_ipv4_sv(uint16_t port, sqlite3 **db_connections)
{
    struct sockaddr_in struct_sv;
    struct sockaddr_in struct_cl;

    socklen_t client_len = sizeof(struct_cl);

    int socket_fd;

    // Creación del socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed in socket creation");

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed trying to set port as reusable");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));

    struct_sv.sin_family = AF_INET;
    struct_sv.sin_addr.s_addr = INADDR_ANY;
    struct_sv.sin_port = htons(port);

    // Binding del socket del server
    if (bind(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed binding socket");

    if (listen(socket_fd, 5) == -1) // Máximo 5 clientes en espera simultánea
        show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed trying to listen to socket");

    char info_msg[30];

    if (sprintf(info_msg, "Available port: %d", htons(struct_sv.sin_port)) < 0)
        show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed making info message");

    show_msg(getpid(), _SERVER_SRC_, _IPV4_, _INFO_, info_msg);

    while (1)
    {
        // Se esperan conexiones
        int cl_socket_fd = accept(socket_fd, (struct sockaddr *)&struct_cl, &client_len);

        if (cl_socket_fd == -1)
            show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed trying to accept client");

        // Por cada conexión, se crea un proceso hijo que reciba los mensajes
        int cp_local_pid = fork();

        if (cp_local_pid == -1)
            show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed on process forking for client handling");

        if (cp_local_pid == 0)
        {
            // Proceso hijo
            close(socket_fd);

            while (1)
            {
                memset(buffer, 0, _MAX_BUFF_SIZE_);

                if (read(cl_socket_fd, buffer, (_MAX_BUFF_SIZE_ - 1)) == -1)
                    show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed receiving message");

                if (strstr(buffer, _EOT_MSG_) != NULL)
                {
                    close(cl_socket_fd);

                    show_msg(getpid(), _SERVER_SRC_, _IPV4_, _INFO_, "Client dropped connection. Finishing current subprocess");

                    exit(EXIT_SUCCESS);
                }

                sqlite3 *cp_db_conn = db_connections[rand() % _DB_CONNECTIONS_];

                if (sqlite3_exec(cp_db_conn, buffer, callback, NULL, NULL) != SQLITE_OK)
                    strcpy(query_result, strtrim(mk_msg(getpid(), _SERVER_SRC_, _IPV4_, _NORM_ERR_, "Failed trying to execute query\n")));

                if (send(cl_socket_fd, query_result, _MAX_BUFF_SIZE_, 0) == -1)
                    show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed sending query result\n");

                if (strstr(query_result, "Failed") == NULL)
                    log_query(_IPV4_, cp_db_conn);

                memset(query_result, 0, _MAX_BUFF_SIZE_);
            }
        }
        else
        {
            // Proceso padre
            char info[60];

            if (sprintf(info, "New client accepted, managed by subprocess #%d", cp_local_pid) < 0)
                show_msg(getpid(), _SERVER_SRC_, _IPV4_, _FATAL_ERR_, "Failed making info message");

            show_msg(getpid(), _SERVER_SRC_, _IPV4_, _INFO_, info);

            close(cl_socket_fd);
        }
    }
}

/**
 * @brief Se inicializa la conexión TCP/IPv6.
 *
 * @details Se inicia la conexión TCP/IPv6 y por cada
 *          cliente que se conecte al servidor mediante
 *          este protocolo, se crea un hilo hijo para
 *          escuchar los mensajes que el cliente envíe.
 *          El proceso hijo enviará el archivo de la base
 *          de datos al cliente, tomará una conexión a la
 *          base de datos y la utilizará para registrar
 *          un mensaje de notificación de transferencia
 *          realizada con éxito.
 *
 * @param port Número de puerto a utilizar para la conexión.
 * @param db_connections Arreglo de conexiones a la base de datos.
 */
void startup_ipv6_sv(uint16_t port, sqlite3 **db_connections)
{
    struct sockaddr_in6 struct_sv;
    struct sockaddr_in6 struct_cl;

    socklen_t client_len = sizeof(struct_cl);

    int socket_fd;

    // Creación del socket
    if ((socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed in socket creation");

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed trying to set port as reusable");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));

    struct_sv.sin6_family = AF_INET6;
    struct_sv.sin6_port = (in_port_t)htons(port);
    struct_sv.sin6_addr = in6addr_any;

    // Binding del socket del server
    if (bind(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed binding socket");

    if (listen(socket_fd, 5) == -1) // Máximo 5 clientes en espera simultánea
        show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed trying to listen to socket");

    char info_msg[30];

    if (sprintf(info_msg, "Available port: %d", htons(struct_sv.sin6_port)) < 0)
        show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed making info message");

    show_msg(getpid(), _SERVER_SRC_, _IPV6_, _INFO_, info_msg);

    while (1)
    {
        // Se esperan conexiones
        int cl_socket_fd = accept(socket_fd, (struct sockaddr *)&struct_cl, &client_len);

        if (cl_socket_fd == -1)
            show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed trying to accept client");

        // Por cada conexión, se crea un proceso hijo que reciba los mensajes
        int cp_ipv6_pid = fork();

        if (cp_ipv6_pid == -1)
            show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed on process forking for client handling");

        if (cp_ipv6_pid == 0)
        {
            // Proceso hijo
            close(socket_fd);

            memset(buffer, 0, _MAX_BUFF_SIZE_);

            send_file(cl_socket_fd);

            close(cl_socket_fd);

            log_query(_IPV6_, db_connections[rand() % _DB_CONNECTIONS_]);

            show_msg(getpid(), _SERVER_SRC_, _IPV6_, _INFO_, "File transfer successfully completed - Finishing current subprocess");

            exit(EXIT_SUCCESS);
        }
        else
        {
            // Proceso padre
            char msg[60];

            if (sprintf(msg, "New client accepted, managed by subprocess #%d", cp_ipv6_pid) < 0)
                show_msg(getpid(), _SERVER_SRC_, _IPV6_, _FATAL_ERR_, "Failed making info message");

            show_msg(getpid(), _SERVER_SRC_, _IPV6_, _INFO_, msg);

            close(cl_socket_fd);
        }
    }
}

/**
 * @brief Se inicializa la conexión TCP local.
 *
 * @details Se inicia la conexión TCP/IP local y por cada
 *          cliente que se conecte al servidor mediante
 *          este protocolo, se crea un hilo hijo para
 *          escuchar los mensajes que el cliente envíe.
 *          Cuando un cliente TCP/IP local envía una query,
 *          se realiza la consulta contra la base de datos
 *          y se le envía al cliente el resultado de la misma.
 *
 * @param socket_file Nombre del archivo a utilizar para la
 *                    comunicación entre cliente y servidor.
 * @param db_connections Arreglo de conexiones a la base de datos.
 */
void startup_local_sv(char *socket_file, sqlite3 **db_connections)
{
    unlink(socket_file); // Desligamos el archivo en caso de ya existir de corridas anteriores

    struct sockaddr_un struct_sv;
    struct sockaddr_un struct_cl;

    socklen_t sv_len;
    socklen_t client_len = sizeof(struct_cl);

    int socket_fd;

    // Creación del socket
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed in socket creation");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));
    struct_sv.sun_family = AF_UNIX;
    strcpy(struct_sv.sun_path, socket_file);

    sv_len = (socklen_t)(strlen(struct_sv.sun_path) + sizeof(struct_sv.sun_family));

    // Binding del socket del server
    if (bind(socket_fd, (struct sockaddr *)&struct_sv, sv_len) == -1)
        show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed binding socket");

    if (listen(socket_fd, 5) == -1) // Máximo 5 clientes en espera simultánea
        show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed trying to listen to socket");

    char info_msg[127];

    if (sprintf(info_msg, "Available port: %s", struct_sv.sun_path) < 0)
        show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed making info message");

    show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _INFO_, info_msg);

    while (1)
    {
        // Se esperan conexiones
        int cl_socket_fd = accept(socket_fd, (struct sockaddr *)&struct_cl, &client_len);

        if (cl_socket_fd == -1)
            show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed trying to accept client");

        // Por cada conexión, se crea un proceso hijo que reciba los mensajes
        int cp_local_pid = fork();

        if (cp_local_pid == -1)
            show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed on process forking for client handling");

        if (cp_local_pid == 0)
        {
            // Proceso hijo
            close(socket_fd);

            while (1)
            {
                memset(buffer, 0, _MAX_BUFF_SIZE_);

                if (read(cl_socket_fd, buffer, (_MAX_BUFF_SIZE_ - 1)) == -1)
                    show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed receiving message");

                if (strstr(buffer, _EOT_MSG_) != NULL)
                {
                    close(cl_socket_fd);

                    show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _INFO_, "Client dropped connection. Finishing current subprocess");

                    exit(EXIT_SUCCESS);
                }

                if (sqlite3_exec(db_connections[rand() % _DB_CONNECTIONS_], buffer, callback, NULL, NULL) != SQLITE_OK)
                    strcpy(query_result, strtrim(mk_msg(getpid(), _SERVER_SRC_, _LOCAL_, _NORM_ERR_, "Failed trying to execute query\n")));

                if (send(cl_socket_fd, query_result, _MAX_BUFF_SIZE_, 0) == -1)
                    show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed sending query result");

                memset(query_result, 0, _MAX_BUFF_SIZE_);
            }
        }
        else
        {
            // Proceso padre
            char info_msg[60];

            if (sprintf(info_msg, "New client accepted, managed by subprocess #%d", cp_local_pid) < 0)
                show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _FATAL_ERR_, "Failed making info message");

            show_msg(getpid(), _SERVER_SRC_, _LOCAL_, _INFO_, info_msg);

            close(cl_socket_fd);
        }
    }
}