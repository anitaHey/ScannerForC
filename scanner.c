#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *OPER[] = {"+","-","*", "/","=", ",", "%",">>", "<<", "++", "--", "+=", "-=", "*=", "/=", "%=",
                "!", "&&", "||", "&","[", "]", "|", "^", ".", "->",">", "<", "==", ">=", "<=", "!="
               };
char *REWD[] = {"if", "else", "while", "for", "do", "switch", "case", "default", "continue",
                "int", "long", "float", "double", "char", "break", "static", "extern", "auto", "register",
                "sizeof", "union", "struct", "short", "enum", "return", "goto", "const", "signed", "unsigned",
                "typedef", "void"
               };
char *SPEC[] = {"{","}","(", ")",";"};

static const SQUARE_BRACKET = 0; // []
static const CURLY_BRACKET = 1;  // {}
static const PARENTHESES = 2;  // ()
static const MULTIPLE_COMMENT = 3;  // /**/
static const CHAR = 4;
static const STRING = 5;

int special_count[6] = {0, 0, 0, 0, 0, 0};

void split(char *src,const char *separator,char **dest,int *num) {
    char *pNext;
    int count = 0;
    if (src == NULL || strlen(src) == 0 || separator == NULL || strlen(separator) == 0) return;
    pNext = (char *)strtok(src,separator);

    while(pNext != NULL) {
        *dest++ = pNext;
        ++count;
        pNext = (char *)strtok(NULL,separator);
    }
    *num = count;
}

bool check_is_REWD(char word[]) {
    bool check = false;
    int i;
    for(i = 0; i < 31; i++) {
        if(strcmp(word, REWD[i]) == 0) {
            check = true;
            break;
        }
    }

    return check;
}

bool check_is_OPER(char word[], int row_count) {
    bool check = false;
    int i;
    for(i = 0; i < 32; i++) {
        if(strcmp(word, OPER[i]) == 0) {
            check = true;
            break;
        }
    }

    if(strcmp(word, "[") == 0) {
        special_count[SQUARE_BRACKET]++;
    } else if(strcmp(word, "]") == 0) {
        if(special_count[SQUARE_BRACKET] > 0)
            special_count[SQUARE_BRACKET]--;
        else
            printf("error at line %d, missing operator '[' ",row_count);
    }

    return check;
}

bool check_is_SPEC(char word[]) {
    bool check = false;
    int i;
    for(i = 0; i < 5; i++) {
        if(strcmp(word, SPEC[i]) == 0) {
            check = true;
            break;
        }
    }

    return check;
}

void scan_word(char word[], int row_count) {
    int i;
    bool print = false;
    char *type;

    if(check_is_REWD(word)) {
        type = "REWD";
        print = true;
    } else if(check_is_OPER(word, row_count)) {
        type = "OPER";
        print = true;
    }


//    for(i=0;i< strlen(word);i++){
//        printf("%c\n",word[i]);
//    }

    if(print) printf("%d   %s   %s",row_count,type,word);
}

int main() {
    char file_name[50];
    char read_word[50];
    char *split_word[50] = {0};
    FILE *fpr;
    int i, row_count = 0, word_count;

    printf("Enter the input file name:\n");
    scanf("%s", &file_name);

    fpr = fopen(file_name, "r");

    while(!feof(fpr)) {
        ++row_count;

        memset(read_word, 0, sizeof(read_word));
        fgets(read_word, sizeof(read_word) - 1, fpr);
        split(read_word, " ", split_word, &word_count);

        for(i=0; i<word_count; i++) {
//            printf("%s\n",split_word[i]);
            scan_word(split_word[i], row_count);
        }

    }

    fclose(fpr);

    return 0;
}
