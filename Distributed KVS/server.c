#include "util.h"

int SERVER_PORT; // 서버 포트번호
char* kv[DATASET_SIZE]; // 정적 key value stores
void init_kvs(){
  for(int i =0;i<DATASET_SIZE;i++){
		kv[i] = malloc(VALUE_SIZE);
		strcpy(kv[i], "DDDCCCCBBBBAAAA");
		//printf("%s\n",kv[i]);
	}

}

static volatile int quit = 0; // Trigger conditions for SIGINT
void signal_handler(int signum) {
	if(signum == SIGINT){  // Functions for Ctrl+C (SIGINT)
		quit = 1;
	}
}

int main(int argc, char *argv[]) {
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

	init_kvs(); // key-value store 초기화

	int n = 0;
 	struct KVS RecvMsg; // 수신용으로 쓸 메시지 구조체 생성

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  	socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;
	while (!quit) {
		n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len); // sock 소켓으로부터 받은 데이터를 RecvMsg 구조체 변수에 저장, src_addr은 송신자(클라이언트)의 정보를 저장
		if (n > 0) {
			printf("Type: %s Key: %s Value: %s\n", get_type(RecvMsg), RecvMsg.key,RecvMsg.value); // 클라이언트로부터 수신한 메시지 출력 (type, key, value)

			int index = 0; // index 변수 초기화
			index = hash64(RecvMsg.key) % DATASET_SIZE; // hash64 해시 함수를 적용 후 DATASET_SIZE로 modula 연산하여 index 값 구하기
			
			if (RecvMsg.type == READ_REQ) { // READ_REQ (읽기 요청)
				RecvMsg.type = READ_REP; // type 변경
				strcpy(RecvMsg.value, kv[index]); // index에 access해서 RecvMsg.value 필드에 index에 해당하는 value 복사
			}
			else if (RecvMsg.type == WRITE_REQ) { // WRITE_REQ (쓰기 요청)
				RecvMsg.type = WRITE_REP; // type 변경
				strcpy(kv[index], RecvMsg.value); // KVS에 해당하는 index에 패킷의 value 필드값을 복사해서 넣어주기
				memset(RecvMsg.value, '\0', sizeof(RecvMsg.value)); // 메모리 초기화시켜서 value field는 없애기
			}

			sendto(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); //  sock 소켓을 통해 RecvMsg 구조체 변수에 있는 데이터를 RecvMsg 크기만큼 src_addr에(클라이언트) 송신
		}
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}