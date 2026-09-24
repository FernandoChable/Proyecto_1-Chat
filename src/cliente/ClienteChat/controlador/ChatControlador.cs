using System;
using System.Reflection.Metadata;
using System.Text.Json.Nodes;
using System.Text.Json;
using ClienteChat.modelo;
using ClienteChat.servicios;
using ClienteChat.vista;
using Microsoft.VisualBasic;

// Aquí vamos a coordinar todo nuestro MVC para el cliente
namespace ClienteChat.controlador
{
    public class ChatControlador
    {
        private NetworkService _networkService;
        private VistaConsola _view;
        private UsuarioModelo _usuario;

        // Aquí creamos el controlador
        public ChatControlador(NetworkService networkService, VistaConsola view)
        {
            _networkService = networkService;
            _view = view;
            _usuario = new UsuarioModelo();

            // Y usamos los eventos de la capa de red
            _networkService.OnMessageReceived += ProcesarMensajeServidor;
            _networkService.OnDisconnected += ManejarDesconexion;
        }

        // Ahora, haremos el método (que ya teníamos) para identificar el tipo de comando que escribió el usuario, solo que lo vamos a modificar
        public void IdentificarTipo(String entrada)
        {
            // Vamos a partir la entrada para identificar el comando que introdujo el usuario y todo del argumento que dió
            string[] partes = entrada.Split(' ', 2);
            string comando = partes[0].ToLower(); // Aquí guardamos el comando
            string argumento; // Aquí guardamos el argumento del usuario en caso de que lo haya
            if(partes.Length > 1)
            {
                argumento = partes[1];
            } else
            {
                argumento = string.Empty;
            }

            // Aquí haremos una verificación para no dejar al usuario hacer nada en caso de que no se haya identificado todavía
            if(comando != "/identify" && !_usuario.EstaIdentificado)
            {
                _view.MostrarError("Debes identificarte primero usando el comando: /identify <username>");
                return;
            }

            // Usaremos un switch para simplificar todos los ifs que tenía
            switch(comando)
            {
                // El comando es IDENTIFY
                case "/identify":
                    Identify(argumento);
                    break;
                
                // El comando es STATUS
                case "/status":
                    Status(argumento);
                    break;
                
                // El comando es USERS
                case "/users":
                    Users();
                    break;
                
                // El comando es TEXT
                case "/text":
                    Text(argumento);
                    break;
                
                // El comando es PUBLIC_TEXT
                case "/public_text":
                    PublicText(argumento);
                    break;
                
                // El comando es NEW_ROOM
                case "/new_room":
                    NewRoom(argumento);
                    break;
                
                // El comando es INVITE
                case "/invite":
                    Invite(argumento);
                    break;

                // El comando es JOIN_ROOM
                case "/join_room":
                    JoinRoom(argumento);
                    break;

                // El comando es ROOM_USERS
                case "/room_users":
                    RoomUsers(argumento);
                    break;

                default:
                _view.MostrarError($"Introduzca un comando válido ({comando} no lo es).");
                break;
            }
        }

        // Aquí abajo ya van todos los métodos para comunicar al cliente con el servidor mediante mensajes JSON

        // Definición de
        // IDENTIFY (del lado del cliente)
        // :)
        public void Identify(String username)
        {
            // Verificamos que username no sea null o un espacio en blanco
            if(string.IsNullOrWhiteSpace(username))
            {
                _view.MostrarError("Uso correcto: /identify <username>");
                return;
            }

            // Verificamos que el nombre de usuario no sea mayor a 8 caracteres
            if(username.Length <= 8)
            {
                // Empezamos a armar nuestro JSON
                var iden_json = new JsonObject();

                iden_json["type"] = "IDENTIFY"; // Decimos que el tipo de comando es IDENTIFY
                iden_json["username"] = username; // Decimos que el username es el especificado por el usuario
                string json_texto = iden_json.ToJsonString(); // Convertimos el JSON en un string

                _networkService.Enviar(json_texto);
                _usuario.Username = username;
                    
            } else
            {
                _view.MostrarError("El nombre debe ser de menos de 8 caracteres.");
            }
        }

