# MafiaMok

수정 사항:

field class로 변경

돌 추가 -> 턴 넘김 구현 완료

udp 수신 -> 턴 받음 구현 완료, 예외처리 필요

돌 제거 함수
bool Field::remove(int x, int y)
구현, 미적용

this->field->remove(x,y)

돌을 지울 수 있는 상황인지 확인 필요
(턴, 돌 색)

돌 놓으면서 승리조건 확인