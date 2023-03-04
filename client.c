#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024

void registToServer(int client_sock);
void loginToServer(int client_sock);
void menu(int client_sock);
int receiveData(int s, char *buff, int flags);
int sendData(int s, char *buff, int flags);
void chuanhoaxau(char *chuoi);

int main(){
	int client_sock;
	char buff[BUFF_SIZE+1];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately!\n");
		return 0;
	}

    //Step 4: Login or Register
    menu(client_sock);

    //Step 5: Game1
		


	//Step n: Close socket
	close(client_sock);
	return 0;
}

void chuanhoaxau(char *chuoi){
    int pos=0;
    for(pos=0;pos<strlen(chuoi);pos++){
        if(chuoi[pos]=='\r'||chuoi[pos]=='\n'){
            chuoi[pos]='\0';
            break;
        }
    }
}

void menu(int client_sock){
    printf("-----------------------\n");
    printf("WELCOME TO 'AI LA TRIEU PHU'\n");
    printf("-----------------------\n");
    printf("1.Register\t2.Sign in\n");
    printf("Your choice (1-2, other to quit):");
    int index;
    char enter;
    scanf("%d%c",&index,&enter);
    int bytes_sent = send(client_sock, &index, sizeof(int), 0); if(bytes_sent < 0) perror("\nError: ");
    switch (index){
        case 1: 
            printf("-----------------------\nRegister function\n");
            registToServer(client_sock);
        case 2: 
            printf("-----------------------\nSign_in function\n");
            loginToServer(client_sock); break;
        default: exit(1);
    }
}

void registToServer(int client_sock){
    char buff[BUFF_SIZE];
    int indication_signal;
    int bytes_sent;
    //send Username, password and confirm password to server
	printf("Username:");
        memset(buff,'\0',(strlen(buff)+1));
        fgets(buff, BUFF_SIZE, stdin);
        chuanhoaxau(buff);
        bytes_sent = sendData(client_sock,buff,0);
    printf("Password:");
        memset(buff,'\0',(strlen(buff)+1));
        fgets(buff, BUFF_SIZE, stdin);
        chuanhoaxau(buff);
        bytes_sent = sendData(client_sock,buff,0);
    printf("Confirm password:");
        memset(buff,'\0',(strlen(buff)+1));
        fgets(buff, BUFF_SIZE, stdin);
        chuanhoaxau(buff);
        bytes_sent = sendData(client_sock,buff,0);
    //receive indication signal to know success or fail
	int bytes_received = recv(client_sock, &indication_signal, sizeof(int), 0);
	if (bytes_received < 0)	perror("\nError: ");
    else if (indication_signal == 0){
        printf("Successful Registration\n");
    }else if (indication_signal == 1){
        printf("Username already exists or confirm password do not same with password\n");
        registToServer(client_sock);
    }else if (indication_signal == 2){
        printf("Error from server!\n");
        exit(0);
    }
}

void loginToServer(int client_sock){
    char username[30], password[40];
    int indication_signal;
    int bytes_sent;
    //send Username, password
	printf("Username:");
        memset(username,'\0',30);
        fgets(username, 30, stdin);
        chuanhoaxau(username);
        bytes_sent = sendData(client_sock,username,0);
    printf("Password:");
        memset(password,'\0',40);
        fgets(password, 40, stdin);
        chuanhoaxau(password);
        bytes_sent = sendData(client_sock,password,0);
    //receive indication signal to know login success or fail
	int bytes_received = recv(client_sock, &indication_signal, sizeof(int), 0);
	if (bytes_received < 0)	perror("\nError: ");
    else if (indication_signal == 1){
        printf("You have entered the wrong account or password. Please try again.\n");
        loginToServer(client_sock);
    }
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
    int n;
	//send size of message
	n = send(s, &size, sizeof(int), flags);
	if(n < 0)
		perror("Error: ");
	buff[size] = '\0';
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}