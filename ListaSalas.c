#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "lista_clientes.h"

// Bueeeeeno, tocará hacer otra implementación de lista ligada simple, pero ahora para administrar las salas que haya en el servidor :v

// Aquí definimos la estructura para los nodos de las salas
typedef struct NodoSala{
    char room_name[20]; // Aquí guardaremos el nombre de la sala
    ListaClientes usuarios; // Aquí guardaremos los usuarios que son miembros de la sala
    struct NodoSala *siguiente; // Y aquí guardaremos la sala que le sigue a esta en la lista
} NodoSala;

// Ahora definiremos otra estructura que sirva como nuestra lista ligada simple (creo que me metí en un despapaye innecesario, verdad?)
typedef struct ListaSalas{
    NodoSala *cabeza; // Aquí guardaremos (como el nombre lo dice) la sala que es la actual cabeza de la lista
    int num_salas; // Y aquí guardaremos el número que representa la cantidad de salas que hay actualmente
} ListaSalas;

// De nuevo, hacemos lo que sería el equivalente a un constructor en java para poder inicializar nuestra lista
ListaSalas* crear_lista_salas(){ 
    ListaSalas *lista_salas = (ListaSalas *)malloc(sizeof(ListaSalas)); // Le reservamos a la lista su respectiva memmoria

    lista_salas->cabeza = NULL; // Obviamente, la lista empezaría vacía, por lo que no tendría cabeza
    lista_salas->num_salas = 0; // Lo mismo, empieza vacía

    return lista_salas;
}

// Ahora harémos un método para añadir una nueva sala a la lista
void insertar_sala(ListaSalas *lista, const char *room_name) {
    NodoSala *nueva_sala = (NodoSala *)malloc(sizeof(NodoSala)); // Reservamos memoria de nuevo

    strncpy(nueva_sala->room_name, room_name, sizeof(nueva_sala->room_name) - 1); // Asignamos el nombre de la sala (de nuevo, el -1 es para dejar un espacio para el caracter nulo)
    nueva_sala->room_name[sizeof(nueva_sala->room_name) - 1] = '\0';

    // Aquí añadimos la nueva sala a la lista
    nueva_sala->siguiente = lista->cabeza;
    lista->cabeza = nueva_sala;
    lista->num_salas++;
}

// Ahora harémos un método para poder eliminar una sala de la lista
void eliminar_sala(ListaSalas *lista, const char *room_name) {
    // Primero revisaremos si la sala que queremos eliminar es la cabeza de la lista
    if(strcmp(lista->cabeza->room_name, room_name) == 0) {
        NodoSala *a_eliminar = lista->cabeza;
        lista->cabeza = lista->cabeza->siguiente;
        free(a_eliminar);
        lista->num_salas--;
        return;
    }

    // Ahora revisaremos el resto de la lista en busca de la sala a eliminar
    NodoSala *actual = lista->cabeza->siguiente;
    NodoSala *anterior = lista->cabeza;

    while(actual != NULL) {
        if(strcmp(actual->room_name, room_name) == 0) {
            anterior->siguiente = actual->siguiente;
            free(actual);
            lista->num_salas--;
            return;
        }

        actual = actual->siguiente;
        anterior = anterior->siguiente;
    }

    // Igual, ya si llegó aquí es porque la sala no existía xd
    printf("No se encontró la sala a eliminar.");
}

// Luego, haremos un método que verifique si la sala sí exite o no en la lista
int buscar_sala(ListaSalas *lista, const char *room_name) {
    NodoSala *actual = lista->cabeza; // Este nodo nos servirá para poder iterar en toda la lista

    // Y ahora iteramos
    while(actual != NULL) {
        // Si esta verificación de aquí sí se cumple, entonces son iguales por lo que devolveremos 1
        if(strcmp(actual->room_name, room_name) == 0) {
            return 1;
        }

        actual = actual->siguiente; // Pasamos a la siguiente sala
    }

    // Si terminó el bucle, entonces la sala aún no existe
    return 0;
}

// Ahora, hacemos un método para buscar una sala en específico y que la devuelva
NodoSala obtener_sala(ListaSalas *lista, const char *room_name) {

    

    // Va a ser muy parecido al método buscar_sala
    NodoSala *actual = lista->cabeza;

    while(actual != NULL) {
        if(strcmp(actual->room_name, room_name) == 0) {
            return actual;
        }

        actual = actual->siguiente;
    }


}

// Por último, haremos un método para limpiar toda la lista y no dejar un cacho de memoria todo feo ahí solito
void limpiar_lista_salas(ListaSalas *lista) {
    // Primero comprobaremos si no hay salas en la lista
    if(lista->num_salas == 0) {
        printf("No hay listas para borrar :D\n");
        free(lista);
        return;
    }

    // Ahora sí vamos a limpiar la lista
    NodoSala *actual = lista->cabeza;
    NodoSala *siguiente;

    while(actual != NULL) {
        siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
        lista->num_salas--;
    }

    // Ya al final matamos a la lista que también ya me tiene harto
    free(lista);
    printf("Y se murió la lista :D\n");
}