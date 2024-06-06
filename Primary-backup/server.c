#include "util.h"

int SERVER_PORT; // 서버 포트번호
const char* dst_ip = "127.0.0.1";
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

    //////
    /* 서버 구조체 설정 */
	struct sockaddr_in srv_addr2; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr2, 0, sizeof(srv_addr2)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr2.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	inet_pton(AF_INET, dst_ip, &srv_addr2.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

    /* 소켓 생성 */
	int sock2; // 소켓 디스크립터(socket descriptor)를 생성한다.
	if ((sock2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n"); // sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
	}


    struct sockaddr_in src_addr2; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
    socklen_t src_addr_len2 = sizeof(src_addr2);

	struct sockaddr_in srv_addr3; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr3, 0, sizeof(srv_addr3)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr3.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	inet_pton(AF_INET, dst_ip, &srv_addr3.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

    /* 소켓 생성 */
	int sock3; // 소켓 디스크립터(socket descriptor)를 생성한다.
	if ((sock3 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n"); // sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
	}

	int sock4; // 소켓 디스크립터(socket descriptor)를 생성한다.
	if ((sock4 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n"); // sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
	}


    struct sockaddr_in src_addr3; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
    socklen_t src_addr_len3 = sizeof(src_addr3);

	 struct sockaddr_in src_addr4; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
    socklen_t src_addr_len4 = sizeof(src_addr4);
    //////

	init_kvs(); // key-value store 초기화

	int n = 0;
    struct KVS RecvMsg={0,}; // 수신용으로 쓸 메시지 구조체 생성 및 초기화
    struct KVS SendMsg={0,};

	struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
    socklen_t src_addr_len = sizeof(src_addr);
	size_t pkt_size = 0;
	


	while (!quit) {
        if (SERVER_PORT == 5001) {
            n = recvfrom(sock, &RecvMsg, sizeof(RecvMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr, &src_addr_len);
            if (n > 0) {
                if (RecvMsg.type == 0) { // 읽기 요청이면
                    printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg), RecvMsg.key, RecvMsg.value);
                    RecvMsg.type = READ_REP; // type 변경
				    uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;	// KVS 참조하기 위한 index 구하기
				    strcpy(RecvMsg.value, kv[index]); // index에 access해서 RecvMsg.value 필드에 index에 해당하는 value 복사
				    //pkt_size = n + VALUE_SIZE;
                    // client한테 보내기
				    sendto(sock, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr, sizeof(src_addr)); //  sock 소켓을 통해 RecvMsg 구조체 변수에 있는 데이터를 RecvMsg 크기만큼 src_addr에(클라이언트) 송신
				    strcpy(RecvMsg.value,""); //버퍼 초기화
                }
                else {  // 쓰기 요청이면
                    printf("Type: %s Key: %s Value: %s\n",get_type(RecvMsg), RecvMsg.key, RecvMsg.value);	//수신받은 request 정보
					uint64_t index = hash64(RecvMsg.key) % DATASET_SIZE;	// KVS 참조하기 위한 index 구하기
					strcpy(kv[index], RecvMsg.value);	// 요청의 value 필드에 있는 값으로 갱신하기

                    // 5001에서 보내서 5002, 5003이 받도록
                    srv_addr2.sin_port = htons(5002);
                    sendto(sock2, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&srv_addr2, sizeof(srv_addr2)); // 생성한 메시지를 서버로 송신한다.
                    int n2 = recvfrom(sock2, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr2, &src_addr_len2);  //5002번으로부터 수신

                    srv_addr3.sin_port = htons(5003);
                    sendto(sock3, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&srv_addr3, sizeof(srv_addr3)); // 생성한 메시지를 서버로 송신한다.
                    int n3 = recvfrom(sock3, &RecvMsg, sizeof(RecvMsg), 0, (struct sockaddr *)&src_addr3, &src_addr_len3);  //5003번으로부터 수신

					// 클라이언트한테 보내기
                    if (n2 > 0 && n3 > 0) {
                        printf("%s\n\n", RecvMsg.value); // 5002, 5003으로부터 수신을 받았다면 wirte is done을 출력
						strcpy(RecvMsg.value,"");
						pkt_size = sizeof(RecvMsg) - sizeof(RecvMsg.value);
						sendto(sock, &RecvMsg, pkt_size, 0, (struct sockaddr *)&src_addr, sizeof(src_addr));
						strcpy(SendMsg.value,""); //버퍼 초기화
                    }
                }
            }
        }
        else { //server port가 5002, 5003일 때
            n = recvfrom(sock, &SendMsg, sizeof(SendMsg), MSG_DONTWAIT, (struct sockaddr *)&src_addr4, &src_addr_len4); // 5001번으로부터 수신
            if (n > 0) {
                printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg), SendMsg.key, SendMsg.value);	//수신받은 request 정보
				uint64_t index = hash64(SendMsg.key) % DATASET_SIZE;	// KVS 참조하기 위한 index 구하기
				strcpy(kv[index], SendMsg.value); // 5002, 5003번 포트의 서버에서 value값 갱신
				SendMsg.type = WRITE_REP; // type 변경
                strcpy(SendMsg.value, "Write is Done"); // value값으로 write is done 문자열 송신
				printf("%s\n\n", SendMsg.value); // write is done 출력
                sendto(sock, &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&src_addr4, sizeof(src_addr4)); // 5001번의 정보가 담긴 src_addr로 송신
            }
        }
    
	}

	printf("\nCtrl+C pressed. Exit the program after closing the socket\n");
	close(sock);

	return 0;
}

