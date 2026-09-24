#ifndef LISTA_SALAS_H
#define LISTA_SALAS_H
#include "lista_clientes.h"

typedef struct NodoSala {
    char room_name[20];
    ListaClientes *usuarios;
    struct NodoSala *siguiente;
} NodoSala;

typedef struct {
    NodoSala *cabeza;
    int num_salas;
} ListaSalas;

ListaSalas* crear_lista_salas();
void insertar_sala(ListaSalas *lista, const char *room_name);
void eliminar_sala(ListaSalas *lista, const char *room_name);
int buscar_sala(ListaSalas *lista, const char *room_name);
NodoSala* obtener_sala(ListaSalas *lista, const char *room_name);
void limpiar_lista_salas(ListaSalas *lista);

#endif