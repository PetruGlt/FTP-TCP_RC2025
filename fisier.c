#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_OPERAND 100

void generare_fiu1(int N) {
    int fd = open("operanzi.bin", O_WRONLY | O_CREAT, 0644);
    for (int i = 0; i < N; i++) {
        int op1 = rand() % MAX_OPERAND;
        int op2 = rand() % MAX_OPERAND;
        write(fd, &op1, sizeof(op1));
        write(fd, &op2, sizeof(op2));
    }
    close(fd);
}

void generare_fiu2(int N) {
    FILE *fp = fopen("operatori.txt", "w");
    char operators[] = {'+', '-', '*', '/'};
    for (int i = 0; i < N; i++) {
        fputc(operators[rand() % 4], fp);
    }
    fclose(fp);
}

void calcul_tata(int N) {
    int fd = open("operanzi.bin", O_RDONLY);
    FILE *fp = fopen("operatori.txt", "r");
    for (int i = 0; i < N; i++) {
        int op1, op2;
        char operator;
        read(fd, &op1, sizeof(op1));
        read(fd, &op2, sizeof(op2));
        operator = fgetc(fp);
        switch (operator) {
            case '+':
                printf("%d + %d = %d\n", op1, op2, op1 + op2);
                break;
            case '-':
                printf("%d - %d = %d\n", op1, op2, op1 - op2);
                break;
            case '*':
                printf("%d * %d = %d\n", op1, op2, op1 * op2);
                break;
            case '/':
                if (op2 != 0)
                    printf("%d / %d = %d\n", op1, op2, op1 / op2);
                else
                    printf("Divide by zero error.\n");
                break;
        }
    }
    close(fd);
    fclose(fp);
}

int N=0;

int main()
{
	pid_t pid_fiu1, pid_fiu2;

	printf("Dati numarul intreg N:");
	scanf("%d", &N); // TODO: tratați excepțiile de citire a unui număr întreg


	/* Crearea procesului fiu #1. */
	if(-1 == (pid_fiu1=fork()) )
	{
		perror("Eroare la fork #1");  return 1;
	}

	/* Ramificarea execuției după primul apel fork. */
	if(pid_fiu1 == 0)
	{   /* Zona de cod executată doar de către fiul #1. */
		printf("\n[P1] Procesul fiu 1, cu PID-ul: %d.\n", getpid());

		generare_fiu1(N);
		return 0;
	}
	else
	{   /* Zona de cod executată doar de către părinte. */
		printf("\n[P0] Procesul tata, cu PID-ul: %d.\n", getpid());

		/* Crearea procesului fiu #2. */
		if(-1 == (pid_fiu2=fork()) )
		{
			perror("Eroare la fork #2");  return 2;
		}
		
		/* Ramificarea execuției după al doilea apel fork. */
		if(pid_fiu2 == 0)
		{   /* Zona de cod executată doar de către fiul #2. */
			printf("\n[P2] Procesul fiu 2, cu PID-ul: %d.\n", getpid());

			generare_fiu2(N);
			return 0;
		}
		else
		{   /* Zona de cod executată doar de către părinte. */

			wait(NULL);
			wait(NULL);
			// Mai întâi aștept terminarea ambilor fii, iar apoi fac calculul.

			calcul_tata(N);
			return 0;
		}
	}
}

