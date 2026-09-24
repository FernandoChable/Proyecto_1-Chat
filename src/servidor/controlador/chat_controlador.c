#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chat_controlador.h"
#include "../vista/red_vista.h"

// Este método lo usamos para identificar el tipo que viene en el JSON
void identificar_tipo(cJSON *json, int socket_cliente, ListaClientes *lista_clientes, ListaSalas *lista_salas) {
    cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type"); // En type vamos a guardar el tipo de mensaje que mandó el cliente

    // Ahora aquí vamos a revisar cuál es el tipo para saber a qué otra función debe llamar esta función
    if(cJSON_IsString(type) && (type->valuestring != NULL)) {
        // Revisamos a qué función pertenece el tipo

        // En caso de que type sea IDENTIFY
        if(strcmp(type->valuestring, "IDENTIFY") == 0) {
            identify(lista_clientes, socket_cliente, json);
        }

        // En caso de que type sea STATUS
        if(strcmp(type->valuestring, "STATUS") == 0) {
            status(socket_cliente, json, lista_clientes);
        }

        // En caso de que type sea USERS
        if(strcmp(type->valuestring, "USERS") == 0) {
            users(socket_cliente, lista_clientes);
        }

        // En caso de que sea type TEXT
        if(strcmp(type->valuestring, "TEXT") == 0) {
            text(socket_cliente, json, lista_clientes);
        }

        // En caso de que sea type PUBLIC_TEXT
        if(strcmp(type->valuestring, "PUBLIC_TEXT") == 0) {
            publictext(socket_cliente, json, lista_clientes);
        }

        // En caso de que sea type NEW_ROOM
        if(strcmp(type->valuestring, "NEW_ROOM") == 0) {
            newroom(socket_cliente, json, lista_salas, lista_clientes);
        }

        // En caso de que sea type INVITE
        if(strcmp(type->valuestring, "INVITE") == 0) {
            invite(socket_cliente, json, lista_salas, lista_clientes);
        }
    }
}

// Aquí ya van el resto de métodos individuales

// Definición de:
// IDENTIFY
// :)
void identify(ListaClientes *lista, int socket_cliente, cJSON *json) {
    cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");
    cJSON *response = cJSON_CreateObject();

    cJSON_AddStringToObject(response, "type", "RESPONSE");
    cJSON_AddStringToObject(response, "operation", "IDENTIFY");

    // Aquí revisamos que el username no exista ya
    if(buscar_cliente(lista, username->valuestring) == 0) {
        insertar_cliente(lista, socket_cliente, username->valuestring, "ACTIVE");

        // Empezamos a formar todo el JSON de respuesta
        cJSON_AddStringToObject(response, "result", "SUCCESS");
        cJSON_AddStringToObject(response, "extra", username->valuestring);

        // Ahora hacemos el JSON para todos los demás usuarios
        cJSON *notif = cJSON_CreateObject();
        cJSON_AddStringToObject(notif, "type", "NEW_USER");
        cJSON_AddStringToObject(notif, "username", username->valuestring);

        // Ahora mandamos el mensaje a todos los demás usuarios
        notificar_usuarios_vista(lista, socket_cliente, notif);
        
        // Luego liberamos memoria
        cJSON_Delete(notif);
    } else {
        // Aquí formamos el JSON de respuesta en caso de que ya exista el username
        cJSON_AddStringToObject(response, "result", "USER_ALREADY_EXISTS");
        cJSON_AddStringToObject(response, "extra", username->valuestring);
    }

    enviar_json(socket_cliente, response);

    // Ahora liberamos toooda la memoria usada
    cJSON_Delete(response);
}

// Definición de
// STATUS
// :)
void status(int socket_cliente, cJSON *json, ListaClientes *lista) {
    cJSON *status = cJSON_GetObjectItemCaseSensitive(json, "status");

    char *nuevo_status = status->valuestring;

    // Esta parte de aquí la usaremos para poder actualizar el estado del cliente en la lista
    ClienteNodo *actual = lista->cabeza;
    
    while(actual != NULL) {
        if(actual->socket == socket_cliente) {
            // Esta comprobación también la hacemos en el lado del cliente así que no impora mucho xd, lo único que importa aquí es ver si el estado del usuario es igual al que quiere poner
            if((strcmp(nuevo_status, "ACTIVE") == 0 || strcmp(nuevo_status, "AWAY") == 0 || strcmp(nuevo_status, "BUSY") == 0) && strcmp(nuevo_status, actual->status) != 0) {
                // En caso de que pase la comprobación, actualizamos el estado del usuario
                strncpy(actual->status, status->valuestring, sizeof(actual->status) - 1);
                actual->status[sizeof(actual->status) - 1] = '\0';

                // Aquí formaremos el JSON y se lo mandaremos a los demás usuarios

                cJSON *notif = cJSON_CreateObject();
                cJSON_AddStringToObject(notif, "type", "NEW_STATUS");
                cJSON_AddStringToObject(notif, "username", actual->username);
                cJSON_AddStringToObject(notif, "status", nuevo_status);

                notificar_usuarios_vista(lista, socket_cliente, notif);

                // Liberamos la memoria utilizada
                cJSON_Delete(notif);
                return;
            }
        }

        actual = actual->siguiente;
    }
}