        // Definición de
        // STATUS (del lado del cliente)
        // :)
        public void Status(String entrada)
        {
            string estado = entrada.ToUpper();

            // Comprobamos que su estado sea válido
            if(estado.Equals("ACTIVE") || estado.Equals("AWAY") || estado.Equals("BUSY"))
            {
                // Ahora sí empezamos a armar nuestro JSON
                var stat_json = new JsonObject();

                stat_json["type"] = "STATUS"; // Decimos que el tipo de comando es STATUS
                stat_json["status"] = estado; // Decimos que el status es el especificado por el usuario (pero en mayúsculas)
                string json_texto = stat_json.ToJsonString(); // Convertimos el JSON a un string

                _networkService.Enviar(json_texto); // Enviamos el texto
                _usuario.Status = estado;
            } else
            {
                _view.MostrarError("El estado que quieres debe ser uno de los tres válidos: ACTIVE, AWAY o BUSY."); // Informamos al usuario que la regó
            }
        }

        // Definición de
        // USERS (del lado del cliente)
        // :)
        public void Users()
        {
            // Como aquí es simplemente un comando y no hay nada más de información importante, no hace falta ni que usemos la entrada del usuario
            var users_json = new JsonObject();

            users_json["type"] = "USERS";

            // Ps un poco más de lo de siempre, mandamos el JSON al servidor y ya :v
            string json_texto = users_json.ToJsonString();
            _networkService.Enviar(json_texto);

            // No puedo creer lo sencilla que fue esta parte
        }

        // Definición de
        // TEXT (del lado del cliente)
        // :)
        public void Text(String entrada)
        {
            // Vamos a partir la entrada porque el argumento debería contener al username del destinatario y el texto que se le quiere mandar
            string[] partes = entrada.Split(' ', 2);

            // Revisamos que no falte información importante
            if(partes.Length < 2)
            {
                _view.MostrarError("Falta información en el comando. Uso correcto: /text <username> <texto>");
                return;
            }

            // Empezamos a crear nuestro JSON
            var text_json = new JsonObject();

            text_json["type"] = "TEXT"; // Decimos que es de tipo TEXT
            text_json["username"] = partes[0]; // Decimos quién es el destinatario
            text_json["text"] = partes[1]; // Decimos que el mensaje es el resto de la entrada

            // Por como cuarta vez, mandamos el JSON :v
            string json_texto = text_json.ToJsonString();
            _networkService.Enviar(json_texto);
        }

        // Definición de
        // PUBLIC_TEXT (del lado del cliente)
        // :)
        public void PublicText(String mensaje)
        {
            // Verificamos que el mensaje no sea null o un espacio en blanco
            if(string.IsNullOrWhiteSpace(mensaje))
            {
                _view.MostrarError("Uso correcto: /public_text <mensaje>");
                return;
            }

            // Empezamos a crear el JSON que vamos a mandarle al servidor
            var publictext_json = new JsonObject();

            publictext_json["type"] = "PUBLIC_TEXT"; // Decimos que es de tipo PUBLIC_TEXT
            publictext_json["text"] = mensaje; // Decimos que el mensaje es el resto de la entrada

            // Por quinta vez y contandoooo mandamos el JSON
            string json_texto = publictext_json.ToJsonString();
            _networkService.Enviar(json_texto);
        }

        // Definición de
        // NEW_ROOM (del lado del cliente)
        // :)
        public void NewRoom(String roomname)
        {
            // Primero verifiquemos que el nombre no sea NULL o un espacio en blanco
            if(string.IsNullOrWhiteSpace(roomname))
            {
                _view.MostrarError("Debes proporcionar un nombre para el cuarto. Uso correcto: /new_room <nombre_del_cuarto>");
                return;
            }
            // Verificamos que el nombre sea de a lo mucho 16 caracteres
            if(roomname.Length <= 16)
            {
                // Ya que haya pasado esas dos verificaciones, ahora sí podemos empezar a trabajar
                var newroom_json = new JsonObject();

                newroom_json["type"] = "NEW_ROOM";
                newroom_json["roomname"] = roomname;

                // Por sexta vez siiiii mandamos el JSON
                string json_texto = newroom_json.ToJsonString();
                _networkService.Enviar(json_texto);
            } else
            {
                _view.MostrarError("El nombre de cuarto solo puede ser de máximo 16 caracteres");
            }
        }

