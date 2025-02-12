### [05. Primary-backup]
- 간단한 Primary-backup 프로토콜을 구현하여 복제 프로토콜에 대한 이해 높이기.

<details>
<summary>replication? </summary>
<div markdown="1">

- Distributed system에서 replication은 statueful 서버들을 복제하여 각 서버들의 data에 대한 state가 완전히 동일해야 한다.
- Replication의 필요성 : high availability, fault tolerance / high performance (more servers, more computing powers)
- Replication storage에서 statueful을 focusing하기 위해서 data replication을 하기 위한 여러 방법들이 있다.
* LB replication
  - PB replication : read 요청은 primary에서 처리하고 write 요청은 backup replication에 update를 하고 ACK을 받는다. 그리고 client한테 commit을 하는 과정이다.
  - Chain replication : Head에서 write하고 chain으로 연결된 replica에 순차적으로 update하며 tail에서 read를 한다. (Leafer가 2개인 셈이다.)
* Leaderless replication
  - leader-election problem을 해결함으로써 fault tolerance를 향상하지만, read-write confilct, Inter-write conflic가 발생가능하다.

</div>
</details>

`client` <br/><br/>
<img src="https://github.com/user-attachments/assets/42d69ea8-6c89-4251-aae3-39da9e3d1e26" width="600" height="400"/>

`primary` <br/><br/>
<img src="https://github.com/user-attachments/assets/d842bf1b-16db-474c-9d7c-74521c636d94" width="600" height="400"/>

`backup1` <br/><br/>
<img src="https://github.com/user-attachments/assets/237bbfe6-f04e-45e9-a7da-4661e93b29f7" width="600" height="400"/>

`backup2` <br/><br/>
<img src="https://github.com/user-attachments/assets/e97b2b2d-2fb6-419e-82a8-724b508810a8" width="600" height="400"/>
