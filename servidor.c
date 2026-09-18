#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <cjson/cJSON.h>
#include "lista_clientes.h"

// Esta pequeña estructura existe para poder empaquetar los datos que necesitamos en el hilo de ejcución y ya xd
typedef struct DatosHilo{
    int socket;
    ListaClientes *lista;
} DatosHilo;

// Definimos la función que va a ejecutar el hilo que atiende al cliente.
void *atender_cliente(void *arg) {

    // Primero vamos a recuperar nuestro DatosHilo
    DatosHilo *datos = (DatosHilo*)arg;

    int socket_cliente = datos->socket; // Obtenemos el socket del cliente
    ListaClientes *lista = datos->lista; // Obtenemos la lista

    free(datos); // Liberamos la memoria que usamos para nuestro paquetito de datos

    char buffer[1024] = {0}; //Creamos un buffer para almacenar los datos enviados por el cliente.

    while(1) {
        // Limpiamos el buffer en cada iteración nueva
        memset(buffer, 0, sizeof(buffer));

        // Lectura de datos
        int new_read = read(socket_cliente, buffer, sizeof(buffer)); //Leemos los datos enviados por el cliente.
        if(new_read < 0) { //Comprobamos si hubo algún error al leer los datos.
            printf("Error al leer los datos\n");
            break;
        }
        
        printf("Datos recibidos del cliente: %s\n", buffer); //Imprimimos los datos para tener una referencia de que sí funciona xd.

        cJSON *json = cJSON_Parse(buffer); // Convertimos los datos del cliente a un objeto JSON    

        // Con esta función de aquí ya se manda cualquier cosa casi xd
        identificar_tipo(json, socket_cliente, lista);

        cJSON_Delete(json); // Liberamos la memoria del json
    }

    close(socket_cliente); //Cerramos el socket.

}

// Aquí abajo voy a definir los métodos de las respuestas que vienen en el protocolo, para intentar tener todo un poco "organizado" (entre muchísimas comillas)

// Esta siguiente función servirá para notificar cualquier JSON para todos los demás usuarios
void notificar_usuarios(ListaClientes *lista, int socket_cliente, char *notif_str) {
    ClienteNodo *actual = lista->cabeza;
        while(actual != NULL) {
            // Vamos a enviar el mensaje a los demás usuarios exceptuando al que se acaba de conectar (pues porque ni modo de decirle que acaba de entrar jeje)
            if(actual->socket != socket_cliente) {
                write(actual->socket, notif_str, strlen(notif_str));
            }
            actual = actual->siguiente;
        }

}

void identificar_tipo(cJSON *json, int socket_cliente, ListaClientes *lista) {
    cJSON *type = cJSON_GetObjectItemCaseSensitive(json, "type"); // En type vamos a guardar el tipo de mensaje que mandó el cliente

    // Ahora aquí vamos a revisar cuál es el tipo para saber a qué otra función debe llamar esta función
    if(cJSON_IsString(type) && (type->valuestring != NULL)) {
        // Revisamos a qué función pertenece el tipo
        if(strcmp(type->valuestring, "IDENTIFY") == 0) { // IDENTIFY
            identify(lista, socket_cliente, json);
        }
    }
}

// Definición de:
// IDENTIFY
// :)
void identify(ListaClientes *lista, int socket_cliente, cJSON *json) {
    cJSON *username = cJSON_GetObjectItemCaseSensitive(json, "username");
    cJSON *response = cJSON_CreateObject();

    // Aquí revisamos que el username no exista ya
    if(buscar_cliente(lista, username->valuestring) == 0) {
        insertar_cliente(lista, socket_cliente, username->valuestring, "ACTIVE");

        // Empezamos a formar todo el JSON de respuesta
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "IDENTIFY");
        cJSON_AddStringToObject(response, "result", "SUCCESS");
        cJSON_AddStringToObject(response, "extra", username->valuestring);

        // Ahora hacemos el JSON para todos los demás usuarios
        cJSON *notif = cJSON_CreateObject();
        cJSON_AddStringToObject(notif, "type", "NEW_USER");
        cJSON_AddStringToObject(notif, "username", username->valuestring);

        // Luego lo convertimos a string
        char *notif_str = cJSON_PrintUnformatted(notif);

        // Ahora mandamos el mensaje a todos los demás usuarios
        notificar_usuarios(lista, socket_cliente, notif_str);
        
        // Luego liberamos memoria
        cJSON_Delete(notif);
        free(notif_str);
    } else {
        // Aquí formamos el JSON de respuesta en caso de que ya exista el username
        cJSON_AddStringToObject(response, "type", "RESPONSE");
        cJSON_AddStringToObject(response, "operation", "IDENTIFY");
        cJSON_AddStringToObject(response, "result", "USER_ALREADY_EXISTS");
        cJSON_AddStringToObject(response, "extra", username->valuestring);
    }

    char *response_str = cJSON_PrintUnformatted(response);

    int new_write = write(socket_cliente, response_str, strlen(response_str)); //Enviamos los datos al cliente.
    if(new_write < 0) { //Comprobamos si hubo algún error al enviar los datos.
        printf("Error al enviar los datos\n");
    }

    // Ahora liberamos toooda la memoria usada
    cJSON_Delete(response);
    free(response_str);
}