// Definición de
// USERS
// :)
void users(int socket_cliente, ListaClientes *lista) {
    // Empezaremos directamente creando la respuesta pues es la parte más simple del protocolo (hasta ahora)
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "type", "USER_LIST");

    // Ahora vamos a crear un JSON que almacenará toooooda la lista de usuarios
    cJSON *user_list = cJSON_CreateObject();

    // Y ahora vamos a recorrer toda la lista de usuarios para poder añadirlos a la lista
    ClienteNodo *actual = lista->cabeza;
    while(actual != NULL) {
        cJSON_AddStringToObject(user_list, actual->username, actual->status);
        actual = actual->siguiente;
    }

    // Metemos la lista de usuarios al JSON
    cJSON_AddItemToObject(response, "users", user_list);

    // Lo mandamos al usuario que lo solicitó
    enviar_json(socket_cliente, response);

    cJSON_Delete(response);
}

// Definición de
// TEXT
// :)
void text(int socket_cliente, cJSON *json, ListaClientes *lista) {
    cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");
    cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "text");

    // Primero, confirmaremos que en efecto existe el usuario al que se le quiere mandar el mensaje
    if(buscar_cliente(lista, username->valuestring) == 1) {

        //Vamos a iterar en la lista de usuarios, por lo que vamos a guardar el socket del usuario al que se le quiere mandar el mensaje
        int socket_destino = 0;
        // También como soy tonto, voy a usar el while que ya teníamos para poder obtener el username de la persona que manda el mensaje xd
        char *username_emisor;

        ClienteNodo *actual = lista->cabeza;

        while(actual != NULL) {
            // Si esta condición de aquí se cumple, quiere decir que ya encontró al usuario al que se le quiere mandar mensaje
            if(strcmp(actual->username, username->valuestring) == 0) {
                socket_destino = actual->socket;
            }

            // Esta mausquerramienta de aquí nos ayudará más tarde (encontrar el username del emisor)
            if(actual->socket == socket_cliente) {
                username_emisor = actual->username;
            }

            // Si no pasa la condición, revisamos si el siguiente en la lista es el que buscamos
            actual = actual->siguiente;
        }

        // Ya que haya salido del while, entonces vamos a formar el json que contendrá al mensaje
        cJSON *notif = cJSON_CreateObject();
        cJSON_AddStringToObject(notif, "type", "TEXT_FROM");
        cJSON_AddStringToObject(notif, "username", username_emisor);
        cJSON_AddStringToObject(notif, "text", message->valuestring);

        // Y ps lo mandamos
        enviar_json(socket_destino, notif);

        cJSON_Delete(notif);

    } else { // En caso de que no exista, el servidor responderá
        cJSON *response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "TEXT");
        cJSON_AddStringToObject(response, "result", "NO_SUCH_USER");
        cJSON_AddStringToObject(response, "extra", username->valuestring);

        enviar_json(socket_cliente, response);

        cJSON_Delete(response);
    }
}

// Definición de
// PUBLIC_TEXT
// :)
void publictext(int socket_cliente, cJSON *json, ListaClientes *lista) {
    cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "text");

    // En este caso, no hay que revisar si el usuario existe porque pues, es para todos, si acaso lo que hay que revisar es si hay más de un usuario, para que en caso de que no, no haga nada
    if(lista->numClientes > 1) {
        // Hay que iterar en la lista para poder saber quién mandó el mensaje xd
        char *username_emisor;

        ClienteNodo *actual = lista->cabeza;
        while(actual != NULL) {
            // Ha vuelto la mausquerramienta
            if(actual->socket == socket_cliente) {
                username_emisor = actual->username;
            }
            actual = actual->siguiente;
        }

        // Empezamos a formar el JSON
        cJSON *notif = cJSON_CreateObject();
        cJSON_AddStringToObject(notif, "type", "PUBLIC_TEXT_FROM");
        cJSON_AddStringToObject(notif, "username", username_emisor);
        cJSON_AddStringToObject(notif, "text", message->valuestring);

        // Y se lo mandamos a todoooos (menos al usuario que lo mandó obvio xd)
        notificar_usuarios_vista(lista, socket_cliente, notif);

        cJSON_Delete(notif);
    }
}

