/**
 * @file server.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Servidor para el laboratorio #2 de Sistemas Operativos II.
 * @version 1.6.1
 * @since 2022-03-20
 */

#include "include/headers/servers_setup.h"

/**
 * @brief Función principal del servidor.
 *
 * @param argc Cantidad de argumentos recibidos.
 * @param argv Vector con los argumentos recibidos.
 *
 * @return 0 Si la ejecución del servidor fue exitosa.
 *         1 Si la ejecución del servidor tuvo errores.
 */
int main(int argc, char *argv[])
{
    int parent_pid = getpid();

    // Validación de argumentos
    if (argc == 2)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0) || (*argv[1] == '?'))
        {
            show_help();

            exit(EXIT_SUCCESS);
        }
        else if ((strcmp(argv[1], "-e") == 0) || (strcmp(argv[1], "--examples") == 0) || (*argv[1] == '!'))
        {
            show_examples();

            exit(EXIT_SUCCESS);
        }
        else
            show_msg(parent_pid, _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Invalid arguments amount. Run this program with '-h', '--help' or '?' for help");
    }

    if (argc != _SV_PARAMS_)
        show_msg(parent_pid, _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Invalid arguments amount. Run this program with '-h', '--help' or '?' for help");

    /*
     * Según 'The Linux programming interface', al ignorar la señal SIGCHLD
     * se logra que cualquier sub-proceso (del proceso actual) que termine,
     * sea removido del sistema en lugar de convertirse en un proceso zombie.
     */
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
        show_msg(parent_pid, _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to ignore signal SIGCHLD");

    /* ------------------ DATABASE ------------------- */

    // Creación y llenado de la base de datos
    startup_db(_DB_NAME_);

    // Establecimiento de cinco conexiones a la base de datos en memoria compartida
    key_t shmkey = ftok("./src/server.c", 'X');

    if (shmkey == -1)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed getting unique key for shared memory ID");

    int shmid = shmget(shmkey, (sizeof(sqlite3 *) * _DB_CONNECTIONS_), (IPC_CREAT | 0666));

    if (shmid == -1)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed on shared memory creation process");

    sqlite3 **db_connections = (sqlite3 **)shmat(shmid, (void *)0, 0);

    if (db_connections == (void *)-1)
        show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed on shared memory attachment process");

    for (int i = 0; i < _DB_CONNECTIONS_; i++)
        if (sqlite3_open_v2(_DB_NAME_, &db_connections[i], (SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX), NULL) != SQLITE_OK)
            show_msg(getpid(), _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed trying to setup database connections");

    /* ----------------- SOCKET LOCAL ----------------- */

    int cp_local_pid = fork();

    if (cp_local_pid == -1)
        show_msg(parent_pid, _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed on process forking for local socket");

    if (cp_local_pid == 0) // Proceso hijo - Creación de socket local
        startup_local_sv(argv[1], db_connections);

    /* ----------------- SOCKET IPv4 ----------------- */

    int cp_ipv4_pid = fork();

    if (cp_ipv4_pid == -1)
        show_msg(parent_pid, _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed on process forking for IPv4 socket");

    if (cp_ipv4_pid == 0) // Proceso hijo - Creación de socket TCP/IPv4
        startup_ipv4_sv((uint16_t)atoi(argv[2]), db_connections);

    /* ----------------- SOCKET IPv6 ----------------- */

    int cp_ipv6_pid = fork();

    if (cp_ipv6_pid == -1)
        show_msg(parent_pid, _SERVER_SRC_, _GENERAL_SRC_, _FATAL_ERR_, "Failed on process forking for IPv6 socket");

    if (cp_ipv6_pid == 0) // Proceso hijo - Creación de socket TCP/IPv6
        startup_ipv6_sv((uint16_t)atoi(argv[3]), db_connections);

    while (1)
    {
    }

    return 0;
}