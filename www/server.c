#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BUF_SIZE 256

void DieWithError(char *errorMessage){
	perror(errorMessage);
	exit(1);
}

void commun(int sock){
	char buf[BUF_SIZE];
	int len_r;       //受信文字数
	char *message = "通称「チャチャタウン」、「チャチャ」";
	char response[BUF_SIZE];
	
	while((len_r=recv(sock, buf, BUF_SIZE, 0)) > 0){
		buf[len_r]='\0';
		
		printf("%s\n", buf);
		
		if(strstr(buf, "\r\n\r\n")){    //改行\r\n\r\n
			break;
		}
	}
	
	if(len_r <= 0)
		DieWithError("recv()failed");
	
	printf("received HTTP Request.\n");
	
	sprintf(response, "HTTP/1.1 200 OK\r\n");
	if(send(sock, response, strlen(response), 0) != strlen(response))
		DieWithError("send()sent a message of unexpected bytes");
}

int main(int argc, char **argv){
	int servSock = socket(PF_INET,SOCK_STREAM,0);   //待ち受け専用ソケット
	int cliSock;                                    //通信専用ソケット
	struct sockaddr_in servAddress;
	struct sockaddr_in clientAddress;
	unsigned int szClientAddr;
	
	servAddress.sin_family = AF_INET;
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddress.sin_port = htons(80);          //ポート番号
	bind(servSock,(struct sockaddr *)&servAddress,sizeof(servAddress));
	listen(servSock,5);    //順番待ちできるクライアント数  ここでは5（OSによって挙動が異なる） 同時に5個接続できるわけではない
	while(1){
		szClientAddr = sizeof(clientAddress);
		cliSock = accept(servSock,(struct sockaddr *)&clientAddress,&szClientAddr);
		commun(cliSock);
		close(cliSock);
	}
	
	close(servSock);
	
	return 0;
}