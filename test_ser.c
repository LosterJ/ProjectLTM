#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char username[30];
    printf("Username:");
    memset(username,'\0',30);
    fgets(username, 30, stdin);
    printf("Submit string: %s\n",username);
    return 0;
}