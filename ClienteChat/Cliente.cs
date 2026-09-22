using System;
using System.Text;
using System.Text.Json.Nodes;
using System.Net.Sockets;
using Microsoft.VisualBasic;
using System.Reflection;

namespace ClienteChat
{
    class Program
    {

        // Vamos a hacer un método para escuchar los mensajes que mande el servidor 
        static void EscucharServidor(NetworkStream stream)
        {
            byte[] buffer = new byte[1024]; // Hacemos nuestro buffer para guardar el mensaje del server
            try
            {
                while(true)
                {
                    // Esta linea va a esperar alguna respuesta del server, por lo que no va a avanzar
                    int bytesLeidos = stream.Read(buffer, 0, buffer.Length);

                    if(bytesLeidos == 0)
                    {
                        Console.WriteLine("\n[Sistema]: El servidor se ha desconectado.");
                        break;
                    }

                    // Ahora vamos a convertir los bytes del mensaje a un string
                    string mensaje = Encoding.UTF8.GetString(buffer, 0, bytesLeidos);
                    Console.WriteLine($"\n[Servidor]: {mensaje}");
                    Console.Write("> "); // Esto solamente es para indicar que puedes escribir xd
                }
            }
            catch (System.Exception)
            {
                Console.WriteLine("\n[Sistema]: Conexión con el servidor interrumpida.");
            }
        }

        // Este método de aquí lo usaremos para mandar el string de algún json por la red
        static void MandarString(NetworkStream stream, string json_texto)
        {
            byte[] datos_bytes = Encoding.UTF8.GetBytes(json_texto); // Convertimos el string que tiene el JSON a bytes para enviarlo por la red
            stream.Write(datos_bytes, 0, datos_bytes.Length); // Enviamos esos bytes al servidor
            Console.WriteLine("Mensaje enviado al servidor."); // Esto igual lo uso como referencia de que funcionó
        }

        // Este método de aquí va a existir para identificar el comando que introdujo el usuario
        static void IdentificarTipo(NetworkStream stream, String entrada)
        {
            // Ahorita este método solo existe para cerrar el programa del cliente, ya después haré que le notifique al servidor que el cliente va a salir y así
            if(entrada.Equals("/exit"))
            {
                Environment.Exit(0);
            }

            // Aquí leerá si el comando es identify, para llamar a la función del mismo nombre
            if(entrada.StartsWith("/identify"))
            {
                Identify(stream, entrada);
            }

            // Aquí leerá si el comando es status, para llamar a la función del mismo nombre
            if(entrada.StartsWith("/status"))
            {
                Status(stream, entrada);
            }

            // Aquí leerá si el comando es users, para llamar a la función del mismo nombre
            if(entrada.StartsWith("/users"))
            {
                Users(stream);
            }

            // Aquí leerá si el comando es text, para llamar a la función del mismo nombre
            if(entrada.StartsWith("/text"))
            {
                Text(stream, entrada);
            }

            // Aquí leerá si el comando es public_text, para llamar a la función del mismo nombre
            if(entrada.StartsWith("/public_text"))
            {
                PublicText(stream, entrada);
            }

            // Aquí leerá si el comando es new_room, para llamar a la función del mismo nombre
            if(entrada.StartsWith("/new_room"))
            {
                NewRoom(stream, entrada);
            }
        }

        // Definición de
        // IDENTIFY (del lado del cliente)
        // :)
        static void Identify(NetworkStream stream, String entrada)
        {
            string[] partes = entrada.Split(' ', 2); // Dividimos la entrada para poder armar el JSON

            if(partes.Length > 1) // Verificamos que no haya sido únicamente el comando y sí haya información importante (el username)
            {
                // Verifficamos que el nombre de usuario no sea mayor a 8 caracteres
                if(partes[2].Length <= 8)
                {
                    // Empezamos a armar nuestro JSON
                    var iden_json = new JsonObject();

                    iden_json["type"] = "IDENTIFY"; // Decimos que el tipo de comando es IDENTIFY
                    iden_json["username"] = partes[1]; // Decimos que el username es el especificado por el usuario

                    string json_texto = iden_json.ToJsonString(); // Convertimos el JSON en un string
                    MandarString(stream, json_texto); // Mandamos el JSON
                } else
                {
                    Console.WriteLine("Error: El nombre de usuario solo puede tener a lo mucho 8 caracteres");
                }
            } else
            {
                Console.WriteLine("Error: Debe proporcionar un nombre de usuario."); // Informamos al usuario que la regó xd
            }   
        }

        // Definición de
        // STATUS (del lado del cliente)
        // :)
        static void Status(NetworkStream stream, String entrada)
        {
            string[] partes = entrada.Split(' '); // Dividimos la entrada para poder armar el JSON

            // Verificamos que haya algo más aparte del comando (en este caso el status)
            if(partes.Length > 1)
            {
                // En el mero caso de que el usuario haya escrito su estado como quiso :v
                string estado = partes[1].ToUpper();

                // Comprobamos que su estado sea válido
                if(estado.Equals("ACTIVE") || estado.Equals("AWAY") || estado.Equals("BUSY"))
                {
                    // Ahora sí empezamos a armar nuestro JSON
                    var stat_json = new JsonObject();

                    stat_json["type"] = "STATUS"; // Decimos que el tipo de comando es STATUS
                    stat_json["status"] = estado; // Decimos que el status es el especificado por el usuario (pero en mayúsculas)

                    string json_texto = stat_json.ToJsonString(); // Convertimos el JSON a un string
                    MandarString(stream, json_texto); // Mandamos el JSON
                } else
                {
                    Console.WriteLine("Error: Debe proporcionar un estado válido, sea ACTIVE, AWAY ó BUSY"); // Informamos al usuario que la regó de nuevo
                }
            } else
            {
                Console.WriteLine("Error: Debe proporcionar el estado."); // Informamos al usuario que la regó xd
            }
        }

