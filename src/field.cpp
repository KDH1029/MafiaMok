#define BOARD_SIZE 19
#define LEN 5

#include "field.h"
#include <iostream>

Node::Node(int x, int y)
{
    this->x = x;
    this->y = y;
    this->value = 0;
}

Field::Field(void)
{
    this->board.resize(BOARD_SIZE);

    for (int y = 0; y < BOARD_SIZE; ++y)
    {
        this->board[y].resize(BOARD_SIZE);
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            this->board[y][x] = new Node(x, y);
        }
    }

    int dx[8] = {0, 1, 1, 1, -1, -1, -1, 0};
    int dy[8] = {-1, -1, 0, 1, -1, 0, 1, 1};

    for (int y = 0; y < BOARD_SIZE; ++y)
    {
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            for (int dir = 0; dir < 8; ++dir)
            {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE)
                {
                    this->board[y][x]->neighbor[dir] = this->board[ny][nx];
                }
                else
                {
                    this->board[y][x]->neighbor[dir] = nullptr;
                }
            }
        }
    }
}

bool Field::place(int x, int y, int value)
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
    {
        return false;
    }
    this->x = x;
    this->y = y;
    if (this->board[y][x]->value == 0)
    {
        this->board[y][x]->value = value;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                std::cout << this->board[i][j]->value;
            }
            std::cout << std::endl;
        }

        std::cout << std::endl
                  << std::endl
                  << std::endl;
        return true;
    }
    return false;
}

bool Field::check(void)
{
    for (int i = 0; i < 4; i++)
    {
        Node *node = this->board[y][x];
        int state = node->value;
        int count = 1;
        while (node->neighbor[i] && node->neighbor[i]->value == state)
        {
            node = node->neighbor[i];
        }
        while (node->neighbor[7 - i] && node->neighbor[7 - i]->value == state)
        {
            node = node->neighbor[7 - i];
            count++;
        }

        if (count >= LEN)
        {
            std::cout << "win" << std::endl
                      << std::endl;
            return true;
        }
    }
    return false;
}