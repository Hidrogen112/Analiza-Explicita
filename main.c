#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>




enum { ID, INT, REAL, STR,
       // Identificatori si constante
       VAR, FUNCTION, IF, ELSE, WHILE, END, RETURN, TYPE_INT, TYPE_REAL, TYPE_STR,
       // Cuvinte cheie
       COLON, SEMICOLON, LPAR, RPAR, COMMA, OR, AND, NOT, EQUAL, NOTEQUAL, LESS, ASSIGN, ADD, SUB, MUL, DIV, FINISH
       // Separatori, operatori si FINISH, fara SPACE si COMMENT
     };
char *numeAtomi[] = { "ID", "INT", "REAL", "STR",
                      "VAR", "FUNCTION", "IF", "ELSE", "WHILE", "END", "RETURN", "TYPE_INT", "TYPE_REAL", "TYPE_STR",
                      "COLON", "SEMICOLON", "LPAR", "RPAR", "COMMA", "OR", "AND", "NOT", "EQUAL", "NOTEQUAL", "LESS", "ASSIGN", "ADD", "SUB", "MUL", "DIV", "FINISH"
                    };

typedef struct
{
    int cod;		// codul atomului ( ID, INT, ...)
    int linie;		// linia atomului respectiv
    union
    {
        char text[100];		// pentru ID-uri si stringuri
        double vr;				// pentru real
        int vi;						// pentru int
    };
} Atom;

Atom atomi[10000];	// vectorul cu atomii extrasi din fisierul de intrare
int nrAtomi;	// numarul de atomi din vectorul atomi


char bufin[30001];
char *pch;	// cursor la caracterul curent din bufin
int linie=1;		// linia curenta; adaugata automat la atom de addAtom

void addAtom(int codAtom)
{
    atomi[nrAtomi].cod = codAtom;
    atomi[nrAtomi].linie = linie;
    nrAtomi++;
}

