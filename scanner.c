#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


int need_square_bracket = 0;

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
    char *REWD[] = {"if", "else", "while", "for", "do", "switch", "case", "default", "continue",
                    "int", "long", "float", "double", "char", "break", "static", "extern", "auto", "register",
                    "sizeof", "union", "struct", "short", "enum", "return", "goto", "const", "signed", "unsigned",
                    "typedef", "void"
                   };
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

bool check_is_OPER(char word[]) {
    char *OPER[] = {"+","-","*", "/","=", ",", "%",">>", "<<", "++", "--", "+=", "-=", "*=", "/=", "%=",
                    "!", "&&", "||", "&","[", "]", "|", "^", ".", "->",">", "<", "==", ">=", "<=", "!="
                   };

    bool check = false;
    int i;
    for(i = 0; i < 31; i++) {
        if(strcmp(word, OPER[i]) == 0) {
            check = true;
            break;
        }
    }

    if(strcmp(word, "[") == 0){
        need_square_bracket++;
    } else if(strcmp(word, "]") == 0) {
        if(need_square_bracket)
        need_square_bracket--;


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
    } else if(check_is_OPER(word)) {
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
