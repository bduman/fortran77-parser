#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define keywordsAdet 39
#define operatorsAdet 11

char lexeme[80];
int lexemeUzunlugu = 0;
void dosyadanKarakterAl();
char sonAlinanKarakter = ' ';

// buna sadece comment tespiti icin gerek duyuyoruz
// onun disinda hic bir yerde kullanilmiyor
char oncekiAlinanKarakter = ' ';

FILE *fortranFile;
FILE *lexicalAnalysisFile;

char *fortranKeywords[] = {
    "ASSIGN","BACKSPACE","BLOCK DATA","CALL","CLOSE",
    "COMMON","CONTINUE","DATA","DIMENSION","DO","ELSE",
    "ELSE IF","END","ENDFILE","ENDIF","ENTRY","EQUIVALENCE",
    "EXTERNAL","FORMAT","FUNCTION","GOTO","IF","IMPLICIT","INQUIRE",
    "INTRINSIC","OPEN","PARAMETER","PAUSE","PRINT","PROGRAM","READ",
    "RETURN","REWIND","REWRITE","SAVE","STOP","SUBROUTINE","THEN","WRITE"
};

char *fortranOperators[] = {
    ".NOT.",".AND.",".OR.",".EQV.",".NEQV.",
    ".LT.",".LE.",".EQ.",".NE.",".GT.",".GE."
};

/*
* 0 => identifiers
* 1 => operators
* 2 => parantesis
* 3 => keywords
* 4 => NULL => kararsiz => comments etc.
*/
int lexemeTypeCounts[5] = {0};

int main() {

    char filename[20];

    printf("If there is any error detection, program stopped and show error\n");
    printf("Write source file name (not including .for):");
    gets(filename);

    // dosya adinin sonuna uzanti ekleniyor
    strcat(filename, ".for");

    if((fortranFile = fopen(filename, "r")) == NULL) {

        printf("\n!!!.for file doesnt exist.");

    } else {

        strtok(filename, ".");
        strcat(filename, ".txt");

        if((lexicalAnalysisFile = fopen(filename, "w+")) == NULL) {

            printf("\n!!!lexical file could not be created or opened.");

        } else {

            // dosyadan bitene lexeme okuma
            do {

                devami();

            } while(!feof(fortranFile));


            // artik sayilari ekrana bastirma zamani
            printf("\n%s file containing the lexemes found during analysis\n", filename);
            printf("\nTotal number of lexemes found by type:\n\n");
            printf("\tidentifier count: %d\n", lexemeTypeCounts[0]);
            printf("\toperators count: %d\n", lexemeTypeCounts[1]);
            printf("\tparantesis count: %d\n", lexemeTypeCounts[2]);
            printf("\tkeywords count: %d\n", lexemeTypeCounts[3]);
            //printf("\tcomments line count: %d\n", lexemeTypeCounts[4]);

        }
    }

    return 0;
}

