### [Look-aside Cache]
- 직접 Look-aside Cache 구현하기
- Look-aside architecture는 별도의 cache 서버(노드)를 두어 top-k item(data)를 cache에 복사하는 것이다.
- 비교되는 architecture로는 On-path Look-through architecture가 있다. 이 아키텍처는 로드밸런서(또는 코디네이터)에 cache 기능을 추가하여 cache-miss penalty 문제를 해결한다. 
![image](https://github.com/rlawjd10/Distributed-Systems/assets/108384971/6dc7eade-a4db-436c-81f0-1d744f418533)
