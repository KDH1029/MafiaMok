#include <vector>
#include <QGraphicsEllipseItem>

class Node
{
public:
    int x, y;
    int value;
    Node *neighbor[8];
    Node(int x, int y);
};

class Field
{
public:
    std::vector<std::vector<Node *>> board;
    int x, y, team;
    bool turn;
    Field(void);
    bool place(int x, int y, int value);
    bool seduce(int x, int y);
    bool remove(int x, int y);
    bool check(void);
};