// la fiecare apel returneaza codul unui atom
int getNextTk()			// get next token (atom lexical)
{
    int state=0;		// starea curenta
    char buf[100];
    int n=0;		// nr caractere din buf
    for(;;)
    {
        char ch=*pch;	// caracterul curent
        //printf("#%d %c(%d)\n",state,ch,ch);	// pt debugging
        // cate un case pentru fiecare stare din diagrama
        switch(state)
        {
        case 0:
            // testam toate tranzitiile posibile din acea stare
            if (isalpha(ch) || ch == '_')
            {
                state = 1;
                pch++;
                buf[n++] = ch;
            }
            else if (isdigit(ch))
            {
                state = 3;
                pch++;
                buf[n++] = ch;
            }
            else if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t')
            {
                pch++;
                if (ch == '\n') linie++;
            }
            else if (ch == '#')
            {
                state = 8;
                pch++;
            }
            else if (ch == '=')
            {
                state = 9;
                pch++;
            }
            else if (ch == ';')
            {
                state = 12;
                pch++;
            }
            else if (ch == '\0')
            {
                state = 13;
                pch++;
            }
            else if (ch == '\"')
            {
                state = 14;
                pch++;
            }
            else if (ch == '(')
            {
                state = 16;
                pch++;
            }
            else if (ch == ')')
            {
                state = 17;
                pch++;
            }
            else if (ch == ',')
            {
                state = 18;
                pch++;
            }
            else if (ch == '|')
            {
                state = 31;
                pch++;
            }
            else if (ch == '&')
            {
                state = 30;
                pch++;
            }
            else if (ch == '!')
            {
                state = 21;
                pch++;
            }
            else if (ch == '<')
            {
                state = 24;
                pch++;
            }
            else if (ch == '+')
            {
                state = 25;
                pch++;
            }
            else if (ch == '-')
            {
                state = 26;
                pch++;
            }
            else if (ch == '*')
            {
                state = 27;
                pch++;
            }
            else if (ch == '/')
            {
                state = 28;
                pch++;
            }
            else if (ch == ':')
            {
                state = 29;
                pch++;
            }
            else printf("Caracter invalid\n");
            break;
        case 1:
            if(isalnum(ch)||ch=='_')
            {
                pch++;
                buf[n++]=ch;
            }
            else
            {
                state=2;
            }
            break;
        case 2:
            buf[n] = '\0';	// incheiere sir
            if (strcmp(buf, "var") == 0) addAtom(VAR);
            else if (strcmp(buf, "function") == 0) addAtom(FUNCTION);
            else if (strcmp(buf, "if") == 0) addAtom(IF);
            else if (strcmp(buf, "else") == 0) addAtom(ELSE);
            else if (strcmp(buf, "while") == 0) addAtom(WHILE);
            else if (strcmp(buf, "end") == 0) addAtom(END);
            else if (strcmp(buf, "return") == 0) addAtom(RETURN);
            else if (strcmp(buf, "int") == 0) addAtom(TYPE_INT);
            else if (strcmp(buf, "real") == 0) addAtom(TYPE_REAL);
            else if (strcmp(buf, "str") == 0) addAtom(TYPE_STR);
            else
            {
                strcpy(atomi[nrAtomi].text, buf);
                addAtom(ID);	// id simplu
            }
            return ID;
        case 3:
            if (isdigit(ch))
            {
                state = 3;
                pch++;
                buf[n++] = ch;
            }
            else if (ch == '.')
            {
                state = 4;
                pch++;
                buf[n++] = ch;
            }
            else
            {
                state = 7;
            }
            break;
        case 4:
            if (isdigit(ch))
            {
                state = 5;
                pch++;
                buf[n++] = ch;
            }
            else
            {
                printf("Dupa punctul zecimal trebuie sa urmeze o cifra");
                exit(-1);
            }
            break;
        case 5:
            if (isdigit(ch))
            {
                state = 5;
                pch++;
                buf[n++] = ch;
            }
            else
            {
                state = 6;
            }
            break;
        case 6:
            buf[n] = '\0';
            addAtom(REAL);
            atomi[nrAtomi-1].vr = atof(buf);
            //printf("Linia:%d :%f \n", atomi[nrAtomi-1].linie, atomi[nrAtomi-1].vr);
            return REAL;
        case 7:
            buf[n] = '\0';
            addAtom(INT);
            atomi[nrAtomi-1].vi = atoi(buf);
            //printf("Linia:%d :%d \n", atomi[nrAtomi-1].linie, atomi[nrAtomi-1].vi);
            return INT;


        case 8:
            if (!(ch == '\n' || ch == '\r' || ch == '\0'))
            {
                state = 8;
                if (ch == '\n') linie++;
                pch++;
            }
            else
            {
                state = 0;
            }
            break;

        case 9:
            if (ch == '=')
            {
                state = 11;
                pch++;
            }
            else
            {
                state = 10;
            }
            break;
        case 10:
            addAtom(ASSIGN);
            return ASSIGN;
        case 11:
            addAtom(EQUAL);
            return EQUAL;
        case 12:
            addAtom(SEMICOLON);
            return SEMICOLON;
        case 13:
            addAtom(FINISH);
            return FINISH;
        case 14:
            if (!(ch == '\"'))
            {
                pch++;
                buf[n++] = ch;
            }
            else if (ch == '\"')
            {
                pch++;
               // buf[n++] = ch;
                state = 15;
            }
            break;

        case 15:
            buf[n] = '\0';
            addAtom(STR);
            strcpy(atomi[nrAtomi-1].text,buf);
            return STR;
        case 16:
            addAtom(LPAR);
            return LPAR;
        case 17:
            addAtom(RPAR);
            return RPAR;
        case 18:
            addAtom(COMMA);
            return COMMA;
        case 19:
            addAtom(OR);
            return OR;
        case 20:
            addAtom(AND);
            return AND;
        case 21:
            if (ch == '=')
            {
                state = 23;
                pch++;
            }
            else
            {
                state = 22;
                pch++;
            }
            break;
        case 22:
            addAtom(NOT);
            return NOT;
        case 23:
            addAtom(NOTEQUAL);
            return NOTEQUAL;
        case 24:
            addAtom(LESS);
            return LESS;
        case 25:
            addAtom(ADD);
            return ADD;
        case 26:
            addAtom(SUB);
            return SUB;
        case 27:
            addAtom(MUL);
            return MUL;
        case 28:
            addAtom(DIV);
            return DIV;
        case 29:
            addAtom(COLON);
            return COLON;
        case 30:
            if (ch == '&')
            {
                state = 20;
                pch++;
            }
            else
            {
                printf("Caracter invalid!");
                return (-1);
            }
            break;
        case 31:
            if (ch == '|')
            {
                state = 19;
                pch++;
            }
            else
            {
                printf("Caracter invalid!");
                return (-1);
            }
            break;
        default:
            printf("stare invalida %d\n", state);
            err("Eroare");
        }
    }

}

void err (char *msg) {
printf ("\n Eroare la linia %d (%s)", atomi[nrAtomi].linie, msg);
exit(0);
}
void afisareAtomi()
{
    int i;
    printf("\n~ Analiza lexicala ~\n");
    for (i=0; i<nrAtomi; i++)
    {
        printf("\nLinia %d %s", atomi[i].linie, numeAtomi[atomi[i].cod]);
        if((atomi[i].cod == ID) || (atomi[i].cod == STR))
        {
            printf(":%s ",atomi[i].text);
        }
        else if (atomi[i].cod == INT)
        {
            printf(":%d ", atomi[i].vi);
        }
        else if(atomi[i].cod == REAL)
        {
            printf(":%f ", atomi[i].vr);
        }
        else printf("");
    }
}
int main()
{
    FILE *fis;
    fis=fopen("prog1.q","rb");
    if(fis==NULL)
    {
        printf("nu s-a putut deschide fisierul");
        return -1;
    }
    int n=fread(bufin,1,30000,fis);	// returneaza nr de elemente citite integral
    bufin[n]='\0';
    fclose(fis);
    puts(bufin);
    pch=bufin;	// initializare pch pe prima pozitie din bufin
    // extragere atomi
    while(getNextTk()!=FINISH)
    {
    }
    // Afisare atomi
    afisareAtomi();
    return 0;
}
