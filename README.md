# MafiaMook

field

field->place(int x, int y, int value)

돌 놓기

if field[y][x] value != 0:

    field[y][x]에 value저장

    return true

else

    return false

endif

field->check()

오목 완성 여부 확인

class Udp
Qstring get_udp();

QUdpSocket *udpSocket;

void send(QString str);
void place(int x, int y);
