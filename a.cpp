using namespace std;
#include <vector>
#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <utility>
#include <ctime>
#include <limits>
#include <queue>


   class Maze {
        private:
        int H;
        int W;
       
        vector<vector<int>> maze;
         
        public:
       Maze(int h, int w): H(h), W(w), maze(h, vector<int>(w,0)) {}


        int getHeight() const {return H; }


        int getWidth() const {return W; }


        bool inBounds(int y, int x) const {
            return (y >= 0 && y < H && x >= 0 && x < W);
        }


         int getCell(int y, int x) const {  
        return maze[y][x];
        }


       


        void reset(){
            for(int y = 0; y < H; y++){
                for(int x = 0; x < W; x++){
                        maze[y][x] = 0;
                }
            }
        }
        void clearVisited(){
            for(int y = 0; y < H; y++){
                for(int x = 0; x < W; x++){
                    if(maze[y][x] == 2){
                        maze[y][x] = 0;
                    }
                }
            }
        }
        void setCell(int y, int x, int value) {
        maze[y][x] = value;
        }
        bool isValid(int y, int x) const{
            if(!inBounds(y,x)){
                return false;
            }
            int cell = getCell(y,x);


            return cell == 0;
        }


    };
const int Tile = 60;


    struct Tools{
        vector<pair<int, int>> path;
        size_t visible_count = 0;
        sf::Clock stclock;
        queue<pair<int, int>> qu;
    };
   
bool bfs(Maze &ma, queue<pair<int, int>>& qu, vector<pair<int, int>> &path)
{


    int x_delta[4] = {0, 0, -1, 1};


    int y_delta[4] = {1, -1, 0, 0};


    int end_X = ma.getWidth() - 1;
    int end_Y = ma.getHeight() - 1;


    int start_x = 0;
    int start_y = 0;


   


    if (ma.getCell(start_y,start_x) == 1)
    {
        return false;
    }
    else if (ma.getCell(start_y,start_x) == 0)
    {
        ma.setCell(start_y,start_x, 2);
        qu.push({start_y, start_x});
        path.push_back({start_y, start_x});
    }


    while (!qu.empty()){
       
        pair<int, int> cur = qu.front();


        int y = cur.first;
        int x = cur.second;
        qu.pop();
        if (y == end_Y && x == end_X)
        {
            path.push_back({y, x});
            return true;
        }
        for (int i = 0; i < 4; i++){


            int nx = x + x_delta[i];


            int ny = y + y_delta[i];


            if(ma.isValid(ny,nx)){


                ma.setCell(ny, nx, 2);
                qu.push({ny,nx});
                path.push_back({ny,nx});
            }
        }
    }
    return false;
}




