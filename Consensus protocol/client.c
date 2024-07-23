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
	int SERVER_PORT[NUM_SRV] = {5001, 5002, 5003};
	struct sockaddr_in srv_addr[3]; // 패킷을 수신할 서버의 정보를 담을 소켓 구조체를 생성한다.
  for (int i = 0; i < 3; i++) {
    memset(&srv_addr[i], 0, sizeof(srv_addr[i])); // 구조체를 모두 '0'으로 초기화해준다.
		srv_addr[i].sin_family = AF_INET; // IPv4를 사용할 것이므로 AF_INET으로 family를 지정한다.
		srv_addr[i].sin_port = htons(SERVER_PORT[i]); // 서버의 포트번호를 넣어준다. 이 때 htons()를 통해 byte order를 network order로 변환한다.
		inet_pton(AF_INET, dst_ip, &srv_addr[i].sin_addr); // 문자열인 IP주소를 바이너리로 변환한 후 소켓 구조체에 저장해준다.
  }

  

  /* 소켓 생성 */
	int sock[3];  // 소켓 디스크립터(socket descriptor)를 생성한다.
  for (int i = 0; i < 3; i++) {
    if ((sock[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // socket()으로 IPv4(AF_INET), UDP(SOC_DGRAM)를 사용하는 소켓을 생성 시도한다.
		printf("Could not create socket\n");// sock으로 return되는 값이 -1이라면 소켓 생성에 실패한 것이다.
		exit(1);
		}
  }

	int n = 0;

  struct KVS SendMsg={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화
  struct KVS SendMsg2={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화
  struct KVS SendMsg3={0,}; // 송신용으로 쓸 메시지 구조체 생성 및 초기화


  struct sockaddr_in src_addr; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len = sizeof(src_addr); // 수신한 패킷의 소켓 구조체 크기를 저장함. IPv4를 사용하므로 sockaddr_in 크기인 16바이트가 저장됨.
  struct sockaddr_in src_addr2; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len2 = sizeof(src_addr2);
  struct sockaddr_in src_addr3; // 패킷을 수신하였을 때, 해당 패킷을 보낸 송신자(Source)의 정보를 저장하기 위한 소켓 구조체
  socklen_t src_addr_len3 = sizeof(src_addr3);


  int cnt = 0; // 패킷 5개를 전송한다.
  size_t pkt_size = 0;
	while(cnt < 5){
    printf("Request ID: %d\n",cnt++);
    generate_key(SendMsg.key);  // 임의의 key 생성
    SendMsg.type = READ_REQ; // 요청 타입을 읽기로 선언한다.
    strcpy(SendMsg.value,"");
    pkt_size = sizeof(struct KVS) - VALUE_SIZE;
    printf("Sent bytes: %ld\n",pkt_size);
    printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg),SendMsg.key,SendMsg.value);

    for(int i = 0; i < 3; i++)
			sendto(sock[i], &SendMsg, pkt_size, 0, (struct sockaddr *)&srv_addr[i], sizeof(srv_addr[i]));

    n = recvfrom(sock[0], &SendMsg, sizeof(SendMsg), 0, (struct sockaddr *)&src_addr, &src_addr_len); // 서버로부터 답장을 수신한다.
    int n2 = recvfrom(sock[1], &SendMsg2, sizeof(SendMsg2), 0, (struct sockaddr *)&src_addr2, &src_addr_len2); // 서버로부터 답장을 수신한다.
    int n3 = recvfrom(sock[2], &SendMsg3, sizeof(SendMsg3), 0, (struct sockaddr *)&src_addr3, &src_addr_len3); // 서버로부터 답장을 수신한다.

    // 각 소켓으로부터 받은 답신이 0바이트 초과일 때, 관련 정보를 출력한다. SendMsg 구조체를 따로 초기화하여 분리함
    if (n > 0) {
      printf("Received bytes: %d\n",n2);
      printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg),SendMsg.key, SendMsg.value);
    }
    if (n2 > 0) {
      printf("Received bytes: %d\n",n2);
      printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg2),SendMsg2.key, SendMsg2.value);
    }
    if (n3 > 0) {
      printf("Received bytes: %d\n",n2);
      printf("Type: %s Key: %s Value: %s\n",get_type(SendMsg3),SendMsg.key, SendMsg3.value);
    }
    // 3개의 송신이 0바이트 초과 수신되었을 때, 각각의 SendMsg의 value를 비교한다.
    if (n > 0 && n3 > 0 && n2 > 0) {
      //스트링 비교
      if (strcmp(SendMsg.value, SendMsg2.value) == 0 || strcmp(SendMsg.value, SendMsg3.value) == 0 || strcmp(SendMsg2.value, SendMsg3.value) == 0) {
        printf("TURE\n\n");
      } else { 
        printf("FALSE\n\n");
      }
    }
	}

  for (int i = 0; i < 3; i++) {
    close(sock[i]);// 소켓을 닫아준다.
  }
	//close(sock); // 소켓을 닫아준다.
	return 0;
}
