#include <stdio.h>
#include <string.h>
#include <math.h>
#include "calculadora.h"

int main(void) {
    /* ----------------------------------------------------------------
       Testes da tabela do enunciado
       Nota: Teste 6 tem inconsistência no enunciado:
         "2 3 + 5 log /" avalia como (2+3)/log(5) = 7.15, não 0.14.
         Para obter 0.14 a posfixa correta seria "2 3 + log 5 /".
         Nossa implementação está matematicamente correta.
    ---------------------------------------------------------------- */
    struct {
        char posfixa[128];
        char infixa_esp[64];
        float valor_esp;
        float tolerancia;
    } testes[] = {
        { "3 4 + 5 *",          "(3+4)*5",           35.0f,   0.001f },
        { "7 2 * 4 +",          "7*2+4",             18.0f,   0.001f },
        { "8 5 2 4 + * +",      "8+5*(2+4)",         38.0f,   0.001f },
        { "6 2 / 3 + 4 *",      "(6/2+3)*4",         24.0f,   0.001f },
        { "9 5 2 8 * 4 + * +",  "9+5*(2*8+4)",      109.0f,   0.001f },
        { "2 3 + 5 log /",      "(2+3)/log(5)",       7.153f,  0.01f  }, /* enunciado inconsistente */
        { "10 log 3 ^ 2 +",     "log(10)^3+2",        3.0f,   0.001f },
        { "45 60 + 30 cos *",   "(45+60)*cos(30)",   90.93f,   0.01f  },
        { "0.5 45 sen 2 ^ +",   "0.5+sen(45)^2",      1.0f,   0.001f },
    };

    int n = sizeof(testes) / sizeof(testes[0]);
    printf("%-4s %-28s %-22s %-10s %-10s\n",
           "Tst", "PosFixa", "InFixa obtida", "Valor", "Status");
    printf("%-80s\n", "--------------------------------------------------------------------------------");

    int pass = 0;
    for (int i = 0; i < n; i++) {
        char buf[128], buf2[128];
        strcpy(buf,  testes[i].posfixa);
        strcpy(buf2, testes[i].posfixa);

        char  *infixa = getInFixa(buf);
        float  valor  = getValor(buf2);

        float diff = valor - testes[i].valor_esp;
        if (diff < 0) diff = -diff;

        int ok = infixa && diff <= testes[i].tolerancia;
        if (ok) pass++;

        printf("%-4d %-28s %-22s %-10.4f %s\n",
               i + 1,
               testes[i].posfixa,
               infixa ? infixa : "NULL (ERRO)",
               valor,
               ok ? "OK" : "FALHOU");
    }

    printf("\n%d/%d testes passaram.\n", pass, n);

    /* Testes de erro */
    printf("\n--- Testes de erro (devem retornar NAN/NULL) ---\n");

    char e1[] = "1 0 /";
    float v1 = getValor(e1);
    printf("Divisao por zero   [1 0 /]   -> %.4f  %s\n", v1, isnan(v1) ? "OK" : "FALHOU");

    char e2[] = "-1 raiz";
    float v2 = getValor(e2);
    printf("Raiz de negativo   [-1 raiz] -> %.4f  %s\n", v2, isnan(v2) ? "OK" : "FALHOU");

    char e3[] = "0 log";
    float v3 = getValor(e3);
    printf("Log de zero        [0 log]   -> %.4f  %s\n", v3, isnan(v3) ? "OK" : "FALHOU");

    char e4[] = "3 4";
    char *r4 = getInFixa(e4);
    printf("Operandos demais   [3 4]     -> %s  %s\n",   r4 ? r4 : "NULL", !r4 ? "OK" : "FALHOU");

    char e5[] = "+";
    char *r5 = getInFixa(e5);
    printf("Sem operandos      [+]       -> %s  %s\n",   r5 ? r5 : "NULL", !r5 ? "OK" : "FALHOU");

    char e6[] = "5 0 %";
    float v6 = getValor(e6);
    printf("Modulo por zero    [5 0 %%]  -> %.4f  %s\n", v6, isnan(v6) ? "OK" : "FALHOU");

    return 0;
}