using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

// Esta clase va a actuar como un puente de comunicación TCP entre el cliente y el servidor
namespace ClienteChat.servicios
{
    public class NetworkService
    {
        private TcpClient _client; // Esto abre la conexión TCP entre el cliente y el servidor
        private NetworkStream _stream; // Esto es un flujo de transporte por donde pasan los datos entre ambos extremos
        private StreamReader _reader; // Esto y el _writer convierten los bytes que viajan por el _stream en texto UTF-8
        private StreamWriter _writer;

        public event Action<string>? OnMessageReceived; // Este evento solo notifica cuando recibe un mensaje
        public event Action? OnDisconnected; // Este evento solo notifica cuando el servidor se desconectó por X o Y razón

        // Este método te conecta al servidor
        public bool Conectar(string ip, int puerto)
        {
            try
            {
                _client = new TcpClient(ip, puerto);
                _stream = _client.GetStream();
                _reader = new StreamReader(_stream, Encoding.UTF8);
                _writer = new StreamWriter(_stream, Encoding.UTF8) { AutoFlush = true}; // El AutoFlush manda inmediatamente el texto al socket

                // Aquí iniciamos un hilo de ejecución secundario que se va a encargar de escuchar al servidor si manda un mensaje en cualquier momento
                Task.Run(EscucharServidor);
                return true;
            } catch (Exception e)
            {
                return false;
            }
        }

        // Este método de aquí se va a encargar de mandar JSON en forma de string
        public void Enviar(string jsonTexto)
        {
            if(_writer != null)
            {
                // Enviamos la cadena
                _writer.WriteLine(jsonTexto);
            }
        }

        // Este método de aquí se va a encargar de escuchar todo lo que le mande el servidor
        private void EscucharServidor()
        {
            try
            {
                string? linea;
                while ((linea = _reader.ReadLine()) != null)
                {
                    // Le notificamos a nuestro evento que llegó un mensaje del servidor
                    OnMessageReceived?.Invoke(linea);
                }
            } catch
            {
                // Se interrumpió la conexión
            }
            finally
            {
                // Le notificamos a nuestro evento que se desconectó el servidor
                OnDisconnected?.Invoke();
            }
        }

        // Este método de aquí solo te desconectará totalmente del servidor
        public void Desconectar()
        {
            _reader?.Close();
            _writer?.Close();
            _stream?.Close();
            _client?.Close();
        }
    }
}