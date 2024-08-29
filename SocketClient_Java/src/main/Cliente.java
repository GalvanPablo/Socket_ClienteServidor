package main;
import java.io.*;
import java.net.*;
import java.util.*;

public class Cliente {
	private static Socket socket;

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        boolean run = true;

        conectarAlServidor(); // Conectar al servidor antes de enviar peticiones

        while (run) {
            System.out.println("Menú:");
            System.out.println("1. Generar un usuario");
            System.out.println("2. Generar una contraseña");
            System.out.println("3. Salir");
            System.out.print("Ingrese una opción: ");
            int opcion = scanner.nextInt();

            switch (opcion) {
                case 1:
                    System.out.print("\nIndicar una longitud para el nombre de usuario (5-15): ");
                    int longUser = scanner.nextInt();
                    enviarPeticion("gen_user|" + longUser);
                    break;
                case 2:
                    System.out.print("\nIndicar una longitud para la contraseña (8-50): ");
                    int longPasswd = scanner.nextInt();
                    enviarPeticion("gen_passwd|" + longPasswd);
                    break;
                case 3:
                    System.out.println("");
                    run = false;
                    desconectarDelServidor();
                    break;
                default:
                    System.out.println("Opción inválida");
            }
            limpiar();
        }
    }

    public static void inicializarSocket() {
        try {
            socket = new Socket("127.0.0.1", 55555);
            System.out.println("Socket inicializado!");
        } catch (UnknownHostException e) {
            System.err.println("Host desconocido");
        } catch (IOException e) {
            System.err.println("Error al inicializar el socket");
        }
    }

    public static void conectarAlServidor() {
        inicializarSocket();
        if (socket == null) {
            System.err.println("No se pudo conectar al servidor");
            return;
        }
        System.out.println("Conexión establecida!\n\n");
    }

    public static void enviarPeticion(String mensaje) {
        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(mensaje.getBytes());
//            System.out.println("Peticion enviada!");
            recibirRespuesta();
        } catch (IOException e) {
            System.err.println("Error al enviar la peticion");
        }
    }

    public static void recibirRespuesta() {
        try {
            InputStream inputStream = socket.getInputStream();
            byte[] buffer = new byte[200];
            int bytesRecibidos = inputStream.read(buffer);
            String respuesta = new String(buffer, 0, bytesRecibidos);
//            System.out.println("Servidor: " + respuesta);
            procesarRespuesta(respuesta);
        } catch (IOException e) {
            System.err.println("Error al recibir la respuesta");
        }
    }

    public static void procesarRespuesta(String respuesta) {
        String[] partes = respuesta.split("\\|");
        int codigo = Integer.parseInt(partes[0]);
        String mensaje = partes[1];
        switch (codigo) {
            case 200:
                System.out.println("Operación exitosa: " + mensaje);
                break;
            case 422:
                System.out.println("Error en la longitud: " + mensaje);
                break;
            default:
                System.out.println("Se ha generado un error del lado del servidor");
                break;
        }
    }

    public static void desconectarDelServidor() {
        try {
            socket.close();
            System.out.println("Conexión terminada!");
        } catch (IOException e) {
            System.err.println("Error al cerrar la conexión");
        }
    }
    
    public static void limpiar() {
        for (int i = 0; i < 2; i++) {
			System.out.println();
		}
    }
    
}