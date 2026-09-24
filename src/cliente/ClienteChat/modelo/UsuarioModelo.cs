// Este programa de aquí solo servirá para darle una estructura a un usuario
namespace ClienteChat.modelo
{
    public class UsuarioModelo
    {
        public string Username {get; set;} = string.Empty; // Iniciamos el username como un string pero vacío
        public string Status {get; set;} = "ACTIVE"; // Iniciamos el estado del usuario por default en ACTIVE
        public bool EstaIdentificado {get; set;} = false; // Esto solo lo usaremos para ver que sí esté identificado
    }
}