// Aquí ya empezamos con el main del servidor.
int main() {

    // Primero creamos nuestra lista que contendrá a los clientes conectados E IDENTIFICADOS al servidor
    ListaClientes *lista_clientes = crear_lista();

    /*
    Créditos a SanjayRV con su articulo: https://dev.to/sanjayrv/a-beginners-guide-to-socket-programming-in-c-5an5
    por el super paro que me tiró para poder hacer y entender cómo hacer un servidor con sockets en C, pues no tenía ni la menor idea de cómo hacerlo xd.
    */

    int server_fd = socket(AF_INET, SOCK_STREAM, 0); //Usamos AF_INET para IPv4 y SOCK_STREAM para TCP.
    if(server_fd < 0) { //Comprobamos si hubo un error al crear el socket.
        printf("Error al crear el socket\n");
        exit(1);
    };

    printf("Se ha creado el socket con éxito: %d\n", server_fd); //Imprimimos en la terminal en caso de que se haya creado bien.

    struct sockaddr_in my_addr;

    bzero(&my_addr, sizeof(my_addr)); //Inicializamos la estructura my_addr en cero.

    my_addr.sin_family = AF_INET; //Definimos a la familia de direcciones como IPv4.
    my_addr.sin_port = htons(5100); //Asignamos el puerto 5100 al socket usando htons() para convertir el número del puerto a formato de red.
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Asignamos la dirección IP del socket a INADDR_ANY para que acepte conexiones desde cualquier dirección IP.

    int bind_result = bind(server_fd, (struct sockaddr*)&my_addr, sizeof(my_addr)); //Enlazamos el socket a la dirección IP y al puerto.
    if(bind_result < 0) { //Comprobamos si hubo un error al enlazar el socket a la dirección IP y puerto.
        printf("Error al hacer bind\n");
        close(server_fd);
        exit(1);
    };

    int listen_result = listen(server_fd, 5); //Hacemos que el socket escuche alguna conexión entrante.
    if(listen_result < 0) { //Comprobamos si hubo algún error al poner el socket en modo de escucha.
        printf("Error al hacer listen\n");
        close(server_fd);
        exit(1);
    }

    printf("Servidor esperando conexiones en el puerto 5100...\n"); //Esto lo puse para corroborar que al menos hasta aquí funciona el servidor xd.

    // Este bucle sirve para que acepte varias conexiones y no se muera con la primera.
    while(1) {

        int new_socket = accept(server_fd, NULL, NULL); //Aceptamos la conexión entrante.
        if(new_socket < 0) { //Comprobamos si hubo algún error al aceptar la conexión.
            printf("Error al aceptar la conexión\n");
            close(server_fd);
            exit(1);
        }

        // A continuación , guardaremos un espacio de memoria para poder desempaquetar el paquetito de datos que tiene el socket y la lista de clientes (la del mero inicio)
        DatosHilo *datos = (DatosHilo *)malloc(sizeof(DatosHilo));
        datos->socket = new_socket; // Asociamos el socket del cliente
        datos->lista = lista_clientes; // Asociamos la lista de clientes (nos servirá para algunas funciones)

        pthread_t thread_id; // Creamos un identificador para el hilo de ejecución.

        // Ahora creamos un hilo de ejecución para atender a ese cliente.
        pthread_create(&thread_id, NULL, atender_cliente, (void*)datos);

        printf("Se ha conectado un nuevo cliente\n");

    }

    return 0;
}
