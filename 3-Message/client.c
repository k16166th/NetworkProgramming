//client ATM

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

void DieWithError(char *);
int prepare_client_socket(char *, int);
void my_scanf(char *, int);
void commun(int);
void read_until_delim(int, char *, char, int);

int main(int argc, char *argv[])
{
    if (argc != 3)
        DieWithError("usage: ./client ip_address port");

    int sock = prepare_client_socket(argv[1], atoi(argv[2]));

    commun(sock);

    close(sock);

    return 0;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int prepare_client_socket(char *ipaddr, int port)
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        DieWithError("socket() failed");

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(ipaddr);
    target.sin_port = htons(port);
    if (connect(sock, (struct sockaddr *)&target, sizeof(target)) < 0)
        DieWithError("connect() failed");

    return sock;
}

void my_scanf(char *buf, int num_letter)
{
    //num_letter文字だけ入力させる（scanfの脆弱性対策）
    char format[20];
    sprintf(format, "%s%d%s", "%", num_letter, "s%*[^\n]");
    scanf(format, buf);
    getchar();
}

void read_until_delim(int sock, char *buf, char delimiter, int max_length)
{
    int len_r = 0;        // 受信文字数
    int index_letter = 0; //受信文字数の合計

    while (index_letter < max_length - 1)
    {
        //１文字だけ受信

        if ((len_r = recv(sock, buf + index_letter, 1, 0)) <= 0)
        {
            //エラー
            DieWithError("recv() failed");
        }

        if (buf[index_letter] == delimiter)
            //区切り文字を受信→ループを抜ける
            break;
        else
            index_letter++;
    }

    //nullを末尾に追加
    buf[index_letter] = '\0';
}

int get_current_balance()
{
    return 1000000;
}

void set_current_balance(int new_balance)
{
    return;
}

void commun(int sock)
{
    char cmd[2] = "";                    //コマンド入力用
    char withdraw[MONEY_DIGIT_SIZE + 1]; //引き出し額
    char deposit[MONEY_DIGIT_SIZE + 1];  //預け入れ額
    char msg[BUF_SIZE];                  //送信メッセージ

    printf("0:引き出し 1:預け入れ 2:残高照会\n");
    printf("何をしますか？ > ");

    my_scanf(cmd, 1);

    switch (cmd[0])
    {
    case '0':
        //引き出し処理
        printf("引き出す金額を入力してください > ");
        my_scanf(withdraw, MONEY_DIGIT_SIZE);

        sprintf(msg, "0_%s_", withdraw);
        break;
    case '1':
        //預け入れ処理
        printf("預け入れる金額を入力してください > ");
        my_scanf(deposit, MONEY_DIGIT_SIZE);

        sprintf(msg, "0_%s_", deposit);
        break;
    case '2':
        //残高照会
        strcpy(msg, "0_0_");
        break;
    default:
        //unknownエラー、終了
        printf("番号が確認できませんでした。\n");
        return;
    }

    printf("%lu バイト\n", sizeof(char) * strlen(msg));
    printf("%lu msg\n", sizeof(msg) * strlen(msg));

    //送信処理
    if (send(sock, msg, strlen(msg), 0) != strlen(msg))
        DieWithError("send() sent a message of unexpected bytes");

    //受信処理

    read_until_delim(sock, msg, '_', BUF_SIZE);

    //表示処理
    printf("残高は%d円になりました。\n", atoi(msg));
}