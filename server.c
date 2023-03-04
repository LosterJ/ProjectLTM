#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "login_regist.h"
#include "load_quest.h"

#define PORT 5500   /* Port that will be opened */ 
#define BACKLOG 8   /* Number of allowed connections */
#define BUFF_SIZE 1024

char buff[BUFF_SIZE];//buff of all

int receiveData(int s, char *buff, int flags);
int sendData(int s, char *buff, int flags);
//registFunc return 0 when fail, 1 when ok
void regist_login(int connfd,NODE_USER *now_Client);
NODE_USER* registFunc(NODE_USER *head_User,int connfd);
NODE_USER* loginFunc(NODE_USER *head_User,int connfd);
void rand_quest(NODE_QUEST *head_Quest,int numOfQuest,NODE_QUEST *chosenQuest);
void updatePoint(NODE_USER *user);

int main()
{
	int opt = 1;
	int max_clients = 8;
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[8];
	NODE_USER* client_info[8];
	ssize_t	ret;
	fd_set	readfds, writefds, allset;
	char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	int num_player=0;

	//initiaise all client[] to -1 so not check
	for (i=0;i<max_clients;i++){
		client[i]=-1;	/* -1 indicates available entry */
	}

	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt)) < 0){
		perror("setsockopt");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(PORT);

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxi = -1;				/* index into client[] array */
	
	
	//Step 4: Communicate with clients
	while (1) {
		//clear the socket set
		FD_ZERO(&readfds);
		
		//add master socket to set
		FD_SET(listenfd, &readfds);
		maxfd = listenfd;

		for (i = 0; i < max_clients; i++){
			//socket descriptor
			sockfd = client[i];

			//if valid socket descriptor the add to read list 
            if(sockfd > 0)  
                FD_SET( sockfd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sockfd > maxfd)  
                maxfd = sockfd;  
		}

		//wait for an activity on one of the sockets, timeout is NULL , 
        //so wait indefinitely 
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		//If something happened on the master socket, 
        //then its an incoming connection
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				//User dang ky dang nhap
				NODE_USER* nowClient=NULL;
				regist_login(connfd,nowClient);
				
				for (i = 0; i < max_clients; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						printf("Adding to list of sockets\n");
						break;
						client_info[i] = nowClient;
					}
				if (i == max_clients){
					printf("\nToo many clients");
					close(connfd);
				}
			}
		}

		//else its some IO operation on some other socket
		for (i = 0; i <= max_clients; i++) {	/* check all clients for data */
			
			sockfd = client[i];

			if (FD_ISSET(sockfd, &readfds)) {
				bzero(&rcvBuff, BUFF_SIZE);
				bzero(&buff, BUFF_SIZE);
				int mode;
				//check if it was for closing
				if (recv(connfd, &mode, sizeof(int), 0) <= 0){
					printf("Client[%d] out\n",i);
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
				}
				//play game
				else {
					switch(mode){
						case 1: 
							printf("Play game\n");
							NODE_QUEST *head_Quest=NULL;
							NODE_QUEST *chosenQuest=NULL;
							int numOfQuest=0;
							head_Quest=loadfileQuest("quest.txt",&numOfQuest);
							rand_quest(head_Quest,numOfQuest,chosenQuest);
							sendData(sockfd,chosenQuest->quest,0);
							sendData(sockfd,chosenQuest->ans1,0);
							sendData(sockfd,chosenQuest->ans2,0);
							sendData(sockfd,chosenQuest->ans3,0);
							sendData(sockfd,chosenQuest->ans4,0);
							printf("Sended RandQuest\n");
							int user_ans;
							if(recv(sockfd,&user_ans,sizeof(int),0)<=0) perror("Error: ");
							else{
								if(user_ans == chosenQuest->true_ans){
									updatePoint(client_info[i]);
								}
							}
						case 2: 
							printf("Winner board\n");
						default:
					}
				}
			}
		}
	}
	return 0;
}

