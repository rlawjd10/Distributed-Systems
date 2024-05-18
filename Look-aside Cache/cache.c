#include "util.h"

int SERVER_PORT; // 서버 포트번호 5001

static volatile int quit = 0; // Trigger conditions for SIGINT
void signal_handler(int signum) {
	if(signum == SIGINT){  // Functions for Ctrl+C (SIGINT)
		quit = 1;
	}
}

int main(int argc, char *argv[]) {
	
	srand((unsigned int)time(NULL));  // 난수 발생기 초기화
	// 프로그램 시작시 입력받은 매개변수를 parsing한다.
	if ( argc < 2 ){
	 printf("Input : %s port number\n", argv[0]);
	 return 1;
	}

	signal(SIGINT, signal_handler); // SIGINT에 대한 핸들러 등록

	SERVER_PORT = atoi(argv[1]); // 입력받은 argument를 포트번호 변수에 넣어준다.

	// 서버의 정보를 담을 소켓 구조체 생성 및 초기화
	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SERVER_PORT);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 i.e., 자기 자신의 IP

	// 소켓을 생성한다.
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not create listen socket\n");
		exit(1);
	}

	// 생성한 소켓에 소켓구조체를 bind시킨다.
	if ((bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr))) < 0) {
		printf("Could not bind socket\n");
		exit(1);
	}


	int n = 0;
  struct KVS RecvMsg={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;
	while (!quit) {

		int random = 0;	// 1/2의 cache 확률을 위한 random 변수
		n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len); // sock 소켓으로부터 받은 데이터를 RecvMgs 구조체 변수에 저장, src_addr은 수신자(클라이언트)의 정보를 저장한다.
		if (n > 0) { // 데이터를 1바이트 이상 수신했다면
			random = rand() % 2; // cache를 50%의 확률로 하기 위함 (0 또는 1)

			if (random == 0) { // cache_miss
				RecvMsg.type = CACHE_MISS;	// util.h에서 Cache 관련 매크로 추가
				printf("Cache Miss for Key: %s\n", RecvMsg.key);
			}
			else { // cache_hit
				RecvMsg.type = CACHE_HIT;
				strcpy(RecvMsg.value, "CACHECACHECACHE"); // cache hit이 발생하면 value로 CACHECACHECACHE를 반환한다.
				printf("Cache Hit for Key: %s\n", RecvMsg.key);
			}

			sendto(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); // sock 소켓의 RecvMsg 구조체 변수에 정보를 가지고 src_addr에 저장된 클라이언트의 주소로 송신을 한다.

		}
  
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}
