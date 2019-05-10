#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>    //close()を使うため
#include<sys/socket.h>
#include<arpa/inet.h>
#define BUF_SIZE 256

void DieWithError(char *errorMessage){
	perror(errorMessage);
	exit(1);
}

void commun(int sock){
	char buf[BUF_SIZE];
	int len_r;       //受信文字数
	char *message="通称「チャチャタウン」、「チャチャ」";
	
	if(send(sock, message, strlen(message), 0) != strlen(message))  //nagao　元＝戻り値・・・おｋ
		DieWithError("send()sent a message of unexpected bytes");
	if((len_r=recv(sock, buf, BUF_SIZE, 0)) <= 0)
		DieWithError("recv()failed");
	buf[len_r]='\0';
	
	printf("%s\n", buf);
}

int main(int argc, char **argv){      //実行時　./cliant 10.13.64.20 10001 ・・・引数3個（cliantも含む） argc・・・引数の数　argv・・・引数の値
	if(argc != 3)
		DieWithError("arguments is not available");
	char *server_ipaddr = argv[1];  //"10.13.64.20"
	int server_port = atoi(argv[2]);  //10001
	int sock=socket(PF_INET,SOCK_STREAM,0);   //socket関数戻り値　0以上・・・成功　　-1・・・エラー
	
	if(sock < 0)
		DieWithError("socket()failed");
	
	//printf("sock is %d \n", sock);
	
	struct sockaddr_in target;
	target.sin_family=AF_INET;
	target.sin_addr.s_addr=inet_addr(server_ipaddr);
	target.sin_port=htons(server_port);
	
	if(connect(sock,(struct sockaddr*)&target, sizeof(target)) < 0)  //connect関数戻り値　0以上・・・正常時　　-1・・・エラー
		DieWithError("connect()failed");
	
	commun(sock);
	
	close(sock);
	
	return 0;
}