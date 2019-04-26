#include<stdio.h>
#include<string.h>
#include<unistd.h>    //close()を使うため
#include<sys/socket.h>
#include<arpa/inet.h>

void commun(int sock){
	char buf[256];
	int len_r;       //受信文字数
	char *message="チャチャタウン小倉（チャチャタウンこくら）は、福岡県北九州市小倉北区砂津にある西日本鉄道経営の複合商業施設。通称「チャチャタウン」、「チャチャ」。";
	send(sock, message, strlen(message), 0);
	len_r=recv(sock, buf, 256, 0);
	buf[len_r]='\0';
	
	printf("%s\n", buf);
}

int main(int argc, char **argv){
	int sock=socket(PF_INET,SOCK_STREAM,0);
	//printf("sock is %d \n", sock);
	
	struct sockaddr_in target;
	target.sin_family=AF_INET;
	target.sin_addr.s_addr=inet_addr("10.13.64.20");
	target.sin_port=htons(10001);
	
	connect(sock,(struct sockaddr*)&target, sizeof(target));
	commun(sock);
	
	close(sock);
	
	return 0;
}