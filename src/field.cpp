#define BOARD_SIZE = 19;

#include "field.h"

Node::Node(int x, int y)
{
    this->x = x;
    this->y = y;
    this->value = 0;
}

Field::Field(void)
{
    this->field.resize(BOARD_SIZE, std::vector<Node>(BOARD_SIZE));

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            this->field[y][x] = new Node(x, y);

            int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
            int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

            for (int dir = 0; dir < 8; dir++)
            {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE)
                {
                    this->field[y][x]->neighbor[dir] = this->field[ny][nx];
                }
                else
                {
                    this->field[y][x]->neighbor[dir] = nullptr; // 경계 밖은 null
                }
            }
        }
    }
}

void Field::placeStone(int x, int y, int value)
{
    field[y][x].value = value;
}