# author: Bonino, Francisco Ignacio.
# version: 0.5.2
# since: 2022-03-20

# Flags
CC = gcc
CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11
CCOMPILE = $(CC) $(CFLAGS)
SLIBF = ar rcs
DIRS = ./bin ./obj ./slib

# Main targets

all: build_folders srv cln

build_folders:
	mkdir -p $(DIRS)

# Libraries

# Librería estática propia: utilities
lib_utilities.a: utilities.o
	$(SLIBF) slib/$@ obj/$<

utilities.o: src/include/bodies/utilities.c src/include/headers/utilities.h
	$(CCOMPILE) -c $< -o obj/$@

# Librería estática propia: servers_setup
lib_servers_setup.a: servers_setup.o
	$(SLIBF) slib/$@ obj/$<

servers_setup.o: src/include/bodies/servers_setup.c src/include/headers/servers_setup.h
	$(CCOMPILE) -c $< -o obj/$@

# Librería estática propia: clients_setup
lib_clients_setup.a: clients_setup.o
	$(SLIBF) slib/$@ obj/$<

clients_setup.o: src/include/bodies/clients_setup.c src/include/headers/clients_setup.h
	$(CCOMPILE) -c $< -o obj/$@

# Librería sqlite3
lib_sqlite3.a: sqlite3.o
	$(SLIBF) slib/$@ obj/$<

sqlite3.o: src/include/bodies/sqlite3.c src/include/headers/sqlite3.h
	gcc -c $< -o obj/$@

# Binaries

# Binario del servidor
srv: srv.o lib_utilities.a lib_servers_setup.a lib_sqlite3.a
	$(CCOMPILE) -o bin/$@ obj/$< slib/lib_utilities.a slib/lib_servers_setup.a slib/lib_sqlite3.a -ldl -pthread

srv.o: src/server.c
	$(CCOMPILE) -c $< -o obj/$@

# Binario del cliente
cln: cln.o lib_utilities.a lib_clients_setup.a
	$(CCOMPILE) -o bin/$@ obj/$< slib/lib_utilities.a slib/lib_clients_setup.a

cln.o: src/client.c
	$(CCOMPILE) -c $< -o obj/$@

# Others

clean:
	rm -r $(DIRS) *.db