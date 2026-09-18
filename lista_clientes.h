#ifndef LISTA_CLIENTES_H
#define LISTA_CLIENTES_H

typedef struct ClienteNodo {
    int socket;
    char username[20];
    char status[7];
    struct ClienteNodo *siguiente;
} ClienteNodo;

typedef struct {
    ClienteNodo *cabeza;
    int numClientes;
} ListaClientes;

ListaClientes* crear_lista();
void insertar_cliente(ListaClientes *lista, int socket, const char *username, const char *status);
int buscar_cliente(ListaClientes *lista, const char *username);

#endif