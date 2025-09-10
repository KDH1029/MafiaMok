#include <vector>

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
private:
    std::vector<std::vector<Node *>> field;

public:
    int x, y, team;
    bool turn;
    Field(void);
    bool place(int x, int y, int value);
    bool check(void);
};
