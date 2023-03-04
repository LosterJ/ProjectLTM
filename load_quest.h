#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct node_quest{
    char quest[450];
    char ans1[30], ans2[30], ans3[30], ans4[30];
    int true_ans;
    struct node_quest *pNext;
} NODE_QUEST;

void chuanhoaxau(char *chuoi){
    int pos=0;
    for(pos=0;pos<strlen(chuoi);pos++){
        if(chuoi[pos]=='\r'||chuoi[pos]=='\n'){
            chuoi[pos]='\0';
            break;
        }
    }
}

NODE_QUEST* createNodeQuest(char* quest,char* ans1,char* ans2,char* ans3,char* ans4,int true_ans){
    NODE_QUEST *newNode=(NODE_QUEST*)malloc(sizeof(NODE_QUEST));
    strcpy(newNode->quest,quest);
    strcpy(newNode->ans1,ans1);
    strcpy(newNode->ans2,ans2);
    strcpy(newNode->ans3,ans3);
    strcpy(newNode->ans4,ans4);
    newNode->true_ans = true_ans;
    newNode->pNext = NULL;
    return newNode;
}

NODE_QUEST* loadfileQuest(char *filename,int* numOfQuest){
    NODE_QUEST *head_Quest=NULL;
    NODE_QUEST *newNode;
    FILE* fptr = fopen(filename,"r");
    if (fptr==NULL){
        printf("Can not open this file"); exit(1);
    }
    do {
        char quest[400], ans1[20], ans2[20], ans3[20], ans4[20];
        int true_ans;
        int enter;
        fgets(quest,400,fptr);fgets(ans1,20,fptr);fgets(ans2,20,fptr);fgets(ans3,20,fptr);fgets(ans4,20,fptr);
        chuanhoaxau(quest);chuanhoaxau(ans1);chuanhoaxau(ans2);chuanhoaxau(ans3);chuanhoaxau(ans4);
        fscanf(fptr,"%d",&true_ans);
        if(head_Quest==NULL){
            head_Quest=createNodeQuest(quest,ans1,ans2,ans3,ans4,true_ans);
        } else {
            newNode = createNodeQuest(quest,ans1,ans2,ans3,ans4,true_ans);
            newNode->pNext = head_Quest;
            head_Quest=newNode;
        }
        (*numOfQuest)++;
        //printf("Quest: %s\nAns: %s %s %s %s %d\nNumOfQuest = %d\n",quest,ans1,ans2,ans3,ans4,true_ans,*numOfQuest);
    } while(getc(fptr)!=EOF);
    fclose(fptr);
    return head_Quest;
}

//Giai phong linklist
void removeLinklistQuest(NODE_QUEST ** head_Quest){
    NODE_QUEST* temp;
    while(*head_Quest != NULL){
        temp = *head_Quest;
        *head_Quest = (*head_Quest)->pNext;
        free(temp);
    }
    return;
}