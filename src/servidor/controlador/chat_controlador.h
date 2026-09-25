#ifndef CHAT_CONTROLADOR_H
#define CHAT_CONTROLADOR_H

#include <cjson/cJSON.h>
#include "../modelo/lista_clientes.h"
#include "../modelo/lista_salas.h"

// Esta función será como el que maneja a todo el equipo de trabajo, porque esta llama a alguna de todas las demás
void identificar_tipo(cJSON *json, int socket_cliente, ListaClientes *lista_clientes, ListaSalas *lista_salas);

// Aquí van todos los métodos individuales
void identify(ListaClientes *lista, int socket_cliente, cJSON *json);
void status(int socket_cliente, cJSON *json, ListaClientes *lista);
void users(int socket_cliente, ListaClientes *lista);
void text(int socket_cliente, cJSON *json, ListaClientes *lista);
void publictext(int socket_cliente, cJSON *json, ListaClientes *lista);
void newroom(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista);
void invite(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista);
void joinroom(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista);
void roomusers(int socket_sliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista);
void roomtext(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista);
void leaveroom(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista);
void disconnect(int socket_cliente, ListaSalas *lista_salas, ListaClientes *lista);

#endif