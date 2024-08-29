#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "generadorUser.h"

char generarVocal() {
    char vocales[] = {'a', 'e', 'i', 'o', 'u'};
    int indice = rand() % 5;
    return vocales[indice];
}

char generarConsonante() {
    char consonantes[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'};
    int indice = rand() % 21;
    return consonantes[indice];
}

char* generarNombreUsuario(int longitud) {
    if (longitud < 5 || longitud > 15) {
        return GENUSER_ERROR_LONGITUD;
    }

    srand(time(NULL));
    char* nombreUsuario = (char*)malloc((longitud + 1) * sizeof(char));
    if (nombreUsuario == NULL) {
        return GENUSER_ERROR_MEMORIA;
    }

    int iniciarConVocal = rand() % 2;

    for (int i = 0; i < longitud; i++) {
        if (i % 2 == 0) {
            nombreUsuario[i] = iniciarConVocal ? generarVocal() : generarConsonante();
        } else {
            nombreUsuario[i] = iniciarConVocal ? generarConsonante() : generarVocal();
        }
    }

    nombreUsuario[longitud] = '\0';
    return nombreUsuario;
}
