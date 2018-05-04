#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int sock;

void signal_handler(int sig) {
	printf("signal_handler...\n");
	close(sock);
	exit(0);
}

int main(int argc, char **argv) {
	struct sockaddr_in addr;
	int n;
	socklen_t len = sizeof(struct sockaddr_in);
	char buff[512];

	if((sock=socket(AF_INET, SOCK_DGRAM, 0)) <0) {  
		perror("socket");
		return 1;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3702);
	addr.sin_addr.s_addr = inet_addr("192.168.1.74");
	if(addr.sin_addr.s_addr == INADDR_NONE) {
		perror("Incorrect ip address!");
		close(sock);
		return 1;
	}

	signal(SIGINT, signal_handler);

	printf("This is a UDP client\n");
	
	for(;;) {  
		// 接受用户输入
		bzero(buff, 512);
		
		//从键盘输入内容
		if(fgets(buff, 511, stdin) != buff) {
			printf("\nEOF\n");
			break;
		}
		
		if(sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)&addr, len) < 0) {
			perror("sendto");
			break;
		} else {
			len = 0;
			n = recvfrom(sock, buff, 511, 0, (struct sockaddr*)&addr, &len);
			if(n>0) {
				buff[n] = 0;
				printf("recvfrom(%3d): %s", n, buff);
			} else {
				perror("recvfrom");
			}
		}
	}
	
	printf("close...\n");
	close(sock);

	return 0;
}