        // Definición de
        // INVITE (del lado del cliente)
        // :)
        public void Invite(String entrada)
        {
            // Primero verificamos que no falte información importante
            if(string.IsNullOrWhiteSpace(entrada))
            {
                _view.MostrarError("Falta información. Uso correcto (la sala debe estar entre comillas): /invite <sala> <usuarios_a_invitar>");
                return;
            }

            // Primero vamos a partir la entrada usando comillas porque el nombre de la sala debe estar entre comillas
            string[] partes = entrada.Split('"');

            if(partes.Length < 3)
            {
                _view.MostrarError("Formato de comillas incorrecto. Uso correcto (la sala debe estar entre comillas): /invite <sala> <usuarios_a_invitar>");
                return;
            }

            string room_name = partes[1].Trim();
            string[] usuariosAEnviar = partes[2].Split(' ', StringSplitOptions.RemoveEmptyEntries);

            // Vamos a verificar rápido que por lo menos haya un usuario
            if(usuariosAEnviar.Length == 0)
            {
                _view.MostrarError("Debes proporcionar usuarios para invitar. Uso correcto (la sala debe estar entre comillas): /invite <sala> <usuarios_a_invitar>");
                return;
            }

            // Como ya verificamos que sí hay usuarios para invitar, hacemos un arreglo para almacenarlos
            var usernames = new JsonArray();

            for(int i = 0; i < usuariosAEnviar.Length; i++)
            {
                usernames.Add(usuariosAEnviar[i]);
            }

            // Por último, hacemos el JSON para mandarlo al servidor
            var invite_json = new JsonObject();

            invite_json["type"] = "INVITE";
            invite_json["roomname"] = room_name;
            invite_json["usernames"] = usernames;

            // Van siete veces que convertimos un json a string xd
            string json_texto = invite_json.ToJsonString();
            _networkService.Enviar(json_texto);
        }

        // Definición de
        // JOIN_ROOM
        // :)
        public void JoinRoom(String roomname)
        {
            // De nuevo verificamos que no falte información importante
            if(string.IsNullOrWhiteSpace(roomname))
            {
                _view.MostrarError("Falta información. Uso correcto: /join_room <sala>");
                return;
            }

            // La ventaja aquí es que, como la entrada es el nombre de la sala, no hay que partir nada
            var join_json = new JsonObject();

            join_json["type"] = "JOIN_ROOM";
            join_json["roomname"] = roomname;

            // Ocho veces mandando jsons siiiii
            string json_texto = join_json.ToJsonString();
            _networkService.Enviar(json_texto);
        }

        // Definición de
        // ROOM_USERS (del lado del cliente)
        // :)
        public void RoomUsers(String roomname)
        {
            // Otra vez verificamos que no falte información importante
            if(string.IsNullOrWhiteSpace(roomname))
            {
                _view.MostrarError("Falta información. Uso correcto: /room_users <sala>");
                return;
            }

            // Igual que con join_room, como solo es el nombre de la sala no hay que partir nada :D
            var room_users_json = new JsonObject();

            room_users_json["type"] = "ROOM_USERS";
            room_users_json["roomname"] = roomname;

            // VAN NUEVE veces que mandamos el json :D
            string json_texto = room_users_json.ToJsonString();
            _networkService.Enviar(json_texto);
        }

        // Ya estos métodos de aquí abajo sirve para escuchar las respuestas del servidor

