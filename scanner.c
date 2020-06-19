#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

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

static const SINGLE = 0;
static const MULTIPLE = 1;
static const STRING = 2;

int start_arr[5] = {-1, -1, -1, -1, -1};
bool check_end[3] = {false, false, false};

bool is_PREP = false;

int token_len = 0;
int token_split[] = {0, 0};


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
    for(int i = 0; i < 31; i++) {
        if(strcmp(word, REWD[i]) == 0) {
            check = 1;
            break;
        }
    }

    return check;
}

int check_is_OPER(char word[], int row_count) {
    int check = 0;
    for(int i = 0; i < 32; i++) {
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
            printf("ERROR: At line %d, missing operator '[' \n",row_count);
            check = 2;
        }
    }

    return check;
}

int check_is_SPEC(char word[], int row_count) {
    int check = 0;
    for(int i = 0; i < 5; i++) {
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
            printf("ERROR: At line %d, missing operator '{' \n",row_count);
            check = 2;
        }
    } else if(strcmp(word, "(") == 0) {
        special_count[PARENTHESES]++;
    } else if(strcmp(word, ")") == 0) {
        if(special_count[PARENTHESES] > 0)
            special_count[PARENTHESES]--;
        else {
            printf("ERROR: At line %d, missing operator '(' \n",row_count);
            check = 2;
        }
    }

    return check;
}

int check_is_COMMENT(char word[], int row_count, int number) {
    int check = 0;
    int len = strlen(word);

    if(len > 1) {
        if(strcmp(word, "//") == 0 || ((word[0] == '/')&&(word[1] == '/'))) {
            start_arr[SINGLE] = number;
            check = 2;
        } else if(strcmp(word, "/*") == 0 || ((word[0] == '/')&&(word[0] == '*'))) {
            special_count[MULTIPLE_COMMENT]++;
            start_arr[MULTIPLE] = number;
            check = 2;
        } else if(strcmp(word, "*/") == 0 || ((word[len-3] == '*')&&(word[len-2] == '/'))) {

            if(special_count[MULTIPLE_COMMENT] > 0) {
                special_count[MULTIPLE_COMMENT]--;
                check_end[MULTIPLE] = true;
            } else {
                printf("ERROR: At line %d, missing operator '/*' \n",row_count);
            }
            check = 2;
        }
    }

    return check;
}

int check_is_CHAR(char word[], int row_count) {
    int check = 0;
    if(word[0] == '\'') {
        if(word[strlen(word)-1] == '\'') {
            if(strlen(word) == 3) {
                check = true;
                check = 1;
            } else {
                printf("ERROR: At line %d, wrong char.\n",row_count);
                check = 2;
            }
        } else {
            if(strlen(word) <3){
                printf("ERROR: At line %d, missing operator ' .\n",row_count);
                check = 2;
            }
        }
    }

    return check;
}

int check_is_STR(char word[], int row_count, int number) {
    int check = 0;

    if(start_arr[STRING] == -1 && word[0] == '"') {
        start_arr[STRING] = number;
        start_arr[STRING + 1] = token_split[0];
        check = 2;
    }

    if(start_arr[STRING] != -1) {
        for(int i = 1; i<strlen(word); i++) {
            if(word[i] == '"' && word[i-1] != '\\') {
                start_arr[STRING + 2] = token_split[0] + i;
                check_end[STRING] = true;
                token_split[0] += (i+1);

                check = 3;
            }
        }
    }

    return check;
}