bool dfs(Maze &ma, int Y, int X, vector<pair<int, int>>& path)
{
    int X_movement[4] = {0, 0, -1, 1};
    int Y_movement[4] = {1, -1, 0, 0};


    int goal_x = ma.getWidth() - 1;
    int goal_y = ma.getHeight() - 1;
    if (X == goal_x && Y == goal_y)
    {
     path.push_back({Y, X});
        return true;
    }
    int cell = ma.getCell(Y, X);
    if(cell == 0){
        ma.setCell(Y, X, 2);
     path.push_back({Y, X});
    } else {
        return false;
    }
    for (int i = 0; i < 4; i++)
    {


        int new_X = X + X_movement[i];


        int new_Y = Y + Y_movement[i];


        if (ma.isValid(new_Y, new_X))
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


const sf::Int32 tick_ms = 50;


//this is where we make the maze show in our window
void DrawMaze(Maze &ma , sf::RenderWindow &window, const Tools &tools){


    int startRow = 0;
    int startCol = 0;
    int end_x = ma.getWidth() - 1;
    int end_y = ma.getHeight() - 1;
    sf::RectangleShape rec_ones;


    rec_ones.setSize(sf::Vector2f(Tile - 2, Tile - 2));


    for (int i = 0; i < ma.getHeight(); i++)
    {
        for (int j = 0; j < ma.getWidth(); j++)
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
                switch (ma.getCell(i,j))
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
                    for (size_t k = 0; k < tools.visible_count; k++)
                    {
                        if(tools.path[k].first == i && tools.path[k].second == j)
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


    int clampSize(int v){
        if(v <= 1) {
            return 10;
        }
        if(v > 15){
            return 15;
        }
        return v;
    }


    void readMazeSize(int& h, int& w) {
    cout << "Enter maze size (H x W), max 15x15 (example: 5 x 7): ";


    int H = 10, W = 10;
    char x;


    if (cin >> H >> x >> W && (x == 'x' || x == 'X')) {
        h = clampSize(H);
        w = clampSize(W);
    } else {
       
        h = 15;
        w = 15;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    }


int main()
{
    Tools tools;
    int H, W;
    readMazeSize(H,W);
    const int PANEL_WIDTH = 150;
    Maze maze(H,W);
    maze.reset();
    sf::RenderWindow window(
        sf::VideoMode(maze.getWidth() * Tile + PANEL_WIDTH, maze.getHeight() * Tile),
        "meow"
    );
    sf::Font font;
    if(!font.loadFromFile("arial.ttf")){
        cout << "failed to load font\n";
    }
    window.setFramerateLimit(60);


    tools.visible_count = 0;
    tools.stclock.restart();


    float panelX = maze.getWidth() * Tile;
    sf::RectangleShape newMazeButtton({120.f, 40.f});


    sf::RectangleShape clearPathButton({120.f, 40.f});


    sf:: RectangleShape dfsButton({12.f, 40.f});


    sf:: RectangleShape bfsButton({120.f, 40.f});


    newMazeButtton.setSize({130.f, 40.f});
    newMazeButtton.setPosition(panelX + 15.f, 20.f);
    newMazeButtton.setFillColor(sf::Color::Green);


    clearPathButton.setSize({130.f, 40.f});
    clearPathButton.setPosition({panelX + 15.f, 100.f});
    clearPathButton.setFillColor({sf::Color::Cyan});


    dfsButton.setSize({60.f, 30.f});
    dfsButton.setPosition(panelX + 15.f, 70.f);
    dfsButton.setFillColor(sf::Color::Red);


    bfsButton.setSize({60.f, 30.f});
    bfsButton.setPosition(panelX + 80.f, 70.f);
    bfsButton.setFillColor(sf::Color::Yellow);
   
    sf::Text aa;
    sf::Text st;
    sf::Text BFS_Name;
    sf::Text DFS_Name;


    st.setFont(font);
    st.setString("New Maze");
    st.setCharacterSize(24);
    st.setFillColor(sf::Color::Black);


    aa.setFont(font);
    aa.setString("clear path");
    aa.setCharacterSize(24);
    aa.setFillColor(sf::Color::Black);


    BFS_Name.setFont(font);
    BFS_Name.setString("BFS");
    BFS_Name.setCharacterSize(24);
    BFS_Name.setFillColor(sf::Color::Black);


    DFS_Name.setFont(font);
    DFS_Name.setString("DFS");
    DFS_Name.setCharacterSize(24);
    DFS_Name.setFillColor(sf::Color::Black);


    sf::Vector2f btnPos = newMazeButtton.getPosition();
    sf::Vector2f btnSize = newMazeButtton.getSize();
   
    sf::Vector2f btPos = clearPathButton.getPosition();
    sf::Vector2f btsize = clearPathButton.getSize();


    sf::Vector2f bt = bfsButton.getPosition();
    sf::Vector2f bts = bfsButton.getSize();
    sf::FloatRect textBounds = BFS_Name.getLocalBounds();


    sf::Vector2f dt = dfsButton.getPosition();
    sf::Vector2f dts = dfsButton.getSize();
    sf::FloatRect textbounds = DFS_Name.getLocalBounds();


    aa.setPosition(
        btPos.x + btsize.y / 4.0f,
        btPos.y + btsize.y /4.0f
    );


    st.setPosition(
        btnPos.x + btnSize.y / 4.0f,
        btnPos.y + btnSize.y /4.0f
    );
       
    BFS_Name.setPosition(
        bt.x + (bts.x - textBounds.width) / 2.f,
        bt.y + (bts.y - textBounds.height) / 2.f - 5.f
    );
   
    DFS_Name.setPosition(
    dt.x + (dts.x - textbounds.width) / 2.f,
    dt.y + (dts.y - textbounds.height) / 2.f - 5.f
    );


    sf::Event ev;


    while (window.isOpen())
    {
       
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                window.close();
            }
             if(ev.type == sf::Event::MouseButtonPressed &&
           ev.mouseButton.button == sf::Mouse::Left){
            sf::Vector2i pixelPos((ev.mouseButton.x),(ev.mouseButton.y));
            sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
            if(newMazeButtton.getGlobalBounds().contains(mousePos)){
             maze.reset();
             tools.path.clear();
             
               
                tools.visible_count = 0;
                tools.stclock.restart();
            } else if(clearPathButton.getGlobalBounds().contains(mousePos)){
             maze.clearVisited();
             tools.path.clear();
             
               
                tools.visible_count = 0;
                tools.stclock.restart();
            }
       
       
            else if(dfsButton.getGlobalBounds().contains(mousePos)){
                maze.clearVisited();
                tools.path.clear();
                dfs(maze, 0, 0, tools.path);


                tools.visible_count = 0;
                tools.stclock.restart();
            }
       
            else if(bfsButton.getGlobalBounds().contains(mousePos)){
                maze.clearVisited();
                tools.path.clear();
                while (!tools.qu.empty()) tools.qu.pop();
                bfs(maze, tools.qu, tools.path);


                tools.visible_count = 0;
                tools.stclock.restart();
            }
          }
        }
       
        // Animated path reveal: show one more step every tick_ms
        if (tools.stclock.getElapsedTime().asMilliseconds() >
            tick_ms && tools.visible_count < tools.path.size())
        {
            tools.visible_count++;
            tools.stclock.restart();
        }


        window.clear(sf::Color::Blue);
        DrawMaze(maze, window, tools);
        window.draw(newMazeButtton);
        window.draw(clearPathButton);
        window.draw(dfsButton);
        window.draw(bfsButton);
        window.draw(st);
        window.draw(aa);
        window.draw(BFS_Name);
        window.draw(DFS_Name);
        window.display();
       
    }
   
    return 0;
}


