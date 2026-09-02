#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>

int main() {

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

    int new_socket = accept(server_fd, NULL, NULL); //Aceptamos la conexión entrante.
    if(new_socket < 0) { //Comprobamos si hubo algún error al aceptar la conexión.
        printf("Error al aceptar la conexión\n");
        close(server_fd);
        exit(1);
    }

    printf("Se ha conectado un nuevo cliente\n");

    char buffer[1024] = {0}; //Creamos un buffer para almacenar los datos enviados por el cliente.

    int new_read = read(new_socket, buffer, sizeof(buffer)); //Leemos los datos enviados por el cliente.
    if(new_read <0) { //Comprobamos si hubo algún error al leer los datos.
        printf("Error al leer los datos\n");
        exit(1);
    }
    printf("Datos recibidos del cliente: %s\n", buffer); //Imprimimos los datos para tener una referencia de que sí funciona xd.

    char *response = "Mensaje recibido correctamente"; //Esto lo creé más que nada para que el servidor tenga algo para enviar en lo que hago el resto de métodos.
    int new_write = write(new_socket, response, strlen(response)); //Enviamos los datos al cliente.
    if(new_write < 0) { //Comprobamos si hubo algún error al enviar los datos.
        printf("Error al enviar los datos\n");
        exit(1);
    }

    close(new_socket); //Cerramos el socket.

    return 0;
}