int receiveData(int s, char *buff, int flags){
	int size;
	int n = recv(s, &size, sizeof(int), flags);
	if(n < 0)
		perror("Error: ");
	n = recv(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	buff[size] = '\0';
	return n;
}

int sendData(int s, char *buff, int flags){
	int size = strlen(buff);
	int n = send(s, &size, sizeof(int), flags);
	if(n < 0)
		perror("Error: ");
	buff[size] = '\0';
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

void regist_login(int connfd,NODE_USER* nowClient){
	//load account to linklist
	NODE_USER *head_User=NULL;
	NODE_USER *now_User=nowClient;
	head_User=loadfileUser("account.txt");
	printf("Loaded account.txt\n");
	//receive mode
	int mode;
	int n = recv(connfd, &mode, sizeof(int), 0);
	if(n < 0)
		perror("Error: ");
	switch (mode){
		case 1: 
		printf("Register function\n");
		head_User=registFunc(head_User,connfd);
		case 2: 
		printf("Sign_in function\n");
		now_User=loginFunc(head_User,connfd);
		//default: removeLinklist(&head_User);
	}
}

//return 1 is fail
NODE_USER* registFunc(NODE_USER *head_User,int connfd){
    char user_name[31];
	char password[41];
	char confirm_password[41];
    NODE_USER *checkNode = head_User;
	receiveData(connfd,user_name,0);
	receiveData(connfd,password,0);
	receiveData(connfd,confirm_password,0);
    
	int checkindex = strcmp(password,confirm_password);
		if (checkindex != 0){
            printf("Password not match with confirm password!\n");
			int err_msg = 1;
            int n = send(connfd,&err_msg,sizeof(int),0); if(n < 0) perror("Error: ");
            registFunc(head_User,connfd);
        }
	while(NULL!=checkNode){
        int checkindex = strcmp(user_name,checkNode->username);
        if (checkindex == 0){
            printf("This username already exists!\n");
			int err_msg = 1;
            int n = send(connfd,&err_msg,sizeof(int),0); if(n < 0) perror("Error: ");
            registFunc(head_User,connfd);
        }
        checkNode=checkNode->pNext;
    }
    checkNode = createNode(user_name,password,1);
    checkNode->pNext = head_User; head_User = checkNode;

    FILE* fptr = fopen("account.txt","a");
    if (fptr==NULL){
        printf("Something wrong when registering!!!");
		int err_msg = 2;
		int n = send(connfd,&err_msg,sizeof(int),0);if(n < 0) perror("Error: ");
		return head_User;
    }
    fprintf(fptr,"\n\n%s %s %d",user_name,password,1);
    fclose(fptr);
	int msg = 0;
	int n = send(connfd,&msg,sizeof(int),0);if(n < 0) perror("Error: ");
    return head_User;
}

//return 1 is fail
NODE_USER* loginFunc(NODE_USER *head_User, int connfd){
    char user_name[31];
	char password[41];
    NODE_USER *checkNode = head_User;
	receiveData(connfd,user_name,0);
	receiveData(connfd,password,0);
    while(NULL!=checkNode){
        int checkindex = strcmp(user_name,checkNode->username);
        if (checkindex == 0){
            //Co ton tai, tiep theo kiem tra password
            checkindex = strcmp(password,checkNode->password);
			if (checkindex==0){//dung pass
				int message = 0;
				int n = send(connfd,&message,sizeof(int),0);if(n < 0) perror("Error: ");
				return checkNode;
			}else{//sai pass
				int message = 1;
				int n = send(connfd,&message,sizeof(int),0);if(n < 0) perror("Error: ");
				loginFunc(head_User,connfd);
				return head_User;
			}
        }//khong trung username
        checkNode=checkNode->pNext;
    }
	//khong co username tuong ung
    int message = 1;
	int n = send(connfd,&message,sizeof(int),0);if(n < 0) perror("Error: ");
	loginFunc(head_User,connfd);
}

//Chon ngau nhien cau hoi
void rand_quest(NODE_QUEST *head_Quest,int numOfQuest,NODE_QUEST *chosenQuest){
    srand(time(NULL));
    int num_quest = rand() % numOfQuest;
    //printf("NumQuest = %d\n",num_quest);
    NODE_QUEST *chosenQuest = head_Quest;
    for (int i=0; i<num_quest;i++) chosenQuest = chosenQuest->pNext;
    //printf("%s %s %d\n",chosenQuest->quest,chosenQuest->ans1,chosenQuest->true_ans);
	removeLinklistQuest(&head_Quest);
    return 0;
}

void updatePoint(NODE_USER *user){
	NODE_USER *head_User=loadfileUser("account.txt");
	printf("Loaded account.txt\n");

    NODE_USER *checkNode = head_User;
    while(NULL!=checkNode){
        int checkindex = strcmp(user->username,checkNode->username);
        if (checkindex == 0){
            (checkNode->status)++;
        }//khong trung username
        checkNode=checkNode->pNext;
    }
	changefile(head_User);
}