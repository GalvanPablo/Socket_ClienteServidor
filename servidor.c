// clear && gcc -o servidor servidor.c generadores/generadorUser.c generadores/generadorPasswd.c -lws2_32 && ./servidor.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h> // Manejo de Sockets

#include <windows.h> // Manejo de acentos

#include "generadores/generadorUser.h"
#include "generadores/generadorPasswd.h"

// CONFIGURACIÓN DE CONEXIÓN
#define IP "127.0.0.1"
#define PUERTO 55555

// CODIGO DE ERRORES QUE MANEJA
/*
    200 - Todo bien!
    400 - La solicitud tiene sintaxis incorrecta "Acción|parametro"
    404 - No se encuentra dicha solicitud (no existe un acción para responder)
    422 - El parametro esta mal ya sea porque no es del tipo nescesario o esta fuera de rango
    500 - Error desconocido
*/

// Inicialización y creación del socket
SOCKET inicializar_socket() {
    WSADATA wsaData;
    int wserr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wserr != 0) {
        printf("Error al inicializar WinSock\n");
        return INVALID_SOCKET;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Error al crear el socket\n");
        WSACleanup();
        return INVALID_SOCKET;
    }
    printf("Socket inicializado!\n");
    return serverSocket;
}

// Asociación de IP y Puerto
int asociar_ip_puerto(SOCKET serverSocket, char* ip, int puerto) {
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(puerto);

    if (bind(serverSocket, (struct sockaddr*)&service, sizeof(service)) == SOCKET_ERROR) {
        printf("Error al asociar IP y Puerto\n");
        return 0;
    }
    printf("Asociación de IP y Puerto realizada correctamente.\n");
    return 1;
}

// Escucha de conexiones
int escuchar_conexiones(SOCKET serverSocket) {
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        printf("Error al escuchar en el socket\n");
        return 0;
    }
    printf("\nServidor escuchando...\n");
    return 1;
}

// Aceptar conexiones
SOCKET aceptar_conexion(SOCKET serverSocket) {
    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        printf("Error al aceptar conexión\n");
        return INVALID_SOCKET;
    }
    printf("Conexión establecida.\n");
    return acceptSocket;
}

// Recepción de datos
char* recibir_datos(SOCKET acceptSocket) {
    char* receiveBuffer = (char*)malloc(201 * sizeof(char));
    memset(receiveBuffer, 0, 201); // Inicializar buffer con ceros
    int rbyteCount = recv(acceptSocket, receiveBuffer, 200, 0);
    if (rbyteCount < 0) {
        printf("Sin datos recibidos\n");
        free(receiveBuffer);
        return NULL;
    }
    return receiveBuffer;
}

// Envío de datos
void enviar_datos(SOCKET acceptSocket, char* mensaje) {
    int sbyteCount = send(acceptSocket, mensaje, strlen(mensaje), 0);
    if (sbyteCount == SOCKET_ERROR) {
        printf("Error al enviar datos\n");
        return;
    }
}

// MANEJO DE MENSAJES RECIBIDOS
char* obtener_accion(const char *cadena) {
    char *ptr = strtok((char*)cadena, "|");
    if (ptr == NULL) {
        return "default";
    }
    return ptr;
}


// EJECUCIÓN
int main() {
    SetConsoleOutputCP(CP_UTF8); // Esto es para visualizar correctamente los acentos

    // MANEJO DE SOCKET
    SOCKET serverSocket = inicializar_socket();
    if (serverSocket == INVALID_SOCKET) {
        return 1;
    }

    // ASOCIACION DE IP Y PUERTO AL SOCKET
    if (!asociar_ip_puerto(serverSocket, IP, PUERTO)) {
        return 1;
    }

    int run = 1;
    while (run) {
        // ACTIVAR ESCUCHA DE CONEXIONES
        if (!escuchar_conexiones(serverSocket)) {
            printf("Error al intentar activar la escucha!\nFinalizando...\n");
            return 1;
        }

        // ACEPTAR CONEXIONES
        SOCKET acceptSocket = aceptar_conexion(serverSocket);
        if (acceptSocket == INVALID_SOCKET) {
            continue; // Si no se puede aceptar la conexión, seguir escuchando
        }

        // INTERCAMBIO DE MENSAJES CON EL CLIENTE
        int conn_run = 1;
        char respuesta[200];
        char* mensajeRecibido;
        while (conn_run) {
            mensajeRecibido = recibir_datos(acceptSocket);
            memset(respuesta, 0, sizeof(respuesta));
            if (mensajeRecibido != NULL) {
                // Procesar el mensaje recibido
                printf("\nCliente: %s\n", mensajeRecibido);

                char *accion = obtener_accion(mensajeRecibido);
                char *parametro = strtok(NULL, "|");

                if (strcmp(accion, "gen_user") == 0) {
                    printf("Acción 'gen_user' seleccionada\n");
                    int longUser = atoi(parametro);

                    char* nombreUsuario = generarNombreUsuario(longUser);
                    if (nombreUsuario != NULL) {
                        if(strcmp(nombreUsuario, GENUSER_ERROR_LONGITUD) == 0){
                            sprintf(respuesta, "422|La longitud esta por fuera del rango (5-15)");
                        } else if(strcmp(nombreUsuario, GENUSER_ERROR_MEMORIA) == 0){
                            sprintf(respuesta, "500|Error al asignar memoria en servidor");
                        } else {
                            sprintf(respuesta, "200|%s", nombreUsuario);
                            free(nombreUsuario); // Liberar memoria después de usarla
                        }
                    } else {
                        sprintf(respuesta, "500|No se ha podido generar el nombre de usuario");
                    }
                } else if (strcmp(accion, "gen_passwd") == 0) {
                    printf("Acción 'gen_passwd' seleccionada\n");
                    int longPasswd = atoi(parametro);

                    char* contrasena = generarContrasena(longPasswd);
                    if (contrasena != NULL) {
                        if(strcmp(contrasena, GENPASSWD_ERROR_LONGITUD) == 0){
                            sprintf(respuesta, "422|La longitud esta por fuera del rango (8-50)");
                        } else if(strcmp(contrasena, GENPASSWD_ERROR_MEMORIA) == 0){
                            sprintf(respuesta, "500|Error al asignar memoria en servidor");
                        } else {
                            sprintf(respuesta, "200|%s", contrasena);
                            free(contrasena); // Liberar memoria después de usarla
                        }
                    } else {
                        sprintf(respuesta, "500|No se ha podido generar la contraseña");
                    }

                } else {
                    sprintf(respuesta, "404|Acción desconocida o no especificada");
                }
                enviar_datos(acceptSocket, respuesta);
                free(mensajeRecibido); // Liberar memoria
            } else {
                // Si no se recibió mensaje, es probable que el cliente haya cerrado la conexión
                printf("Conexión cerrada por el cliente\n");
                conn_run = 0;
            }
        }

        // Cerrar conexiones
        closesocket(acceptSocket);
    }

    // Cerrar conexiones
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}