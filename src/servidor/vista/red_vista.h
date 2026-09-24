#ifndef RED_VISTA_H
#define RED_VISTA_H

#include <cjson/cJSON.h>
#include "../modelo/lista_clientes.h"

// Enviamos un JSON directamente al socket especificado con su salto de línea
void enviar_json(int socket_cliente, cJSON *json);

// Manda un JSON a todos los clientes conectados excepto al que manda el JSON
void notificar_usuarios_vista(ListaClientes *lista, int socket_emisor, cJSON *json);

#endif