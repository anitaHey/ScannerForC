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

int special_count[4] = {0, 0, 0, 0};
bool single_comment = false;
bool multiple_comment[] = {false,false};
bool is_string[] = {false,false};
bool is_PREP = false;

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

int check_is_REWD(char word[]) {
    int check = 0;
    int i;
    for(i = 0; i < 31; i++) {
        if(strcmp(word, REWD[i]) == 0) {
            check = 1;
            break;
        }
    }

    return check;
}

int check_is_OPER(char word[], int row_count) {
    int check = 0;
    int i;
    for(i = 0; i < 32; i++) {
        if(strcmp(word, OPER[i]) == 0) {
            check = 1;
            break;
        }
    }

    if(strcmp(word, "[") == 0) {
        special_count[SQUARE_BRACKET]++;
    } else if(strcmp(word, "]") == 0) {
        if(special_count[SQUARE_BRACKET] > 0) {
            special_count[SQUARE_BRACKET]--;
        } else {
            printf("error at line %d, missing operator '[' ",row_count);
            check = 2;
        }
    }

    return check;
}

int check_is_SPEC(char word[], int row_count) {
    int check = 0;
    int i;
    for(i = 0; i < 5; i++) {
        if(strcmp(word, SPEC[i]) == 0) {
            check = 1;
            break;
        }
    }

    if(strcmp(word, "{") == 0) {
        special_count[CURLY_BRACKET]++;
    } else if(strcmp(word, "}") == 0) {
        if(special_count[CURLY_BRACKET] > 0) {
            special_count[CURLY_BRACKET]--;
        } else {
            printf("error at line %d, missing operator '{' ",row_count);
            check = 2;
        }
    } else if(strcmp(word, "(") == 0) {
        special_count[PARENTHESES]++;
    } else if(strcmp(word, ")") == 0) {
        if(special_count[PARENTHESES] > 0)
            special_count[PARENTHESES]--;
        else {
            printf("error at line %d, missing operator '(' ",row_count);
            check = 2;
        }
    }

    return check;
}

int check_is_COMMENT(char word[], int row_count) {
    int check = 0;
    if(strcmp(word, "//") == 0) {
        single_comment = true;
        check = true;
        check = 2;
    } else if(strcmp(word, "/*") == 0) {
        special_count[MULTIPLE_COMMENT]++;
        multiple_comment[0] = true;
        check = 2;
    } else if(strcmp(word, "*/") == 0) {
        if(special_count[MULTIPLE_COMMENT] > 0) {
            special_count[MULTIPLE_COMMENT]--;
            multiple_comment[0] = false;
            multiple_comment[1] = true;
        } else {
            printf("error at line %d, missing operator '/*' ",row_count);
        }
        check = 2;
    }

    return check;
}

int check_is_CHAR(char word[], int row_count) {
    int check = 0;
    if(strcmp(word[0], "'") == 0) {
        if(strcmp(word[-1], "'") == 0) {
            if(strlen(word) == 3) {
                check = true;
                check = 1;
            } else {
                printf("error at line %d, wrong char.",row_count);
                check = 2;
            }
        } else {
            printf("error at line %d, missing operator ' .",row_count);
            check = 2;
        }
    }

    return check;
}

int check_is_STR(char word[], int row_count) {
    int check = 0;
    bool first = false;
    if(strcmp(word[0], "\"") == 0) {
        is_string[0] = true;
        first = true;
        check = 2;
    }

    if(strcmp(word[-1], "\"") == 0) {
        if(first) {
            is_string[0] = false;
            check = 1;
        } else {
            is_string[0] = false;
            is_string[1] = true;
            check = 2;
        }
    }

    return check;
}

int check_is_INTE(char word[]) {
    int i;
    for(i = 0;i<strlen(word);i++){
        if(!isdigit(word[i]))
            return 0;
    }

    return 1;
}


int check_is_FLOAT(char word[]) {
    if (word == NULL) {
        return 0;
    }
    char *endptr;
    if (word == endptr) {
        return 0; // no conversion;
    }
    // Look at trailing text
    while (isspace((unsigned char ) *endptr))
        endptr++;

    if(*endptr == '\0') return 1;
    else return 0;
}

int check_is_PREP(char word[]){
    if(strcmp(word[0], "#include") == 0){
        is_PREP = true;
        return 1;
    }
}

void scan_word(char word[], int row_count) {
    int state;
    bool print = false;
    char *type;

    if(!single_comment && !is_PREP) {

        if(is_string[0]) {
            check_is_STR(word, row_count);
        } else if(multiple_comment[0]) {
            check_is_COMMENT(word, row_count);
        } else {
            if(state = check_is_REWD(word) != 0) {
                type = "REWD";
            } else if(state = check_is_OPER(word, row_count) != 0) {
                type = "OPER";
            } else if(state = check_is_SPEC(word, row_count) != 0) {
                type = "SPEC";
            } else if(state = check_is_COMMENT(word, row_count) != 0) {

            } else if(state = check_is_CHAR(word, row_count) != 0) {
                type = "CHAR";
            } else if(state = check_is_STR(word, row_count) != 0) {
                type = "STR";
            } else if(state = check_is_INTE(word) != 0) {
                type = "INTE";
            } else if(state = check_is_FLOAT(word) != 0) {
                type = "FLOT";
            } else if()
        }


        //    for(i=0;i< strlen(word);i++){
        //        printf("%c\n",word[i]);
        //    }

        if(state == 1) printf("%d   %s   %s",row_count,type,word);
    }

}

int main() {
    char file_name[50];
    char read_word[50];
    char *split_word[50] = {0};
    FILE *fpr;
    int i, j, row_count = 0, word_count;

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

            if(multiple_comment[1]) {
                char tem_word[50];
                for(j = 0; j<=i; j++) {
                    strcpy(tem_word, split_word[j]);
                    strcpy(tem_word, " ");
                }

                printf("%d   MC   %s",row_count, tem_word);
                multiple_comment[1] = false;
            } else if(is_string[1]) {
                char tem_word[50];
                for(j = 0; j<=i; j++) {
                    strcpy(tem_word, split_word[j]);
                    strcpy(tem_word, " ");
                }

                printf("%d   STR   %s",row_count, tem_word);
                is_string[1] = false;
            }
        }

        if(single_comment) {
            printf("%d   SC   %s",row_count, read_word);
        } else if(multiple_comment[0]) {
            printf("%d   MC   %s",row_count, read_word);
        } else if(is_string[0]) {
            printf("error at line %d, missing \" for the string .",row_count);
        }
    }

    fclose(fpr);

    return 0;
}
