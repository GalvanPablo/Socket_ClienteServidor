#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "generadorPasswd.h"

char generarCaracterAleatorio() {
    int tipo = rand() % 3;

    if (tipo == 0)
        return '0' + (rand() % 10);  // Número
    else if (tipo == 1)
        return 'a' + (rand() % 26);  // Letra minúscula
    else
        return 'A' + (rand() % 26);  // Letra mayúscula
}

char* generarContrasena(int longitud) {
    if (longitud < 8 || longitud > 50) {
        return GENPASSWD_ERROR_LONGITUD;
    }

    srand(time(NULL));
    char* contrasena = (char*)malloc((longitud + 1) * sizeof(char));
    if (contrasena == NULL) {
        return GENPASSWD_ERROR_MEMORIA;
    }

    for (int i = 0; i < longitud; i++) {
        contrasena[i] = generarCaracterAleatorio();
    }

    contrasena[longitud] = '\0';
    return contrasena;
}
