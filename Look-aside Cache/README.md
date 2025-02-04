### [03. Look-aside Cache]
- 직접 Look-aside Cache 구현하기
- Look-aside architecture는 별도의 cache 서버(노드)를 두어 top-k item(data)를 cache에 복사하는 것이다.
- 비교되는 architecture로는 On-path Look-through architecture가 있다. 이 아키텍처는 로드밸런서(또는 코디네이터)에 cache 기능을 추가하여 cache-miss penalty 문제를 해결한다. 

`client` <br/><br/>
<img src="https://github.com/user-attachments/assets/c2896efb-6cdd-4bb8-828e-ae85840fd1fd" width="600" height="400"/>


`cache` <br/><br/>
<img src="https://github.com/user-attachments/assets/42cdd540-1cde-441b-85c7-6460b956c94f" width="600" height="400"/>

`server` <br/><br/>
<img src="https://github.com/user-attachments/assets/2982f173-e23c-460c-90cd-b2ffe0878345" width="600" height="400"/>
