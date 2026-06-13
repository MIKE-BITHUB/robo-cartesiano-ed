#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "calculadora.h"

/* =====================================================================
   DEFINICOES DE COMPATIBILIDADE
   ===================================================================== */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* =====================================================================
   PILHA DE STRINGS (para getInFixa)
   ===================================================================== */
#define MAX_STACK 256
#define MAX_EXPR  512

typedef struct {
    char data[MAX_STACK][MAX_EXPR];
    int  top;
} StackStr;

static void   ssInit (StackStr *s)               { s->top = -1; }
static int    ssEmpty(StackStr *s)               { return s->top < 0; }
static void   ssPush (StackStr *s, const char *v){ strcpy(s->data[++(s->top)], v); }
static char  *ssPop  (StackStr *s)               { return s->data[(s->top)--]; }
static char  *ssPeek (StackStr *s)               { return s->data[s->top]; }

/* =====================================================================
   PILHA DE DOUBLES (para getValor)
   ===================================================================== */
typedef struct {
    double data[MAX_STACK];
    int    top;
} StackDbl;

static void   sdInit (StackDbl *s)           { s->top = -1; }
static int    sdEmpty(StackDbl *s)           { return s->top < 0; }
static void   sdPush (StackDbl *s, double v) { s->data[++(s->top)] = v; }
static double sdPop  (StackDbl *s)           { return s->data[(s->top)--]; }

/* =====================================================================
   UTILITARIOS
   ===================================================================== */

static int isBinOp(const char *t) {
    return (strcmp(t, "+") == 0 || strcmp(t, "-") == 0 ||
            strcmp(t, "*") == 0 || strcmp(t, "/") == 0 ||
            strcmp(t, "%") == 0 || strcmp(t, "^") == 0);
}

static int isUnary(const char *t) {
    return (strcmp(t, "raiz") == 0 || strcmp(t, "sen") == 0 ||
            strcmp(t, "cos")  == 0 || strcmp(t, "tg")  == 0 ||
            strcmp(t, "log")  == 0);
}

static int isNumber(const char *t) {
    int i = 0;
    int hasDot = 0, hasDigit = 0;
    if (!t || !*t) return 0;
    if (t[i] == '-') i++;
    if (!t[i]) return 0;
    for (; t[i]; i++) {
        if (isdigit((unsigned char)t[i])) { hasDigit = 1; continue; }
        if (t[i] == '.' && !hasDot)       { hasDot   = 1; continue; }
        return 0;
    }
    return hasDigit;
}

static int precedence(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 ||
        strcmp(op, "%") == 0)                          return 2;
    if (strcmp(op, "^") == 0)                          return 3;
    return 0;
}

static double toRad(double deg) {
    return deg * M_PI / 180.0;
}

/*
 * Retorna a menor precedencia dos operadores no nivel 0 de parenteses
 * de uma sub-expressao infixa. Retorna 99 se nao ha operador raiz.
 */
static int rootPrecedence(const char *expr, char *rootOp) {
    int i, depth, minPrec, p;
    char c[2];
    depth   = 0;
    minPrec = 99;
    rootOp[0] = '\0';
    c[1] = '\0';

    for (i = 0; expr[i]; i++) {
        if (expr[i] == '(') { depth++; continue; }
        if (expr[i] == ')') { depth--; continue; }
        if (depth == 0) {
            c[0] = expr[i];
            if (isBinOp(c)) {
                p = precedence(c);
                if (p <= minPrec) {
                    minPrec = p;
                    strcpy(rootOp, c);
                }
            }
        }
    }
    return minPrec;
}

/* =====================================================================
   getInFixa - converte string pos-fixa para infixa
   Retorna ponteiro para buffer estatico; NULL em caso de erro.
   ===================================================================== */