void devami() {

    int tespit;
    char karakter;

    if(isspace(sonAlinanKarakter)) {
        dosyadanKarakterAl();
    }

    lexeme[lexemeUzunlugu++] = sonAlinanKarakter;
    lexeme[lexemeUzunlugu] = '\0';

    // comments
    if(lexeme[0] == 'c' || lexeme[0] == 'C' && oncekiAlinanKarakter == '\n') {

        while(sonAlinanKarakter != '\n' && sonAlinanKarakter != EOF)
            sonAlinanKarakter = getc(fortranFile);

        // NOTICE: bu comment oldugu icin lexeme olarak loglanmiyor.
        //printLexeme("comment", NULL, 4);

    // identifier yada keyword olduguna isaret
    } else if(isalpha(lexeme[0])) {

        while(sonAlinanKarakter == '_' || isalpha(sonAlinanKarakter) || isdigit(sonAlinanKarakter)) {

            sonAlinanKarakter = getc(fortranFile);

            if(isspace(sonAlinanKarakter))
                break;

            if(sonAlinanKarakter == '_' || isalpha(sonAlinanKarakter) || isdigit(sonAlinanKarakter)) {
                lexeme[lexemeUzunlugu++] = sonAlinanKarakter;
                lexeme[lexemeUzunlugu] = '\0';
            }
        }

        tespit = keywordOrIdentifier();

        if(tespit) {
            // keyword olduguna dair bilgiyi dosyaya yazdir
            printLexeme("keyword", NULL, 3);
        } else {
            // identifier olduguna dair bilgiyi dosyaya yazdir
            printLexeme("identifier", NULL, 0);
        }

    // sayisal ifade
    } else if(isdigit(lexeme[0])) {

        while(isdigit(sonAlinanKarakter)) {

            sonAlinanKarakter = getc(fortranFile);

            // bosluk gelirse birak
            if(isspace(sonAlinanKarakter))
                break;

            if(isalpha(sonAlinanKarakter)) {
                // DONE: OLUMCUL HATA! Olmamasi gereken bir durum burada hata vermesi gerek
                printfError("Its must be digit constant but have alphabetic characters");
                break;
            }
            if(isdigit(sonAlinanKarakter)) {
                // digit ise lexeme ekler
                lexeme[lexemeUzunlugu++] = sonAlinanKarakter;
                lexeme[lexemeUzunlugu] = '\0';
            }
        }

        // digit olduguna dair bilgiyi dosyaya yazdir
        printLexeme("identifier [digit]", NULL, 0);

    // operator
    } else if(lexeme[0] == '.') {

        sonAlinanKarakter = getc(fortranFile);

        while(isalpha(sonAlinanKarakter)) {
            // alfabetik ise lexeme ekler
            lexeme[lexemeUzunlugu++] = sonAlinanKarakter;
            lexeme[lexemeUzunlugu] = '\0';

            sonAlinanKarakter = getc(fortranFile);
        }

        if(sonAlinanKarakter == '.') {

            lexeme[lexemeUzunlugu++] = sonAlinanKarakter;
            lexeme[lexemeUzunlugu] = '\0';

            // kontrol
            tespit = isOperator();

            if(tespit) {
                // operator olduguna dair bilgiyi dosyaya yazdir
                printLexeme("operator [logical or relational]", NULL, 1);
                sonAlinanKarakter = getc(fortranFile);
            } else {
                // DONE: HATA! operator gorunumlu baska bisey .d
                printf("\n!!! %s", lexeme);
                printfError("Its must be operator [logical or relational] but does not exist this language [err1]");
            }
        } else {
            // DONE: HATA2! operator gorunumlu baska bisey .d
            printf("\n!!! %s", lexeme);
            printfError("Its must be operator [logical or relational] but does not exist this language [err2]");
        }

    // ozel isaretler
    } else {

        switch(lexeme[0]) {

            case '(':
                printLexeme("parenthesis [left]", "(", 2);
                break;
            case ')':
                printLexeme("parenthesis [right]", ")", 2);
                break;
            case '+':
                printLexeme("operator [addition]", "+", 1);
                break;
            case '-':
                printLexeme("operator [subtraction or unary]", "-", 1);
                break;
            case '*':
                printLexeme("operator [multiplication]", "*", 1);
                break;
            case '/':
                printLexeme("operator [division]", "/", 1);
                break;
            case ',':
                printLexeme("operator [comma]", ",", 1);
                break;
            case '=':
                printLexeme("operator [equation]", "=", 1);
                break;
            case '\'':
                do {
                    sonAlinanKarakter = getc(fortranFile);
                    lexeme[lexemeUzunlugu++] = sonAlinanKarakter;
                } while(sonAlinanKarakter != '\n' && sonAlinanKarakter != '\'');

                if(sonAlinanKarakter == '\'') {
                    printLexeme("identifier [string]", NULL, 0);
                } else {
                    // DONE: HATA! string tirnagi acilip kapanmamis yok boyle bi dunya
                    printfError("Unclosed quotation mark");
                }
            break;
            default:
                // DONE: HATA! bende boyle bir karakter yok
                if(!isspace(lexeme[0])) {
                    lexeme[lexemeUzunlugu++] = '!';
                    lexeme[lexemeUzunlugu++] = '!';
                    lexeme[lexemeUzunlugu++] = '!';
                    lexeme[lexemeUzunlugu++] = ' ';
                    lexeme[lexemeUzunlugu++] = lexeme[0];
                    lexeme[0] = '\n';
                    lexeme[lexemeUzunlugu] = '\0';
                    printf("%s", lexeme);
                    printfError("Found unidentified character");
                }
            break;
        }

        sonAlinanKarakter = getc(fortranFile);
    }
    // lexeme stringi bir sonraki lexeme tespiti icin resetleniyor
    lexemeUzunlugu = 0;
    lexeme[0] = '\0';
}

// bosluk atlar
void dosyadanKarakterAl() {

    sonAlinanKarakter = getc(fortranFile);

    while(isspace(sonAlinanKarakter)) {
        oncekiAlinanKarakter = sonAlinanKarakter;
        sonAlinanKarakter = getc(fortranFile);
    }
}

// TODO func aciklamasi yazilacak
void printLexeme(char * string, char *lexemee, int lexemeType) {

    if(lexemee == NULL)
        lexemee = lexeme;

    lexemeTypeCounts[lexemeType]++;

    fprintf(lexicalAnalysisFile, "%-25s=>\t%s\n", lexeme, string);
}

void printfError(char * error) {

    printf("\n\n!!! %s", error);
    exit(1);
}

// keyword ise 1 identifier 0
int keywordOrIdentifier() {

    int i = 0;

    for(i;i<keywordsAdet;i++) {

        if(strcasecmp(fortranKeywords[i], lexeme) == 0)
            return 1;
    }

    return 0;
}

// 0 => operator degil, 1 => operator
int isOperator() {

    int i = 0;

    for(i;i<operatorsAdet;i++) {

        if(strcasecmp(fortranOperators[i], lexeme) == 0)
            return 1;
    }

    return 0;
}
