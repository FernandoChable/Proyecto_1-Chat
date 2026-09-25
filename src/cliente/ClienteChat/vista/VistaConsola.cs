// Este programa de aquí es el que interactuará con el usuario y se comunicará con el controlador
using System;
using System.Runtime;

namespace ClienteChat.vista
{
    public class VistaConsola
    {
        // Esto de aquí nos sirve para que, en caso de que el usuario esté escribiendo algo y le llega un mensaje, no se rompa su comando
        private readonly object _lockconsole = new object();

        // Este método de aquí mostrará en la terminal el string que se le pase
        public void MostrarMensaje(string mensaje)
        {
            lock(_lockconsole)
            {
                Console.WriteLine(mensaje);
                Console.Write("> ");
            }
        }

        // Este método de aquí mostrará en la terminal el mensaje que haya llegado desde una sala en específico
        public void MostrarMensajeSala(string mensaje)
        {
            lock(_lockconsole)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine(mensaje);
                Console.ResetColor();
                Console.Write("> ");
            }
        }

        // Este método de aquí mostrará algún error que se presente
        public void MostrarError(string error)
        {
            lock(_lockconsole)
            {
                Console.ForegroundColor = ConsoleColor.Red; // Vamos a dejar el color rojo exclusivo para los errores
                Console.WriteLine($"---ERROR--- : {error}");
                Console.ResetColor(); // Reiniciamos el color de la terminal xd
            }
        }

        // Este método mostrará información importante
        public void MostrarInformacion(string informacion)
        {
            lock(_lockconsole)
            {
                Console.ForegroundColor = ConsoleColor.Cyan; // Igual que con los errores, para la info importante dejaremos el cyan
                Console.WriteLine($"---INFO--- : {informacion}");
                Console.ResetColor(); // De nuevo, reiniciamos el color de la terminal
                Console.Write("> ");
            }
        }

        // Este método de aquí leerá lo que escriba el usuario
        public string? LeerEntrada()
        {
            Console.Write("> ");
            string entrada = Console.ReadLine() ?? "";
            return entrada;
        }
    }
}