char *getInFixa(char *Str) {
    static char result[MAX_EXPR];
    StackStr    stk;
    char        buf[MAX_EXPR];
    char        tmp[MAX_EXPR];
    char        expr[MAX_EXPR];
    char        op1[MAX_EXPR], op2[MAX_EXPR];
    char        rootL[8], rootR[8];
    int         precL, precR, precOp;
    int         needLeft, needRight;
    char       *token;

    if (!Str) return NULL;

    ssInit(&stk);
    strncpy(expr, Str, MAX_EXPR - 1);
    expr[MAX_EXPR - 1] = '\0';

    token = strtok(expr, " ");
    while (token != NULL) {

        if (isNumber(token)) {
            ssPush(&stk, token);

        } else if (isBinOp(token)) {
            if (stk.top < 1) return NULL;   /* operandos insuficientes */

            strcpy(op2, ssPop(&stk));
            strcpy(op1, ssPop(&stk));

            precL  = rootPrecedence(op1, rootL);
            precR  = rootPrecedence(op2, rootR);
            precOp = precedence(token);

            needLeft  = (precL < precOp);
            needRight = (precR < precOp) ||
                        (precR == precOp &&
                         (strcmp(token, "-") == 0 ||
                          strcmp(token, "/") == 0 ||
                          strcmp(token, "%") == 0));

            if (needLeft && needRight)
                snprintf(buf, MAX_EXPR, "(%s)%s(%s)", op1, token, op2);
            else if (needLeft)
                snprintf(buf, MAX_EXPR, "(%s)%s%s",   op1, token, op2);
            else if (needRight)
                snprintf(buf, MAX_EXPR, "%s%s(%s)",   op1, token, op2);
            else
                snprintf(buf, MAX_EXPR, "%s%s%s",     op1, token, op2);

            ssPush(&stk, buf);

        } else if (isUnary(token)) {
            if (ssEmpty(&stk)) return NULL;  /* operando insuficiente */

            strcpy(tmp, ssPop(&stk));

            if      (strcmp(token, "raiz") == 0) snprintf(buf, MAX_EXPR, "raiz(%s)", tmp);
            else if (strcmp(token, "log")  == 0) snprintf(buf, MAX_EXPR, "log(%s)",  tmp);
            else                                 snprintf(buf, MAX_EXPR, "%s(%s)",   token, tmp);

            ssPush(&stk, buf);

        } else {
            return NULL;   /* operador desconhecido */
        }

        token = strtok(NULL, " ");
    }

    if (stk.top != 0) return NULL;  /* sobrou mais de um valor: erro */

    strncpy(result, ssPeek(&stk), MAX_EXPR - 1);
    result[MAX_EXPR - 1] = '\0';
    return result;
}

/* =====================================================================
   getValor - avalia expressao pos-fixa e retorna o resultado
   Retorna 0.0/0.0 (NaN) em caso de erro.
   ===================================================================== */
float getValor(char *Str) {
    StackDbl stk;
    char     expr[MAX_EXPR];
    char    *token;
    double   a, b, r;

    if (!Str) return 0.0f / 0.0f;

    sdInit(&stk);
    strncpy(expr, Str, MAX_EXPR - 1);
    expr[MAX_EXPR - 1] = '\0';

    token = strtok(expr, " ");
    while (token != NULL) {

        if (isNumber(token)) {
            sdPush(&stk, atof(token));

        } else if (isBinOp(token)) {
            if (stk.top < 1) return 0.0f / 0.0f;

            b = sdPop(&stk);
            a = sdPop(&stk);

            if      (strcmp(token, "+") == 0) r = a + b;
            else if (strcmp(token, "-") == 0) r = a - b;
            else if (strcmp(token, "*") == 0) r = a * b;
            else if (strcmp(token, "/") == 0) {
                if (b == 0.0) return 0.0f / 0.0f;   /* divisao por zero */
                r = a / b;
            }
            else if (strcmp(token, "%") == 0) {
                if (b == 0.0) return 0.0f / 0.0f;
                r = fmod(a, b);
            }
            else r = pow(a, b);   /* ^ */

            sdPush(&stk, r);

        } else if (isUnary(token)) {
            if (sdEmpty(&stk)) return 0.0f / 0.0f;

            a = sdPop(&stk);

            if (strcmp(token, "raiz") == 0) {
                if (a < 0.0) return 0.0f / 0.0f;    /* raiz de negativo */
                sdPush(&stk, sqrt(a));
            } else if (strcmp(token, "log") == 0) {
                if (a <= 0.0) return 0.0f / 0.0f;   /* log de zero ou negativo */
                sdPush(&stk, log10(a));
            } else if (strcmp(token, "sen") == 0) {
                sdPush(&stk, sin(toRad(a)));
            } else if (strcmp(token, "cos") == 0) {
                sdPush(&stk, cos(toRad(a)));
            } else if (strcmp(token, "tg") == 0) {
                sdPush(&stk, tan(toRad(a)));
            }

        } else {
            return 0.0f / 0.0f;   /* operador desconhecido */
        }

        token = strtok(NULL, " ");
    }

    if (stk.top != 0) return 0.0f / 0.0f;
    return (float)sdPop(&stk);
}