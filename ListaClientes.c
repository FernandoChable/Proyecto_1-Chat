#include <stdio.h>
#include <stdlib.h>

// Este programa existe más que nada para poder tener la estructura que se usará para los clientes y no saturar de código el programa del servidor

// Aquí definimos la estructura para los nodos de los clientes
typedef struct ClienteNodo{
    int socket; // Guardaremos el socket del cliente
    char username[20]; // Guardaremos su username (ponemos 20 de capacidad por si las moscas)
    char status[7]; // Guardaremos su estado (solo ponemos de capacidad 6 porque el estado más largo es ACTIVE, dice 7 porque también se incluye el caracter nulo)
    struct ClienteNodo *siguiente; // Y como es una lista ligada simple, guardamos su siguiente nodo
} ClienteNodo;

// Ahora definiremos otra estructura que sirva como nuestra lista ligada simple (te odio C)
typedef struct ListaClientes{
    ClienteNodo *cabeza; // Obviamente nuestra lista necesita una cabeza
    int numClientes; // Esto nomas nos sirve para saber el tamaño de la lista
} ListaClientes;

// Ahora, haremos lo que sería el equivalente a un constructor en otros lenguajes para poder crear nuestra lista (Canek me va a matar x'd)
ListaClientes* crear_lista() {
    ListaClientes *lista = (ListaClientes *)malloc(sizeof(ListaClientes)); // Reservamos la respectivda memoria para la lista
    
    lista->cabeza = NULL; // Iniciamos la cabeza de la lista en NULL
    lista->numClientes = 0; // Como obviamente no hay clientes al crear la lista, es 0

    return lista;
}

// Luego, haremos un método para poder insertar un nuevo cliente a la lista muejejeje
void insertar_cliente(ListaClientes *lista, int socket, const char *username, const char *status) {
    ClienteNodo *nuevo_Cliente = (ClienteNodo *)malloc(sizeof(ClienteNodo)); // Reservamos la respectiva memoria del nuevo cliente

    nuevo_Cliente->socket = socket; // Asignamos el socket al nuevo cliente
    strncpy(nuevo_Cliente->username, username, sizeof(nuevo_Cliente->username) - 1); // Asignamos su username (el -1 es para dejar al menos un espacio libre para el caracter nulo y que sea una cadena válida)
    nuevo_Cliente->username[sizeof(nuevo_Cliente->username) - 1] = '\0';
    strncpy(nuevo_Cliente->status, status, sizeof(nuevo_Cliente->status) - 1); // Asignamos su estado
    nuevo_Cliente->status[sizeof(nuevo_Cliente->status) - 1] = '\0';

    // Aquí ya añadimos el nuevo cliente a la lista
    nuevo_Cliente->siguiente = lista->cabeza;
    lista->cabeza = nuevo_Cliente;
    lista->numClientes++;
}

// Por último, haremos un método que verifique si el cliente ya existe o aún no en la lista (nos servirá para el identify)
int buscar_cliente(ListaClientes *lista, ClienteNodo *cliente) {
    ClienteNodo *actual = lista->cabeza; // Hacemos un nodo que nos servirá como un puntero para iterar en la lista

    // A continuación, haremos un while donde mientras actual no sea NULL, revise si el cliente sobre el que está parado es igual al que tenemos como parámetro
    while (actual != NULL)
    {
        if (strcmp(actual->username, cliente->username) == 0) { // Aquí verificamos con strcmp si los dos username son iguales
            return 1; // Solo regresamos 1 en caso de que haya salido uno igual
        }

        actual = actual->siguiente; // Pasamos al siguiente cliente.
    }

    //Si ya terminó el bucle, entonces no hay nadie igual al cliente.
    return 0;
}