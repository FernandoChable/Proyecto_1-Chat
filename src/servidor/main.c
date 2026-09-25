#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <cjson/cJSON.h>

// Esto de aquí es todo el modelo MVC que hicimos en las otras carpetas
#include "modelo/lista_clientes.h"
#include "modelo/lista_salas.h"
#include "controlador/chat_controlador.h"

// Esta pequeña estructura existe para poder empaquetar los datos que necesitamos en el hilo de ejcución y ya xd
typedef struct DatosHilo{
    int socket;
    ListaClientes *lista;
    ListaSalas *lista_salas;
} DatosHilo;

// Definimos la función que va a ejecutar el hilo que atiende al cliente.
void *atender_cliente(void *arg) {
    // Primero vamos a recuperar nuestro DatosHilo
    DatosHilo *datos = (DatosHilo*)arg;

    int socket_cliente = datos->socket; // Obtenemos el socket del cliente
    ListaClientes *lista = datos->lista; // Obtenemos la lista
    ListaSalas *lista_salas = datos->lista_salas;

    free(datos); // Liberamos la memoria que usamos para nuestro paquetito de datos

    size_t tamano_buffer = 1048576; // Esto de aquí lo hice porque empezó a haber un error en el que los json se mostraban todos fragmentados

    char *buffer = (char *)malloc(tamano_buffer); //Creamos un buffer para almacenar los datos enviados por el cliente. (ACTUALIZACIÓN: Cambié el tamaño de 1024 bytes a 1,048,576 para que sea de exatcamente 1mb)
    if(buffer == NULL) {
        printf("Error: No se puede asignar memoria en el buffer.");
        close(socket_cliente);
        return NULL;
    }

    while(1) {
        // Limpiamos el buffer en cada iteración nueva
        memset(buffer, 0, tamano_buffer);

        // Lectura de datos
        int new_read = read(socket_cliente, buffer, tamano_buffer); //Leemos los datos enviados por el cliente.
        if(new_read <= 0) { //Comprobamos si hubo algún error al leer los datos o si se desconectó el usuario
            if(new_read < 0) {
                printf("Error al leer los datos.\n");
            } else {
                printf("El usuario se ha desconectado.\n");
            }

            break;
        }
        
        printf("Datos recibidos del cliente: %s\n", buffer); //Imprimimos los datos para tener una referencia de que sí funciona xd.

        cJSON *json = cJSON_Parse(buffer); // Convertimos los datos del cliente a un objeto JSON    

        if(json != NULL) {
            // Le mandamos toda la lógica al controlador
            identificar_tipo(json, socket_cliente, lista, lista_salas);
            cJSON_Delete(json);
        }
    }

    eliminar_cliente(lista, socket_cliente); // Eliminamos al cliente así en lo que hacemos la función desconectar
    close(socket_cliente); //Cerramos el socket.
    free(buffer); // Liberamos todo el buffer del cliente
    return NULL;
}

// Podré haber hecho de nuevo el servidor, pero recordemos que:
/*
    Créditos a SanjayRV con su articulo: https://dev.to/sanjayrv/a-beginners-guide-to-socket-programming-in-c-5an5
    por el super paro que me tiró para poder hacer y entender cómo hacer un servidor con sockets en C, pues no tenía ni la menor idea de cómo hacerlo xd.
*/

// Aquí ya empezamos con el main del servidor.
int main() {

    // Primero creamos nuestra lista que contendrá a los clientes conectados E IDENTIFICADOS al servidor
    ListaClientes *lista_clientes = crear_lista();

    // Ahora creamos una lista que contendrá a las salas que se creen (por si acaso xd)
    ListaSalas *lista_salas = crear_lista_salas();

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
        datos->lista_salas = lista_salas; // Asociamos la lista de salas (igual, nos servirá para algunas funciones)

        pthread_t thread_id; // Creamos un identificador para el hilo de ejecución.

        // Ahora creamos un hilo de ejecución para atender a ese cliente.
        pthread_create(&thread_id, NULL, atender_cliente, (void*)datos);
        pthread_detach(thread_id);

        printf("Se ha conectado un nuevo cliente. Socket: %d\n", new_socket);

    }

    close(server_fd);
    return 0;
}
