#include "util.h"

const char* dst_ip = "127.0.0.1";
#define NUM_SRV 4

// 임의의 key를 생성해서 반환해줌
void generate_key(char* key) {
    uint64_t number = rand() % DATASET_SIZE;
    for (int i = 0; i < 5; ++i) number = ((number << 3) - number + 7) & 0xFFFFFFFFFFFFFFFF;
    key[KEY_SIZE - 1] = '\0';
    for (int i = KEY_SIZE - 2; i >= 0; i--) {
        int index = number % SET_SIZE;
        key[i] = SET[index];
        number /= SET_SIZE;
    }
}

int main(int argc, char *argv[]) {

  srand((unsigned int)time(NULL));  // 난수 발생기 초기화
  /* 서버 구조체 설정 */
	int SERVER_PORT = 5001;
	struct sockaddr_in srv_addr; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
	memset(&srv_addr, 0, sizeof(srv_addr)); // 구조체를 모두 '0'으로 초기화해준다.
	srv_addr.sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
	srv_addr.sin_port = htons(SERVER_PORT); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
	inet_pton(AF_INET, dst_ip, &srv_addr.sin_addr);  // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.

  /* 소켓 생성 */
	int sock; // 소켓 디스크립터(socket descriptor)를 생성한다.
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n"); // sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
	}

	int n = 0;

  struct KVS SendMsg={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화

  struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr); // 수신한 패킷의 소켓 구조체 크기를 저장함. IPv4를 사용하므로 sockaddr_in 크기인 16바이트가 저장됨.
  int cnt = 0; // 패킷 5개를 전송한다.
  size_t pkt_size = 0;
  int rr = 0; // 읽기/쓰기 요청 랜덤 생성하기 위한 변수
	while(cnt < 5){
    printf("Request ID: %d\n",cnt++);
    generate_key(SendMsg.key);  // 임의의 key 생성

    rr = rand() % 2;  // 요청 랜덤생성
    if (rr == READ) { // 0이면 Read 요청
      SendMsg.type = READ_REQ; // 읽기 요청 타입
      strcpy(SendMsg.value, "");  // 읽기 요청은 value를 보내지 않으므로 NULL
      pkt_size = sizeof(struct KVS) - VALUE_SIZE; // 읽기 요청은 value를 보내지 않으므로 packet size에서 제거
      }
    else {  // 쓰기 요청
      SendMsg.type = WRITE_REQ; // 1이면 Write 요청
      strcpy(SendMsg.value,"AAAABBBBCCCCDDD");  // 쓰기 요청이므로 값을 복사해준다.
      pkt_size = sizeof(struct KVS); // 쓰기 요청은 Value를 포함해서 보내므로 패킷 크기는 KVS 구조체의 크기와 같다.
     }

    // 관련 정보 출력하기
    printf("Sent bytes: %ld\n", pkt_size);
    printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg), SendMsg.key, SendMsg.value);

    //송수신
    sendto(sock, &SendMsg, pkt_size, 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr)); // 생성한 메시지를 서버로 송신한다.
    strcpy(SendMsg.value, ""); 
    n = recvfrom(sock, &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len);  // sock 소켓으로부터 수신받은 데이터를 SendMsg 구조체 변수에 SendMsg 크기만큼 저장. 송신자의 정보를 src_addr에 저장

		if (n > 0) { // 만약 송신한 데이터가 0바이트를 초과한다면 (즉, 1바이트라도 수신했다면)
      printf("Received bytes: %d\n",n);
      printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg),SendMsg.key, SendMsg.value); // 수신한 내용을 출력한다.
		}

    
	}

	close(sock); // 소켓을 닫아준다.
	return 0;
}
