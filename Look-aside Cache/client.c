#include "util.h"

const char* dst_ip = "127.0.0.1"; // 하나의 host안에서 통신할 것이므로 서버주소는 localhost(i.e., 127.0.0.1)임

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
  int n2 = 0;

  struct KVS SendMsg={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화

  struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr); // 수신한 패킷의 소켓 구조체 크기를 저장함. IPv4를 사용하므로 sockaddr_in 크기인 16바이트가 저장됨.
  int cnt = 0; // 패킷 5개를 전송한다.
  size_t pkt_size = 0;
	while(cnt < 5){
    printf("Request ID: %d\n",cnt++);

    generate_key(SendMsg.key);  // 임의의 key를 생성한다.
    SendMsg.type = READ_REQ; // 타입은 읽기 요청
    strcpy(SendMsg.value, ""); // 읽기 요청은 key에 대한 value를 요청하므로 value는 아무것도 없이 보낸다.
    pkt_size = sizeof(struct KVS) - VALUE_SIZE;  // 해당 client는 읽기 요청/답신만이 존재한다고 가정하므로, 전체 패킷 크기에서 VALUE-SIZE를 제외한다.
    printf("Sent bytes: %ld\n",pkt_size); // 패킷 사이즈 출력
    printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg), SendMsg.key, SendMsg.value); // 읽기 요청에 대한 정보 출력
    srv_addr.sin_port = htons(SERVER_PORT); // cache miss가 발생하면 포트 넘버가 바뀌므로 loop될 때마다 선언
    // sock 소켓을 통해 SendMsg 구조체 변수에 있는 데이터를 SendMsg 크기만큼 srv_addr의 5001번 포트의 서버에 송신
    sendto(sock, &SendMsg, pkt_size, 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr));  //cache 서버에 보낸다. default 포트 넘버는 5001이므로 port number는 수정할 필요가 없다. 
    n = recvfrom(sock, &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len); // sock 소켓으로부터 수신받은 데이터를 SendMsg 구조체 변수에 SendMsg 크기만큼 저장. 송신자의 정보를 src_addr에 저장

    if (n > 0) { // 수신한 데이터가 0바이트 초과라면
      if (SendMsg.type == CACHE_HIT) { // Cache hit인 경우, 관련 정보(수신 데이터 크기, key, value, type)를 출력하고 처리 종료
        printf("Cache Hit!!\n");
        printf("Received bytes: %d\n",n);
      printf("Type: %s Key: %s Value: %s\n\n",get_type(SendMsg),SendMsg.key, SendMsg.value); // 수신한 내용을 출력한다.
      }
      else if (SendMsg.type == CACHE_MISS) { // Cache miss인 경우
        printf("Cache Miss!!\n"); 
        srv_addr.sin_port = htons(SERVER_PORT + 1); // 5002번 포트로 송신하기 위해서 +1을 한다.
        sendto(sock, &SendMsg, pkt_size, 0, (struct sockaddr *)&srv_addr, sizeof(srv_addr));  //스토리지 서버에 보낸다.
        n2 = recvfrom(sock, &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len); // 스토리지 서버로부터 받은 데이터를 SendMsg 구조체 변수에 저장. 송신자의 정보를 src_addr에 저장
        if (n2 > 0) {
          SendMsg.value[VALUE_SIZE - 1] = '\0'; // 필드값 오염을 방지하기 위해 null 문자로 끝내기
          printf("Received bytes: %d\n",n2);
          printf("Type: %s Key: %s Value: %s\n\n",get_type(SendMsg),SendMsg.key, SendMsg.value); // 수신한 내용을 출력한다.
        }
      }
      
    }

	}

	close(sock); // 소켓을 닫아준다.
	return 0;
}
