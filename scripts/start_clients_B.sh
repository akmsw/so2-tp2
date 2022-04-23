#!/bin/bash

# author: Bonino, Francisco Ignacio.
# version: 0.1
# since: 2022-04-09

# IMPORTANTE -- Correr este script desde la carpeta root del proyecto

echo "Cantidad de clientes B a instanciar:"
read clientsAmount

echo "Dirección IPv4 del server:"
read serverAddress

echo "Puerto a conectarse:"
read portNumber

for ((i = 0; i < clientsAmount; i++))
do
   gnome-terminal -- ./bin/cln ipv4 $serverAddress $portNumber
done