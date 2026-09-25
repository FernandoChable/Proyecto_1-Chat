# PROYECTO 1: CHAT

Este repositorio contiene tanto un servidor como un cliente, los cuales juntos conforman un chat.

Este chat sigue el protocolo indicado por el profesor Canek Peláez Valdéz, y que si revisas el código
tanto del servidor como del cliente puedes deducir un poco por dónde va.

El servidor está programado en C y el cliente en C#.

## Instrucciones para compilarlo

(Sé que el profesor me va a matar por poner esto pero prefiero ponerlo a que no se le compile por X o Y razón).

- **Servidor**: Para poder ***compilar*** el servidor, hay que posicionarnos en la carpeta **Proyecto_1-Chat/src/servidor** y poner el comando: gcc -Wall main.c modelo/ListaClientes.c modelo/ListaSalas.c vista/red_vista.c controlador/chat_controlador.c -lcjson -lpthread -o <nombre_del_ejecutable>.
Toda esta línea se debe a que tenemos que compilar todos los archivos a la vez.
Si de casualidad tienes la duda de cómo ***ejecutarlo***, usa ./<nombre_del_ejecutable>.

- **Cliente**: Para poder ***compilar*** y *ejecutar* el cliente, hay que posicionarnos en la carpeta **Proyecto_1-Chat/src/cliente/ClienteChat** y poner el comando: dotnet run

Siéntete libre de probarlo :D.

Este proyecto fue hecho por:
Fernando Chablé Alonzo
* 32332444-7
* fernandochable@ciencias.unam.mx