        // Definición de
        // USERS (del lado del cliente)
        // :)
        static void Users(NetworkStream stream)
        {
            // Como aquí es simplemente un comando y no hay nada más de información importante, no hace falta ni que usemos la entrada del usuario
            var users_json = new JsonObject();

            users_json["type"] = "USERS";

            // Ps un poco más de lo de siempre, mandamos el JSON al servidor y ya :v
            string json_texto = users_json.ToJsonString();
            MandarString(stream, json_texto);

            // No puedo creer lo sencilla que fue esta parte
        }

        // Definición de
        // TEXT (del lado del cliente)
        // :)
        static void Text(NetworkStream stream, String entrada)
        {
            // Aquí vamos a dividir la entrada únicamente en tres partes, esto porque el formato sería /text <usuario> <mensaje>, y no queremos que el mensaje se divida también
            string[] partes = entrada.Split(' ', 3);

            // Verificamos que no haga falta información importante, en este caso aparte del comando también se tiene que escribir el destinatario y el mensaje
            if(partes.Length > 2)
            {
                // Empezamos a crear nuestro JSON
                var text_json = new JsonObject();

                text_json["type"] = "TEXT"; // Decimos que es de tipo TEXT
                text_json["username"] = partes[1]; // Decimos quién es el destinatario
                text_json["text"] = partes[2]; // Decimos que el mensaje es el resto de la entrada

                // Por como cuarta vez, mandamos el JSON :v
                string json_texto = text_json.ToJsonString();
                MandarString(stream, json_texto);
            } else
            {
                Console.WriteLine("Error: El formato del comando es /text <usuario_destinatario> <mensaje_a_enviar>");
            }
        }

        // Definición de
        // PUBLIC_TEXT (del lado del cliente)
        // :)
        static void PublicText(NetworkStream stream, String entrada)
        {
            // Parecido a TEXT, vamos a partir la cadena pero ahora solo en dos partes, pues solamente son el comando y el mensaje, pues este último va para todos
            string[] partes = entrada.Split(' ', 2);

            // Verificamos que en efecto, exista un mensaje :v
            if(partes.Length > 1)
            {
                // Empezamos a crear el JSON que vamos a mandarle al servidor
                var publictext_json = new JsonObject();

                publictext_json["type"] = "PUBLIC_TEXT"; // Decimos que es de tipo PUBLIC_TEXT
                publictext_json["text"] = partes[1]; // Decimos que el mensaje es el resto de la entrada

                // Por quinta vez y contandoooo mandamos el JSON
                string json_texto = publictext_json.ToJsonString();
                MandarString(stream, json_texto);
            } else
            {
                Console.WriteLine("Error: Debe introducir un mensaje, el formato del comando es /public_text <mensaje_<_enviar>");
            }
        }

        // Definición de
        // NEW_ROOM (del lado del cliente)
        // :)
        static void NewRoom(NetworkStream stream, String entrada)
        {
            // Primero vamos a dividir la entrada en dos partes, el comando y el nombre de la sala
            string[] partes = entrada.Split(' ', 2);

            // Verificamos que no solo esté el comando y sí esté el nombre de la sala
            if(partes.Length < 2)
            {
                // Ahora, verificamos que el nombre sea de a lo mucho 16 caracteres
                if(partes[2].Length <= 8)
                {
                    // Ya que haya pasado esas dos verificaciones, ahora sí podemos empezar a trabajar
                    var newroom_json = new JsonObject();

                    newroom_json["type"] = "NEW_ROOM";
                    newroom_json["roomname"] = partes[1];

                    // Por sexta vez siiiii mandamos el JSON
                    string json_texto = newroom_json.ToJsonString();
                    MandarString(stream, json_texto);
                } else
                {
                    Console.WriteLine("Error: El nombre del cuarto debe ser de máximo 16 caracteres");
                }
            } else
            {
                Console.WriteLine("Error: Debes proporcionar un nombre para el cuarto (xd)");
            }
        }

        static void Main(string[] args)
        {
            try
            {
                // Esto solo lo usaré como referencia xd
                Console.WriteLine("Conectando con el servidor...");

                using(TcpClient cliente = new TcpClient("127.0.0.1", 5100)) // El cliente se conectará por la IP de mi internet (de momento) por el puerto 5100
                using(NetworkStream stream = cliente.GetStream())
                {
                    Console.WriteLine("Conexión con el servidor lograda con éxito");

                    // Esta linea de aquí sirve para tener un hilo de ejecución secundario y en ese hilo vamos a escuchar todo lo que diga el server
                    Task.Run(() => EscucharServidor(stream));

                    while(true)
                    {
                        // Como aún no tengo nada de la interfaz gráfica, lo manejaré de momento con comandos
                        Console.Write("> ");
                        string entrada = Console.ReadLine(); // Leeremos el comando que ponga el usuario

                        IdentificarTipo(stream, entrada); // Identificamos el comando que introdujo el usuario
                        
                    }
                    
                }
            } catch(Exception e)
            {
                Console.WriteLine($"Error: {e.Message}"); // En caso de algún error, lo imprimimos
            }
        }
    }
}