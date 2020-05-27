#include <stdio.h>
#include <stdlib.h>

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

void scan_word(char word[]) {
  //  printf("%s", word);
    int i;
    for(i=0;i< strlen(word);i++){
        printf("%s\n",word[i]);
    }
}

int main()
{
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

        for(i=0;i<word_count-1;i++){
            printf("%s", split_word[i]);
            scan_word(split_word[i]);
        }

    }

    fclose(fpr);

    return 0;
}
