// clear && gcc -o cliente cliente.c -lws2_32 && ./cliente.exe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h> // Manejo de Sockets

#include <windows.h> // Manejo de acentos

// CONFIGURACIÓN DE CONEXIÓN
#define IP "127.0.0.1"
#define PUERTO 55555
#define MAX_INTENTOS 3
#define RETARDO 2000 // 2 segundos

// Inicialización y creación del socket
SOCKET inicializar_socket() {
    printf("Inicializando Socket...\n");
    WSADATA wsaData;
    int wserr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wserr != 0) {
        printf("Error al inicializar WinSock\n");
        return INVALID_SOCKET;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Error al crear el socket\n");
        WSACleanup();
        return INVALID_SOCKET;
    }
    printf("Socket inicializado!\n");
    return clientSocket;
}

// Conexión al servidor
int conectar_al_servidor(SOCKET clientSocket, char* ip, int puerto) {
    int intentos = 0;
    printf("Iniciando conexión...\n");

    while (intentos < MAX_INTENTOS) {
        struct sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_addr.s_addr = inet_addr(ip);
        clientService.sin_port = htons(puerto);

        if (connect(clientSocket, (struct sockaddr*)&clientService, sizeof(clientService)) != SOCKET_ERROR) {
            printf("Conexión establecida!\n\n");
            return 1;
        }

        printf("Intento %d/%d fallido. Reintentando en %d segundos...\n", intentos + 1, MAX_INTENTOS, RETARDO / 1000);
        Sleep(RETARDO);
        intentos++;
    }

    printf("\nNo se pudo conectar al servidor después de %d intentos.\nConexión canelada!\n", MAX_INTENTOS);
    return 0;
}

// Desconexión del servidor
void desconectar_del_servidor(SOCKET clientSocket) {
    printf("Cerrando conexión...\n");
    closesocket(clientSocket);
    WSACleanup();
    printf("Conexión terminada!\n");
}

// Envío de datos
int enviar_datos(SOCKET clientSocket, char* buffer) {
    int sbyteCount = send(clientSocket, buffer, strlen(buffer), 0);
    if (sbyteCount == SOCKET_ERROR) {
        printf("Error al enviar datos\n");
        return 0;
    }

    return 1;
}

// Recepción de datos
int recibir_datos(SOCKET clientSocket, char* receiveBuffer) {
    int rbyteCount = recv(clientSocket, receiveBuffer, 200, 0);
    if (rbyteCount < 0) {
        printf("Error al recibir datos\n");
        return 0;
    }

    return 1;
}

int obtener_codigo(const char *cadena) {
    char *ptr = strtok((char*)cadena, "|");
    if (ptr == NULL) {
        return -1; // o algún otro valor por defecto
    }
    return atoi(ptr); // Convertir el string a un entero
}
char* obtener_mensaje(const char *cadena) {
    return strtok(NULL, "|");
}

void msjFinalizado(){
    printf("\n\nCliente Finalizado!");
    getchar();
}


// EJECUCIÓN
int main()
{
    SetConsoleOutputCP(CP_UTF8); // Esto es para visualizar correctamente los acentos

    // MANEJO DE SOCKET
    SOCKET clientSocket = inicializar_socket();
    if (clientSocket == INVALID_SOCKET) {
        msjFinalizado();
        return 1;
    }
    printf("\n");

    // INICIO DE CONEXIÓN
    if (!conectar_al_servidor(clientSocket, IP, PUERTO)) {
        msjFinalizado();
        return 1;
    }

    // ACCIONES
    system("pause");
    char bufferPeticion[200];
    char bufferRespuesta[200];
    int run = 1;
    int opcion = 0;
    while(run){
        // Limpiar buffers
        memset(bufferPeticion, 0, sizeof(bufferPeticion));
        memset(bufferRespuesta, 0, sizeof(bufferRespuesta));

        if(opcion == 0){ // Si es 0 es que tiene que selecionar otra opcion y no continua en el mismo punto del menu
            system("clear");
            printf("Menú:\n");
            printf("1. Generar un usuario\n");
            printf("2. Generar una contraseña\n");
            printf("3. Salir\n");
            printf("Ingrese una opción: ");
            scanf("%d", &opcion);
        }

        switch (opcion) {
        case 1:
            system("clear");
            int longUser;
            printf("Generar usuario\nIndicar una longitud (5-15): ");
            fflush(stdin);
            scanf("%d", &longUser);
            // ENVIAR PETICIÓN AL SERVIDOR
            sprintf(bufferPeticion, "gen_user|%d\0", longUser);
            if (!enviar_datos(clientSocket, bufferPeticion)) {
                printf("Error al enviar la petición!");
            } else {
                if (!recibir_datos(clientSocket, bufferRespuesta)) {
                    printf("Error al recibir la respuesta!");
                } else {
                    // printf("Servidor: %s\n", bufferRespuesta);
                    int codigo = obtener_codigo(bufferRespuesta);
                    char *mensaje = obtener_mensaje(bufferRespuesta);
                    switch (codigo) {
                    case 200:
                        printf("Usuario generado: %s", mensaje);
                        opcion = 0;
                        break;
                    case 422:
                        printf("Error en la longitud: %s", mensaje);
                        break;
                    default:
                        printf("Se ha generado un error del lado del servidor");
                        break;
                    }
                }
            }
            printf("\n");
            system("pause");
            break;
        case 2:
            system("clear");
            int longPasswd;
            printf("Generar usuario\nIndicar una longitud (8-50):");
            fflush(stdin);
            scanf("%d", &longPasswd);
            // ENVIAR PETICIÓN AL SERVIDOR
            sprintf(bufferPeticion, "gen_passwd|%d", longPasswd);
            if (!enviar_datos(clientSocket, bufferPeticion)) {
                printf("Error al enviar la petición!");
            } else {
                if (!recibir_datos(clientSocket, bufferRespuesta)) {
                    printf("Error al recibir la respuesta!");
                } else {
                    // printf("Servidor: %s\n", bufferRespuesta);
                    int codigo = obtener_codigo(bufferRespuesta);
                    char *mensaje = obtener_mensaje(bufferRespuesta);
                    switch (codigo) {
                    case 200:
                        printf("Contraseña generado: %s", mensaje);
                        opcion = 0;
                        break;
                    case 422:
                        printf("Error en la longitud: %s", mensaje);
                        break;
                    default:
                        printf("Se ha generado un error del lado del servidor");
                        break;
                    }
                }
            }
            printf("\n");
            system("pause");
            break;
        case 3:
            run = 0;
            // CERRAR CONEXIÓN
            desconectar_del_servidor(clientSocket);
            break;
        
        default:
            opcion = 0;
            printf("\nError - opcion invalida!\nVuelva a intentar!\n");
            system("pause");
            break;
        }
    }

    // FIN DEL CLIENTE
    msjFinalizado();
    return 0;
}