// Definición de
// NEW_ROOM
// :)
void newroom(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista) {
    cJSON *room_name = cJSON_GetObjectItemCaseSensitive(json, "roomname");
    cJSON *response = cJSON_CreateObject();

    // Primero vamos a comprobar que la sala no exista ya, en caso de que sí pues le decimos al usuario que ya existe una sala con ese nombre
    if(buscar_sala(lista_salas, room_name->valuestring) == 1) {
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "NEW_ROOM");
        cJSON_AddStringToObject(response, "result", "ROOM_ALREADY_EXISTS");
        cJSON_AddStringToObject(response, "extra", room_name->valuestring);
    } else {
        // En caso de que no exista ya, la creamos :D
        insertar_sala(lista_salas, room_name->valuestring);

        // Ahora vamos a buscar el username y el estado del usuario que creó la sala
        ClienteNodo *actual = lista->cabeza;
        char *username;
        char *status;
        while(actual != NULL) {
            if(actual->socket == socket_cliente) {
                username = actual->username;
                status = actual->status;
            }

            actual = actual->siguiente;
        }

        // Ahora vamos a meter al usuario que creó la sala
        NodoSala *nueva_sala = obtener_sala(lista_salas, room_name->valuestring);

        insertar_cliente(nueva_sala->usuarios, socket_cliente, username, status);

        // Por último, vamos a hacer el JSON de respuesta
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "NEW_ROOM");
        cJSON_AddStringToObject(response, "result", "SUCCESS");
        cJSON_AddStringToObject(response, "extra", room_name->valuestring);
    }

    // Y lo mandamos
    enviar_json(socket_cliente, response);

    cJSON_Delete(response);
}

// Definición de
// INVITE
// :)
void invite(int socket_cliente, cJSON *json, ListaSalas *lista_salas, ListaClientes *lista) {
    cJSON *roomname = cJSON_GetObjectItemCaseSensitive(json, "roomname");
    cJSON *usernames = cJSON_GetObjectItemCaseSensitive(json, "usernames");

    if(!cJSON_IsString(roomname) || !cJSON_IsArray(usernames)) {
        return;
    }

    // Primero vamos a revisar que efectivamente existe la sala
    if(buscar_sala(lista_salas, roomname->valuestring) == 0) {
        // Si no existe, le notificaremos al usuario que puso una sala inexistente xd
        cJSON *response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "INVITE");
        cJSON_AddStringToObject(response, "result", "NO_SUCH_ROOM");
        cJSON_AddStringToObject(response, "extra", roomname->valuestring);

        enviar_json(socket_cliente, response);

        cJSON_Delete(response);
        return;
    }

    // Ahora, vamos a verificar que todos los usuarios especificados en el arreglo sí existen
    cJSON *user_item = NULL;
    cJSON_ArrayForEach(user_item, usernames) {
        if(cJSON_IsString(user_item) && user_item->valuestring != NULL) {
            if(buscar_cliente(lista, user_item->valuestring) == 0) {
                // Al detectar al primer usuario inexistente, respondemos que no existe
                cJSON *response = cJSON_CreateObject();
                cJSON_AddStringToObject(response, "type", "RESPONSE");
                cJSON_AddStringToObject(response, "operation", "INVITE");
                cJSON_AddStringToObject(response, "result", "NO_SUCH_USER");
                cJSON_AddStringToObject(response, "extra", user_item->valuestring);

                enviar_json(socket_cliente, response);

                cJSON_Delete(response);
                return;
            }
        }
    }

    // Ahora que ya sabemos que tanto la sala como los usuarios existen, vamos a obtener el nombre de usuario del que mandó la invitación
    ClienteNodo *actual = lista->cabeza;
    char *emisor;
    while(actual != NULL) {
        if(actual->socket == socket_cliente) {
            emisor = actual->username;
        }
        actual = actual->siguiente;
    }

    // Y por último vamos a mandar todas las invitaciones
    NodoSala *sala = obtener_sala(lista_salas, roomname->valuestring);

    cJSON_ArrayForEach(user_item, usernames) {
        // Si el usuario ya está en la sala simplemente lo vamos a ignorar
        if(sala != NULL && buscar_cliente(sala->usuarios, user_item->valuestring) == 1) {
            continue;
        }

        // Vamos ahora a buscar el socket del usuario al que se quiere invitar
        int socket_destino = -1;
        ClienteNodo *actual_destino = lista->cabeza;
        while(actual_destino != NULL) {
            if(strcmp(user_item->valuestring, actual_destino->username) == 0) {
                socket_destino = actual_destino->socket;
                break;
            }

            actual_destino = actual_destino->siguiente;
        }

        // Y por último vamos a mandarle la invitación al usuario
        if(socket_destino != -1) {
            cJSON *notif = cJSON_CreateObject();
            cJSON_AddStringToObject(notif, "type", "INVITATION");
            cJSON_AddStringToObject(notif, "username", emisor);
            cJSON_AddStringToObject(notif, "roomname", sala);

            enviar_json(socket_destino, notif);
            cJSON_Delete(notif);
        }
    }
}