int check_is_INTE(char word[]) {
    if(strlen(word) == 0) return 0;
    for(int i = 0; i<strlen(word); i++) {
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
    strtod(word, &endptr);

    if (word == endptr) {
        return 0;
    }

    while (isspace((unsigned char ) *endptr))
        endptr++;

    if(*endptr == '\0') return 1;
    else return 0;
}

int check_is_PREP(char word[]) {
    if(word[0] == '#') {
        is_PREP = true;
        return 2;
    }

    return 0;
}

int check_is_IDEN(char word[]) {
    int check = 0;
    if(isalpha(word[0]) != 0 || word[0] == '_') {
        check = 1;
        for(int i = 1; i < strlen(word); i++) {
            if(isalpha(word[i]) == 0 && word[0] != '_' && isdigit(word[i]) == 0) {
                check = 0;
                break;
            }
        }
    }

    return check;
}

void scan_word(char word[], int row_count, int number) {
    int state = 0;
    char *type = "";
    char current_word[1000] = "";
    int count = 0;
    for(int a = token_split[0]; a<token_split[1]; a++) {
        current_word[count] = word[a];
        count ++;
    }

    if(start_arr[SINGLE] == -1 && !is_PREP) {
        if(start_arr[STRING] != -1 && !check_end[STRING]) {
            state = check_is_STR(current_word, row_count, number);

            if(state == 0) state = 1; // Check once. If not, move to next token.
        } else if(start_arr[MULTIPLE] != -1 && !check_end[MULTIPLE]) {
            state = check_is_COMMENT(current_word, row_count, number);

            if(state == 0) state = 2; // Check once. If not, move to next token.
        } else {
            if((state = check_is_COMMENT(current_word, row_count, number)) != 0) {
            } else if((state = check_is_PREP(current_word)) != 0) {
            } else if((state = check_is_SPEC(current_word, row_count)) != 0) {
                type = "SPEC";
            } else if((state = check_is_REWD(current_word)) != 0) {
                type = "REWD";
            } else if((state = check_is_CHAR(current_word, row_count)) != 0) {
                type = "CHAR";
            } else if((state = check_is_STR(current_word, row_count, number)) != 0) {
            } else if((state = check_is_INTE(current_word)) != 0) {
                type = "INTE";
            } else if((state = check_is_FLOAT(current_word)) != 0) {
                type = "FLOT";
            } else if((state = check_is_OPER(current_word, row_count)) != 0) {
                type = "OPER";
            } else if((state = check_is_IDEN(current_word)) != 0) {
                type = "IDEN";
            }
        }

        if(state != 0) {
            if(state != 3) {
                token_split[0] = token_split[1];
            }
            token_split[1] = token_len;

            if(state == 1) {
                printf("%d   %s   %s\n",row_count,type,current_word);
            }

        } else {
            token_split[1]--;
            if(token_split[1] == token_split[0]) token_split[0] = token_len;
        }

    } else {
        token_split[0] = token_split[1];
    }
}

int main() {
    char file_name[50];
    char read_word[1000];
    char *split_word[1000] = {0};
    FILE *fpr;
    int row_count = 0, word_count;

    printf("Enter the input file name:\n");
    scanf("%s", &file_name);

    fpr = fopen(file_name, "r");

    while(!feof(fpr)) {
        ++row_count;

        memset(read_word, 0, sizeof(read_word));
        fgets(read_word, sizeof(read_word) - 1, fpr);
        split(read_word, " ", split_word, &word_count);

        for(int i=0; i<word_count; i++) {
            token_len = strlen(split_word[i]);
            token_split[0] = 0;
            token_split[1] = token_len;

            do {
                scan_word(split_word[i], row_count, i);
            } while(token_split[0] != token_len);

            if(start_arr[MULTIPLE] != -1 && check_end[MULTIPLE]) {
                char tem_word[1000]= "";
                for(int j = start_arr[MULTIPLE]; j <= i; j++) {
                    strcat(tem_word, split_word[j]);
                    strcat(tem_word, " ");
                }

                printf("%d   MC   %s\n",row_count, tem_word);
                start_arr[MULTIPLE] = -1;
                check_end[MULTIPLE] = false;
            } else if(start_arr[STRING] != -1 && check_end[STRING]) {
                char tem_word[1000]= "";

                for(int j = start_arr[STRING]; j <= i; j++) {
                    if(start_arr[STRING] == i) {
                        for(int a = start_arr[STRING+1]; a <= start_arr[STRING+2]; a++) {

                            char cToStr[2] = {'\0', '\0'};
                            cToStr[0] = split_word[j][a];
                            strcat(tem_word, cToStr);
                        }
                    } else if(j == start_arr[STRING]) {
                        for(int a = start_arr[STRING+1]; a< strlen(split_word[j]); a++) {
                            char cToStr[2] = {'\0', '\0'};
                            cToStr[0] = split_word[j][a];
                            strcat(tem_word, cToStr);
                        }
                    } else if(j == i) {
                        for(int a = 0; a <= start_arr[STRING+2]; a++) {
                            char cToStr[2] = {'\0', '\0'};
                            cToStr[0] = split_word[j][a];
                            strcat(tem_word, cToStr);
                        }
                    } else {
                        strcat(tem_word, split_word[j]);
                    }

                    strcat(tem_word, " ");
                }

                printf("%d   STR   %s\n",row_count, tem_word);
                start_arr[STRING] = -1;
                check_end[STRING] = false;
            }
        }

        if(start_arr[SINGLE] != -1) {
            char tem_word[1000] = "";
            for(int j = start_arr[SINGLE]; j < word_count; j++) {
                strcat(tem_word, split_word[j]);
                strcat(tem_word, " ");
            }

            printf("%d   SC   %s\n",row_count, tem_word);
            start_arr[SINGLE] = -1;
        } else if(start_arr[MULTIPLE] != -1) {
            char tem_word[1000] = "";
            for(int j = start_arr[MULTIPLE]; j < word_count; j++) {
                strcat(tem_word, split_word[j]);
                strcat(tem_word, " ");
            }
            printf("%d   MC   %s\n",row_count, tem_word);

            start_arr[MULTIPLE] = 0;
        } else if(start_arr[STRING] != -1) {
            printf("ERROR: At line %d, missing \" for the string .\n", row_count);
            start_arr[STRING] = -1;
        } else if(is_PREP) {
            char tem_word[1000] = "";
            for(int j = 0; j < word_count; j++) {
                strcat(tem_word, split_word[j]);
                strcat(tem_word, " ");
            }

            printf("%d   PREP   %s\n",row_count, tem_word);
            is_PREP = false;
        }
    }

    fclose(fpr);

    return 0;
}