        // Este método de aquí va a procesar los mensajes del servidor
        private void ProcesarMensajeServidor(string jsonRespuesta)
        {
            try
            {
                // Parseamos el JSON y obtenemos su raíz (pues los JSON son árboles si no me equivoco)
                var mensaje = JsonDocument.Parse(jsonRespuesta);
                var raiz = mensaje.RootElement;

                // Hacemos una pequeña comprobación por seguridad, en el mero caso de que no haya un "type" en el JSON
                if(raiz.TryGetProperty("type", out var type))
                {
                    string tipo = type.GetString() ?? "";

                    // En caso de que sea una respuesta
                    if(tipo == "RESPONSE")
                    {
                        // Guardamos la operación que se hizo y el resultado de la misma
                        string operation = raiz.GetProperty("operation").GetString() ?? "";
                        string result = raiz.GetProperty("result").GetString() ?? "";

                        if(result == "SUCCESS")
                        {
                            _view.MostrarInformacion($"Operación {operation} realizada exitosamente");
                            if(operation == "IDENTIFY") // Esta verificación de aquí es para decir que el usuario ya se identificó
                            {
                                _usuario.EstaIdentificado = true;
                            }
                        } else
                        {
                            _view.MostrarError($"Error en {operation}: {result}");
                        }
                    }
                    else if(tipo == "NEW_USER") // En caso de que sea una notificación de un nuevo usuario
                    {
                        string user = raiz.GetProperty("username").GetString() ?? "Anónimo";
                        _view.MostrarInformacion($"[Notificación]: {user} se ha conectado. ¡Dí hola!");
                    }
                    else if(tipo == "NEW_STATUS") // En caso de que sea una notificación de un nuevo estado de un usuario
                    {
                        string user = raiz.GetProperty("username").GetString() ?? "Anónimo";
                        string status = raiz.GetProperty("status").GetString() ?? "";
                        _view.MostrarInformacion($"[Notificación]: {user} ahora está {status}");
                    } 
                    else if(tipo == "USER_LIST") // En caso de que se haya solicitado la lista de usuarios
                    {
                        _view.MostrarInformacion("--- Lista de Usuarios Conectados ---");
                        if (raiz.TryGetProperty("users", out var users))
                        {
                            foreach (var userProp in users.EnumerateObject())
                            {
                                _view.MostrarMensaje($" • {userProp.Name} -> [{userProp.Value.GetString()}]");
                            }
                        }
                    }
                    else if(tipo == "TEXT_FROM") // En caso de que sea un mensaje privado
                    {
                        string emisor = raiz.GetProperty("username").GetString() ?? "Anónimo";
                        string message = raiz.GetProperty("text").GetString() ?? "";
                        _view.MostrarMensaje($"[Privado de: {emisor}]: {message}");
                    }
                    else if(tipo == "PUBLIC_TEXT_FROM"){
                        string emisor = raiz.GetProperty("username").GetString() ?? "Anónimo";
                        string message = raiz.GetProperty("text").GetString() ?? "";
                        _view.MostrarMensaje($"[Público de: {emisor}]: {message}");
                    }
                    else if(tipo == "INVITATION") // En caso de que sea una invitación
                    {
                        string emisor = raiz.GetProperty("username").GetString() ?? "Anónimo";
                        string roomname = raiz.GetProperty("roomname").GetString() ?? "";
                        _view.MostrarInformacion($"[Notificación]: ¡{emisor} te está invitando a la sala {roomname}!");
                    }
                    else if(tipo == "JOINED_ROOM") // En caso de que se haya aceptado la invitación
                    {
                        string user = raiz.GetProperty("username").GetString() ?? "Anónimo";
                        string roomname = raiz.GetProperty("roomname").GetString() ?? "";
                        _view.MostrarInformacion($"[Notificación]: ¡{user} acaba de entrar a {roomname}!");
                    }
                    else if(tipo == "ROOM_USER_LIST") // En caso de que se haya solicitado la lista de usuarios de una sala
                    {
                        string roomname = raiz.GetProperty("roomname").GetString() ?? "";

                        _view.MostrarInformacion($"--- Lista de Usuarios Conectados en {roomname} ---");
                        if (raiz.TryGetProperty("users", out var users))
                        {
                            foreach (var userProp in users.EnumerateObject())
                            {
                                _view.MostrarMensaje($" • {userProp.Name} -> [{userProp.Value.GetString()}]");
                            }
                        }
                    }
                    else
                    {
                        // Ya muestra cualquier cosa que haya mandado el servidor
                        _view.MostrarInformacion($"[Servidor]: {jsonRespuesta}");
                    }
                }
            } catch
            {
                // Voy a dejar que no haga nada en caso de que lo que se manda no sea un json
            }
        }

        // Este método de aquí es para manejar la desconexión de parte del servidor
        private void ManejarDesconexion()
        {
            _view.MostrarError("Se perdió la conexión con el servidor.");
        }
    }

}