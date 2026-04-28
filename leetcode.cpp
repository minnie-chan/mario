using namespace std;
#include <vector>
#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <utility>
#include <ctime>
#include <queue>
int maze[5][5] = {{0, 0, 0, 0, 0},
                  {0, 1, 1, 1, 0},
                  {0, 1, 1, 1, 0},
                  {0, 1, 1, 1, 0},
                  {0, 0, 0, 0, 0}};
   class Maze {
        private: 
        int H;
        int W;
         int maze[5][5];
                 int initial[5][5] = {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 0, 0, 0}
        };
         
        public:
       Maze(){
        H = 5;
        W = 5;
        }
        int getHeight() const {return H; }
        int getwidth() const {return W; }
        bool inBounds(int y, int x) const {
            return (y >= 0 && y < H && x >= 0 && x < W);
        }
         int getCell(int y, int x) const {  
        return maze[y][x];
        }


        void reset(){
            for(int y = 0; y < H; y++){
                for(int x = 0; x < W; x++){
                    maze[y][x] = initial[y][x];
                }
            }
        }
        void setCell(int y, int x, int value) {
        maze[y][x] = value;
        }
        bool iValid(int y, int x) const{
            int cell = getCell(y,x);
            if(!inBounds(y,x)){
                return false;
            }
            if(cell == 0){
                return true;
            }
            return false;
        }

    };
int upndown = 5;
int leftnright = 5;
const int Tile = 60;
bool is_Valid(int y, int x)
{
    if (x < 0 || x >= leftnright || y < 0 || y >= upndown)
    {
        return false;
    }
    constexpr int FLOOR = 0, WALL = 1, VISITED = 2;
    if (maze[y][x] == FLOOR)
    {
        return true;
    }
    else if (maze[y][x] == WALL)
    {
        return false;
    }
    else if (maze[y][x] == VISITED)
    {
        return false;
    }
    return false;
}
vector<pair<int, int>> vec;
queue<pair<int, int>> qu;
bool bfs(queue<pair<int, int>> &qu, vector<pair<int, int>> &vec)
{

    int x_delta[4] = {0, 0, -1, 1};

    int y_delta[4] = {1, -1, 0, 0};

    int end_X = leftnright - 1;
    int end_Y = upndown - 1;

    int start_x = 0;
    int start_y = 0;

   

    if (maze[start_y][start_x] == 1)
    {
        return false;
    }
    else if (maze[start_y][start_x] == 0)
    {
        maze[start_y][start_x] = 2;
        qu.push({start_y, start_x});
        vec.push_back({start_y, start_x});
    }

    while (!qu.empty())
    {
        
        pair<int, int> cur = qu.front();

        int y = cur.first;
        int x = cur.second;
        qu.pop();
        if (y == end_Y && x == end_X)
        {
            vec.push_back({y, x});
            return true;
        }
        for (int i = 0; i < 4; i++){
            int nx = x + x_delta[i];
            int ny = y + y_delta[i];
            if(is_Valid(ny,nx)){
                maze[ny][nx] = 2;
                qu.push({ny,nx});
                vec.push_back({ny,nx});
            }
        }
    }
    return false;
}

vector<pair<int, int>> path;
bool dfs(Maze &ma, int Y, int X, vector<pair<int, int>> &path)
{
    int X_movement[4] = {0, 0, -1, 1};
    int Y_movement[4] = {1, -1, 0, 0};

    int goal_x = leftnright - 1;
    int goal_y = upndown - 1;
    if (X == goal_x && Y == goal_y)
    {
        path.push_back({Y, X});
        return true;
    }
    if (maze[Y][X] == 0)
    {
        maze[Y][X] = 2;
        path.push_back({Y, X});
    }
    else if (maze[Y][X] != 0)
    {
        return false;
    }

    for (int i = 0; i < 4; i++)
    {

        int new_X = X + X_movement[i];

        int new_Y = Y + Y_movement[i];

        if (is_Valid(new_Y, new_X))
        {

            if (dfs(ma, new_Y, new_X, path) == true)
            {
                return true;
            }
        }
    }
    path.pop_back();
    return false;
}
size_t visible_count = 0;
sf::Clock stclock;
const sf::Int32 tick_ms = 1000;

void DrawMaze(sf::RenderWindow &window)
{
    int startRow = 0;
    int startCol = 0;
    int end_x = leftnright - 1;
    int end_y = upndown - 1;
    sf::RectangleShape rec_ones;

    rec_ones.setSize(sf::Vector2f(Tile - 2, Tile - 2));

    for (int i = 0; i < upndown; i++)
    {
        for (int j = 0; j < leftnright; j++)
        {

            if (i == startRow && j == startCol)
            {
                rec_ones.setFillColor(sf::Color::Green);
            }
            else if (i == end_y && j == end_x)
            {
                rec_ones.setFillColor(sf::Color::Red);
            }
            else
            {
                switch (maze[i][j])
                {
                case 0:
                    rec_ones.setFillColor(sf::Color::White);
                    break;
                case 1:
                    rec_ones.setFillColor(sf::Color(200, 200, 200));
                    break;
                case 2:
                {
                    bool showYellow = false;
                    for (size_t k = 0; k < visible_count; k++)
                    {
                        if (path[k].first == i && path[k].second == j)
                        {
                            showYellow = true;
                            break;
                        }
                    }
                    if (showYellow)
                        rec_ones.setFillColor(sf::Color::Yellow);
                    else
                        rec_ones.setFillColor(sf::Color::White);
                    break;
                }
                default:
                    rec_ones.setFillColor(sf::Color::Cyan);
                }
            }
            rec_ones.setPosition(j * Tile, i * Tile);
            window.draw(rec_ones);
        }
    }
}

 

int main()
{
    Maze ma;
    ma.reset();
    sf::RenderWindow window(sf::VideoMode(leftnright * Tile, upndown * Tile), "meow");
    window.setFramerateLimit(60);

    dfs(ma, 0, 0, path);
    visible_count = 0;
    stclock.restart();
    
    //sf::RectangleShape shape({120.f, 50.f});
    //shape.setFillColor(sf::Color::Black);
    //shape.setScale(2.5f,1.2f);
    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        if (stclock.getElapsedTime().asMilliseconds() > tick_ms && visible_count < path.size())
        {
            visible_count++;
            stclock.restart();
        }
        window.clear();
        DrawMaze(window);
      //  window.draw(shape);
        window.display();
    }
 
    return 0;
}
