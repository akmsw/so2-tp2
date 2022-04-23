#!/bin/bash

# author: Bonino, Francisco Ignacio.
# version: 0.1
# since: 2022-04-09

# IMPORTANTE -- Correr este script desde la carpeta root del proyecto

echo "Cantidad de clientes C a instanciar:"
read clientsAmount

echo "Dirección IPv6 del server:"
read serverAddress

echo "Interfaz IPv6 del server:"
read serverInterface

echo "Puerto a conectarse:"
read portNumber

for ((i = 0; i < clientsAmount; i++))
do
   gnome-terminal -- ./bin/cln ipv6 $serverAddress $serverInterface $portNumber copy$i.db
done