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
	int len_r;       //��M������
	char *message = "�ʏ́u�`���`���^�E���v�A�u�`���`���v";
	
	if((len_r=recv(sock, buf, BUF_SIZE, 0)) <= 0)
		DieWithError("recv()failed");
	buf[len_r]='\0';
	
	printf("%s\n", buf);
	
	if(send(sock, message, strlen(buf), 0) != strlen(buf))
		DieWithError("send()sent a message of unexpected bytes");
}

int main(int argc, char **argv){
	int servSock = socket(PF_INET,SOCK_STREAM,0);   //�҂��󂯐�p�\�P�b�g
	int cliSock;                                    //�ʐM��p�\�P�b�g
	struct sockaddr_in servAddress;
	struct sockaddr_in clientAddress;
	unsigned int szClientAddr;
	
	servAddress.sin_family = AF_INET;
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddress.sin_port = htons(80);          //�|�[�g�ԍ�
	bind(servSock,(struct sockaddr *)&servAddress,sizeof(servAddress));
	listen(servSock,5);    //���ԑ҂��ł���N���C�A���g��  �����ł�5�iOS�ɂ���ċ������قȂ�j ������5�ڑ��ł���킯�ł͂Ȃ�
	while(1){
		szClientAddr = sizeof(clientAddress);
		cliSock = accept(servSock,(struct sockaddr *)&clientAddress,&szClientAddr);
		commun(cliSock);
		close(cliSock);
	}
	
	close(servSock);
	
	return 0;
}