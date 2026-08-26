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

void manejo_error_lectura_escritura(ssize_t resultado)
{
    if (resultado < 0)
    {
        perror("Error al leer/escribir un numero");
        exit(0);
    }
}

void manejo_error_pipe(int error)
{
    if (error < 0)
    {
        perror("Error al crear un pipe");
        exit(0);
    }
}

void pingpong(int fd_padre_hijo[2], int fd_hijo_padre[2])
{
    pid_t id_proceso = fork();
    ssize_t resultado;

    if (id_proceso < 0)
    {
        perror("Error al ejecutar un fork\n");
        exit(0);
    }

    if (id_proceso == 0)
    {
        cerrar_pipes(fd_padre_hijo[1], fd_hijo_padre[0]);

        long entero_leido;
        resultado = read(fd_padre_hijo[0], &entero_leido, sizeof(long));
        manejo_error_lectura_escritura(resultado);

        printf("getpid me devuelve: %d\n", getpid());
        printf("getppid me devuelve: %d\n", getppid());
        printf("Recibo valor %ld via fd = %d\n", entero_leido, fd_padre_hijo[0]);
        printf("Reenvio valor en fd = %d y termino\n\n", fd_hijo_padre[1]);

        resultado = write(fd_hijo_padre[1], &entero_leido, sizeof(long));
        manejo_error_lectura_escritura(resultado);

        cerrar_pipes(fd_padre_hijo[0], fd_hijo_padre[1]);
    }
    else
    {
        cerrar_pipes(fd_padre_hijo[0], fd_hijo_padre[1]);

        printf("getpid me devuelve: %d\n", getpid());
        printf("getppid me devuelve: %d\n", getppid());

        long entero_aleatorio = random();
        printf("Valor random: %ld\n", entero_aleatorio);
        printf("Envio valor %ld a traves del fd = %d\n\n", entero_aleatorio, fd_padre_hijo[1]);

        resultado = write(fd_padre_hijo[1], &entero_aleatorio, sizeof(long));
        manejo_error_lectura_escritura(resultado);

        long devuelto;
        resultado = read(fd_hijo_padre[0], &devuelto, sizeof(long));
        manejo_error_lectura_escritura(resultado);

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
    int error;

    printf("Hola, soy PID: %d\n", getpid());

    error = pipe(fd_padre_hijo);
    manejo_error_pipe(error);
    error = pipe(fd_hijo_padre);
    manejo_error_pipe(error);

    printf("IDs del primer pipe: [%d, %d]\n", fd_padre_hijo[0], fd_padre_hijo[1]);
    printf("IDs del segundo pipe: [%d, %d]\n\n", fd_hijo_padre[0], fd_hijo_padre[1]);

    srandom(time(NULL));
    pingpong(fd_padre_hijo, fd_hijo_padre);
    return 0;
}