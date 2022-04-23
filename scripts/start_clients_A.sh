#!/bin/bash

# author: Bonino, Francisco Ignacio.
# version: 0.1
# since: 2022-04-09

# IMPORTANTE -- Correr este script desde la carpeta root del proyecto

echo "Cantidad de clientes A a instanciar:"
read clientsAmount

echo "Nombre del socket local a conectarse:"
read localSocketFilename

for ((i = 0; i < clientsAmount; i++))
do
   gnome-terminal -- ./bin/cln local $localSocketFilename
done