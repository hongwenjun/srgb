#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NKEYS ( sizeof keytab / sizeof keytab[0] )
#define MAXWORD 100

struct key {
    char* word;
    int count;
} keytab[] = {
    "auto", 0,
    "break", 0,
    "case", 0,
    "char", 0,
    "const", 0,
    "continue", 0,
    "default", 0,
    "do", 0,
    "double", 0,
    "else", 0,
    "enum", 0,
    "extern", 0,
    "float", 0,
    "for", 0,
    "goto", 0,
    "if", 0,
    "inline", 0,
    "int", 0,
    "long", 0,
    "register", 0,
    "restrict", 0,
    "return", 0,
    "short", 0,
    "signed", 0,
    "sizeof", 0,
    "static", 0,
    "struct", 0,
    "switch", 0,
    "typedef", 0,
    "union", 0,
    "unsigned", 0,
    "void", 0,
    "volatile", 0,
    "while", 0,
    "_Bool", 0,
    "_Complex", 0,
    "_Imaginary", 0,
    "_Generic", 0
};

int getword(char*, int);
int binsearch(char*, struct key*, int);

/* 统计输入中C语言关键字的出现次数 */
int main(void)
{
    int n;
    char word[MAXWORD];

    while (getword(word, MAXWORD) != EOF)
        if (isalpha(word[0]))
            if ((n = binsearch(word, keytab, NKEYS)) >= 0)
                keytab[n].count++;

    for (n = 0; n < NKEYS; n++)
        if (keytab[n].count > 0)
            printf("%4d %s\n", keytab[n].count, keytab[n].word);

    return EXIT_SUCCESS;
}

/* 在tab[o]到tab[n-1]中查找单词 */
int binsearch(char* word, struct key tab[], int n)
{
    int cond;
    int low, high, mid;

    low = 0;
    high = n - 1;
    while (low <= high) {
        mid = (low + high) / 2;
        if ((cond = strcmp(word, tab[mid].word)) < 0)
            high = mid - 1;
        else if (cond > 0)
            low = mid + 1;
        else
            return mid;
    }
    return -1;
}


// getch.c
#include <stdio.h>
#define BUFSIZE 100

int buf[BUFSIZE];  // buffer for ungetch
int bufp = 0;      // next free position in buf

// getch: get a (possibily pushed back)  character
int getch(void)
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

// ungetch: push character back onto the input
void ungetch(int c)
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters \n");
    else
        buf[bufp++] = c;
}

// comment: skip over comment and return a character
int comment(void)
{
    int c;
    while ((c = getch()) != EOF)
        if (c == '*')
            if ((c = getch()) == '/')
                break;
            else
                ungetch(c);

    return c;
}

// getword: get next word or character from input
int getword(char* word, int lim)
{
    int c, d;
    char* w = word;

    while (isspace(c = getch()))
        ;

    if (c != EOF)
        *w++ = c;

    if (isalpha(c) || c == '_' || c == '#') {
        for (; --lim > 0; w++)
            if (!isalnum(*w = getch()) && *w != '_') {
                ungetch(*w);
                break;
            }
    } else if (c == '\'' || c == '"') {
        for (; --lim > 0; w++)
            if ((*w = getch()) == '\\')
                *++w = getch();
            else if (*w == c) {
                w++;
                break;
            } else if (*w == EOF)
                break;
    } else if (c == '/')
        if ((d = getch()) == '*')
            c = comment();
        else
            ungetch(d);

    *w = '\0';
    return c;
}
