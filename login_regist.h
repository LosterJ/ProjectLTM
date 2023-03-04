typedef struct node_User{
    char username[40];
    char password[50];
    int status;
    struct node_User *pNext;
} NODE_USER;

NODE_USER* createNode(char* username,char* password,int status){
    NODE_USER *newNode=(NODE_USER*)malloc(sizeof(NODE_USER));
    strcpy(newNode->username,username);
    strcpy(newNode->password,password);
    newNode->status = status;
    newNode->pNext = NULL;
    return newNode;
}

NODE_USER* loadfileUser(char *filename){
    NODE_USER *head_User=NULL;
    NODE_USER *newNode;
    FILE* fptr = fopen(filename,"r");
    if (fptr==NULL){
        printf("Can not open this file"); exit(1);
    }
    do {
        char username[30], password[30];
        int status;
        fscanf(fptr,"%s %s %d",username,password,&status);
        if(head_User==NULL){
            head_User=createNode(username,password,status);
        } else {
            newNode = createNode(username,password,status);
            newNode->pNext = head_User;
            head_User=newNode;
        }
    } while(getc(fptr)!=EOF);
    fclose(fptr);
    return head_User;
}

void changefile(NODE_USER* head){
    FILE *fptr = fopen("account.txt","w+");
    if (fptr == NULL){
        printf("Change file error!!!"); return;
    }
    while(NULL!=head){
        fprintf(fptr,"%s %s %d\n\n",head->username,head->password,head->status);
        head = head->pNext;
        if (head->pNext == NULL){
            fprintf(fptr,"%s %s %d",head->username,head->password,head->status);
            break;
        }
    }
    fclose(fptr);
}

void removeLinklist(NODE_USER ** head_User){
    NODE_USER* temp;
    while(*head_User != NULL){
        temp = *head_User;
        *head_User = (*head_User)->pNext;
        free(temp);
    }
    exit(1);
}