#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#ifndef NARGS
#define NARGS 4
#endif

void manejo_error_fork(pid_t id_proceso)
{
    if (id_proceso < 0)
    {
        perror("Error al iniciar el fork");
        exit(-1);
    }
}

void manejo_error_execvp()
{
    perror("Error al ejecutar execvp");
    exit(-1);
}

void ejecutar_comando(char *parametros[], char *comando, int i, int cantidad)
{
    parametros[i + 1] = NULL;

    pid_t id_proceso = fork();
    manejo_error_fork(id_proceso);

    if (id_proceso == 0)
    {
        execvp(comando, parametros);
        // Si llega aca, hubo un error
        manejo_error_execvp();
    }
    else
    {
        // Se queda esperando a que el hijo termine de ejecutar el comando
        wait(NULL);
    }
    for (int j = 0; j < cantidad; j++)
    {
        free(parametros[j + 1]);
    }
}

void xargs(char *parametros[], char *comando)
{
    int cant_parametros_leidos = 0;
    char *buffer = NULL;
    size_t size = 0;

    while (getline(&buffer, &size, stdin) != -1)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        parametros[cant_parametros_leidos + 1] = strdup(buffer);
        cant_parametros_leidos++;
        if (cant_parametros_leidos == NARGS)
        {
            ejecutar_comando(parametros, comando, cant_parametros_leidos, NARGS);
            cant_parametros_leidos = 0;
        }
    }
    // Si luego de terminar de leer, todavia quedaron parametros sin mostrar
    if (cant_parametros_leidos > 0)
    {
        ejecutar_comando(parametros, comando, cant_parametros_leidos, cant_parametros_leidos);
    }
    free(buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("No se envio la cantidad correcta (2) de parametros.");
        exit(-1);
    }

    char *parametros[NARGS + 2];
    char *comando;

    parametros[0] = argv[1];
    comando = argv[1];

    xargs(parametros, comando);

    return 0;
}
