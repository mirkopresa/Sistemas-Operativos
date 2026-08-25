#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void cerrar_pipes(int fd_padre_hijo, int fd_hijo_padre)
{
    close(fd_padre_hijo);
    close(fd_hijo_padre);
}

void pingpong(int fd_padre_hijo[2], int fd_hijo_padre[2])
{
    pid_t id_proceso = fork();

    if (id_proceso < 0)
    {
        perror("Error al ejecutar un fork\n");
        exit(0);
    }

    if (id_proceso == 0)
    {
        // hijo
        cerrar_pipes(fd_padre_hijo[1], fd_hijo_padre[0]);

        long entero_leido;
        read(fd_padre_hijo[0], &entero_leido, sizeof(long));
        printf("getpid me devuelve: %d\n", getpid());
        printf("getppid me devuelve: %d\n", getppid());
        printf("Recibo valor %ld via fd = %d\n", entero_leido, fd_padre_hijo[0]);
        printf("Reenvio valor en fd = %d y termino\n\n", fd_hijo_padre[1]);
        write(fd_hijo_padre[1], &entero_leido, sizeof(long));

        cerrar_pipes(fd_padre_hijo[0], fd_hijo_padre[1]);
    }
    else
    {
        // padre
        cerrar_pipes(fd_padre_hijo[0], fd_hijo_padre[1]);

        printf("getpid me devuelve: %d\n", getpid());
        printf("getppid me devuelve: %d\n", getppid());
        long entero_aleatorio = random();
        printf("Valor random: %ld\n", entero_aleatorio);
        printf("Envio valor %ld a traves del fd = %d\n\n", entero_aleatorio, fd_padre_hijo[1]);
        write(fd_padre_hijo[1], &entero_aleatorio, sizeof(long));
        long devuelto;
        read(fd_hijo_padre[0], &devuelto, sizeof(long));
        printf("Hola, de nuevo PID: %d\n", getpid());
        printf("Recibi valor %ld via fd = %d\n", devuelto, fd_hijo_padre[0]);

        wait(NULL);
        cerrar_pipes(fd_padre_hijo[1], fd_hijo_padre[0]);
    }
}

int main()
{
    int fd_padre_hijo[2];
    int fd_hijo_padre[2];

    printf("Hola, soy PID: %d\n", getpid());

    int error = pipe(fd_padre_hijo);
    int error_2 = pipe(fd_hijo_padre);

    if (error < 0)
    {
        printf("Error al crear el pipe de padre a hijo.\n");
        exit(0);
    }
    if (error_2 < 0)
    {
        printf("Error al crear el pipe de hijo a padre.\n");
        exit(0);
    }

    printf("IDs del primer pipe: [%d, %d]\n", fd_padre_hijo[0], fd_padre_hijo[1]);
    printf("IDs del segundo pipe: [%d, %d]\n\n", fd_hijo_padre[0], fd_hijo_padre[1]);

    srandom(time(NULL));
    pingpong(fd_padre_hijo, fd_hijo_padre);
    return 0;
}