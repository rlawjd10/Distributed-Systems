### [04. Power of two choices Load Balancing]
- power of two choices는 random LB와 leat connection을 혼합한 것과 유사하다.
- 임의의 두 개의 서버를 선택을 한 후, 선택된 두 개의 서버 중에서 connection 수 또는 요청 수가 적은 쪽은 선택

`client` <br/><br/>
<img src="https://github.com/user-attachments/assets/95f7e01c-e326-4e72-9a3f-4acf7e824d5c" width="600" height="400"/>


`server1` <br/><br/>
<img src="https://github.com/user-attachments/assets/fd373f72-d887-4a5c-8782-cd17019c0edd" width="600" height="400"/>


`server2` <br/><br/>
<img src="https://github.com/user-attachments/assets/9f345842-9262-4eae-9ab4-575692008183" width="600" height="400"/>


`server3` <br/><br/>
<img src="https://github.com/user-attachments/assets/466abf00-023d-47c4-a2f2-f47f9e144cac" width="600" height="400"/>

`server4` <br/><br/>
<img src="https://github.com/user-attachments/assets/3328a07a-2809-47f1-bcef-7393303cc2bc" width="600" height="400"/>
