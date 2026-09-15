using System;
using System.Text;
using System.Text.Json.Nodes;
using System.Net.Sockets;

namespace ClienteChat
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                // Esto solo lo usaré como referencia xd
                Console.WriteLine("Conectando con el servidor...");

                using(TcpClient cliente = new TcpClient("192.168.100.85", 5100)) // El cliente se conectará por la IP de mi internet (de momento) por el puerto 5100
                using(NetworkStream stream = cliente.GetStream())
                {
                    Console.WriteLine("Conexión con el servidor lograda con éxito");

                    // Como aún no tengo nada de la interfaz gráfica, lo manejaré de momento con comandos
                    Console.WriteLine("Ingrese su comando:");
                    string entrada = Console.ReadLine(); // Leeremos el comando que ponga el usuario

                    // Primero haré la parte IDENTIFY del protocolo:
                    if(entrada.StartsWith("/identify"))
                    {
                        string[] partes = entrada.Split(' '); // Dividimos la entrada para poder armar el JSON

                        if(partes.Length > 1) // Verificamos que no haya sido únicamente el comando y sí haya información importante (el username)
                        {
                            // Empezamos a armar nuestro JSON
                            var iden_json = new JsonObject();

                            iden_json["type"] = "IDENTIFY"; // Decimos que el tipo de comando es IDENTIFY
                            iden_json["username"] = partes[1]; // Decimos que el username es el especificado por el usuario

                            string json_texto = iden_json.ToJsonString(); // Convertimos el JSON en un string
                            byte[] datos_bytes = Encoding.UTF8.GetBytes(json_texto); // Convertimos el string que tiene el JSON a bytes para enviarlo por la red
                            stream.Write(datos_bytes, 0, datos_bytes.Length); // Enviamos esos bytes al servidor
                            Console.WriteLine("Mensaje enviado al servidor."); // Esto igual lo uso como referencia de que funcionó
                        } else
                        {
                            Console.WriteLine("Error: Debe proporcionar un nombre de usuario."); // Informamos al usuario que la regó xd
                            return;
                        }   
                    }
                    
                }
            } catch(Exception e)
            {
                Console.WriteLine($"Error: {e.Message}"); // En caso de algún error, lo imprimimos
            }
        }
    }
}