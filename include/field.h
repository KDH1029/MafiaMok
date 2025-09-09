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
    std::vector<std::vector<Node*>> field;

public:
    Field(void);
    void placeStone(int x, int y, int value);
};
