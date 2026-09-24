using System;
using ClienteChat.controlador;
using ClienteChat.servicios;
using ClienteChat.vista;

// Esto sería como el main de nuestro cliente

namespace ClienteChat
{
    class Program
    {
        static void Main(string[] args)
        {
            // Vamos a instanciar nuestros componentes
            var vista = new VistaConsola();
            var networkService = new NetworkService();
            var controlador = new ChatControlador(networkService, vista);

            // Ahora vamos a conectar (o al menos intentarlo) al cliente con el servidor
            string ip = "192.168.100.85";
            int puerto = 5100;

            vista.MostrarInformacion($"Conectando al servidor en {ip}: {puerto}");

            if(!networkService.Conectar(ip, puerto))
            {
                vista.MostrarError("No se pudo conectar con el servidor ;(");
                return;
            }

            vista.MostrarInformacion("¡Estamos conectados!");
            Console.WriteLine("-------------------------------------------------------------------------");

            // Iniciamos un bucle para poder leer las entradas del usuario
            while(true)
            {
                // Guardamos la entrada del usuario
                string? entrada = vista.LeerEntrada();

                // En caso de que el usuario no ponga nada, rompemos el bucle
                if(entrada == null)
                {
                    break;
                }

                // Ahora le pasamos la chamba al controlador
                controlador.IdentificarTipo(entrada);
            }

            // Limpiamos al salir
            networkService.Desconectar();
        }
    }
}