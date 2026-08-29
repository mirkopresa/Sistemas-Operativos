#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void manejo_error_fork(pid_t id_proceso)
{
    if (id_proceso < 0)
    {
        perror("Error al iniciar el fork");
        exit(-1);
    }
}

void manejo_error_pipe(int error)
{
    if (error < 0)
    {
        perror("Error al crear un pipe");
        exit(-1);
    }
}

void manejo_error_lectura_escritura(ssize_t resultado)
{
    if (resultado < 0)
    {
        perror("Error al leer/escribir un numero");
        exit(-1);
    }
}

void filtrado_recursivo(int pipe_lectura, int nuevo_pipe_escritura, int leido_anterior)
{
    int leido_nuevo;
    ssize_t resultado;

    resultado = read(pipe_lectura, &leido_nuevo, sizeof(int));
    manejo_error_lectura_escritura(resultado);

    // Caso base: no quedan mas numeros en el pipe
    if (resultado == 0)
    {
        return;
    }

    if (leido_nuevo % leido_anterior != 0)
    {
        resultado = write(nuevo_pipe_escritura, &leido_nuevo, sizeof(int));
        manejo_error_lectura_escritura(resultado);
    }
    filtrado_recursivo(pipe_lectura, nuevo_pipe_escritura, leido_anterior);
}

void filtrado(int pipe_lectura)
{
    int leido;
    ssize_t resultado;

    resultado = read(pipe_lectura, &leido, sizeof(int));
    manejo_error_lectura_escritura(resultado);

    if (resultado == 0)
    {
        close(pipe_lectura);
        return;
    }

    printf("primo %d\n", leido);
    fflush(stdout);

    int nuevo_pipe[2];
    int error;

    error = pipe(nuevo_pipe);
    manejo_error_pipe(error);

    pid_t id_proceso = fork();
    manejo_error_fork(id_proceso);

    if (id_proceso > 0)
    {
        close(nuevo_pipe[0]);
        filtrado_recursivo(pipe_lectura, nuevo_pipe[1], leido);
        close(pipe_lectura);
        close(nuevo_pipe[1]);
        wait(NULL);
    }
    else
    {
        close(nuevo_pipe[1]);
        close(pipe_lectura);
        filtrado(nuevo_pipe[0]);
    }
}

void primes(int n)
{
    int pipe_izq_der[2];
    int error;
    ssize_t resultado;

    error = pipe(pipe_izq_der);
    manejo_error_pipe(error);

    pid_t id_proceso = fork();
    manejo_error_fork(id_proceso);

    if (id_proceso > 0)
    {
        close(pipe_izq_der[0]);
        for (int i = 2; i <= n; i++)
        {
            resultado = write(pipe_izq_der[1], &i, sizeof(int));
            manejo_error_lectura_escritura(resultado);
        }
        close(pipe_izq_der[1]);
        wait(NULL);
    }
    else
    {
        close(pipe_izq_der[1]);
        filtrado(pipe_izq_der[0]);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("No se envio la cantidad correcta (2) de parametros.");
        exit(-1);
    }
    int n = atoi(argv[1]);
    if (n == 0)
    {
        perror("Error al convertir de string a numero.");
        exit(-1);
    }
    if (n < 2)
    {
        perror("El numero tiene que ser mayor o igual a 2.");
        exit(-1);
    }
    primes(n);
    return 0;
}