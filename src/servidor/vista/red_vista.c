#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "red_vista.h"

// Definimos nuestro método para mandar JSON
void enviar_json(int socket_cliente, cJSON *json) {
    if(json == NULL || socket_cliente < 0) {
        return;
    }

    // Hacemos al JSON un string
    char *json_str = cJSON_PrintUnformatted(json);
    if(json_str != NULL) {
        // Y la enviamos
        write(socket_cliente, json_str, strlen(json_str));
        write(socket_cliente, "\n", 1);

        // Y liberamos la memoria que usó el string del JSON
        free(json_str);
    }
}

// Ahora definimos un método para mandar un JSON a los demás usuarios
void notificar_usuarios_vista(ListaClientes *lista, int socket_emisor, cJSON *json) {
    if(json == NULL || socket_emisor < 0) {
        return;
    }

    // Ahora le mandamos a todos el JSON especificado
    ClienteNodo *actual = lista->cabeza;
    while(actual != NULL) {
        if(actual->socket != socket_emisor) {
            enviar_json(actual->socket, json);
        }

        actual = actual->siguiente;
    }
}