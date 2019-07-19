#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <regex.h>

#define BUF_SIZE 256

void DieWithError(char *errorMessage){
	perror(errorMessage);
	exit(1);
}

void commun(int sock){
	char buf[BUF_SIZE];
	char buf_old[BUF_SIZE];
	char buf2[2*BUF_SIZE];
	int len_r;                  //受信文字数
	char response[BUF_SIZE];	//これに入れて表示
	
	regex_t regBuf;
	regmatch_t regMatch[1];		//1個だけ検索結果を取得する
	const char *pattern = "GET[^\\n]+HTTP";
	char result[100];
	char *uri;
	
	result[0] = '\0';
	buf_old[0] = '\0';
	
	if(regcomp(&regBuf, pattern, REG_EXTENDED | REG_NEWLINE) != 0){
		DieWithError("regcomp failed");
	}
	
	while((len_r=recv(sock, buf, BUF_SIZE, 0)) > 0){
		buf[len_r] = '\0';
		sprintf(buf2, "%s%s", buf_old, buf);	//buf2にbuf_oldとbufを書き込む
		
		//printf("%s\n", buf);
		
		if(regexec(&regBuf, buf2, 1, regMatch, 0) != 0){
			int startIndex = regMatch[0].rm_so;		//開始位置
			int endIndex = regMatch[0].rm_eo;		//終了位置
			strncpy(result, buf2 + startIndex, endIndex - startIndex);	//resultに
		}
		
		if(strstr(buf2, "\r\n\r\n")){			//改行\r\n\r\n
			break;
		}
		
		sprintf(buf_old, "%s", buf);			//buf_oldにbufを書き込む
	}
	
	if(result[0] != '\0'){
		uri = strtok(result, " ");		//resultから空白文字までをuriに代入
		uri = strtok(NULL, " ");		//その続きから空白文字までをuriに代入
		printf("%s\n", uri);
	}else{
		DieWithError("No URI");
	}
	
	regfree(&regBuf);
	
	if(len_r <= 0)
		DieWithError("recv()failed");
	
	printf("received HTTP Request.\n");
	
	sprintf(response, "HTTP/1.1 200 OK\r\n");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "Content-Type: text/html; charset=utf-8\r\n");	//プログラムと文字コードを合わせる
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
        
    sprintf(response, "\r\n");
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "<!DOCTYPE html><html><head><title>");	//タイトル
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "ネットワークプログラミングのwebサイト");	//タイトル名
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
    
    sprintf(response, "</title></head><body>ネットワークダイスキ</body></html>");	//表示される文
    if(send(sock, response, strlen(response), 0) != strlen(response))
        DieWithError("send() sent a message of unexpected bytes");
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

