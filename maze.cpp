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
#include <random>

   enum class SearchMode{
        None,
        DFS,
        Bfs,
        DJI
   };

   enum class Phase {
        Idle,
        Exploring,
        ShowingPath
   };

   class Maze {
        private:
        int H;
        int W;
       
        vector<vector<int>> maze;
        vector<vector<int>> weights;
         
        public:
       Maze(int h, int w): H(h), W(w), maze(h, vector<int>(w,0)),
            weights(h, vector<int>(w,1)) {}


        int getHeight() const {return H; }


        int getWidth() const {return W; }
        
        int getWeight(int y, int x) const{
            if(!inBounds(y,x)){
                return 0;
            }
            return weights[y][x];
        }

        bool inBounds(int y, int x) const {
            return (y >= 0 && y < H && x >= 0 && x < W);
        }


         int getCell(int y, int x) const {  
        return maze[y][x];
        }

        void randomizeWeights(int minW = 1, int maxW = 9){
            static random_device rd;
            static mt19937 rng(rd());
            uniform_int_distribution<int> dist(minW, maxW);

            for(int i = 0; i < H; i++){
                for(int j = 0; j < W; j++){
                    if(maze[i][j] == 1){
                        weights[i][j] = 0;
                    } else{
                        weights[i][j] = dist(rng);
                    }
                     
                }
            }
        }

        void resetWeights(int value){
            for(int y = 0; y < H; y++){
                for(int x = 0; x < W; x++){
                        weights[y][x] = value;
                }
            }
        }
        void reset(){
            for(int y = 0; y < H; y++){
                for(int x = 0; x < W; x++){
                        maze[y][x] = 0;
                }
            }
            resetWeights(1);
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
        vector<pair<int,int>> explore;
        vector<pair<int, int>> path;

        size_t visible_count = 0;
        sf::Clock stclock;

        SearchMode mode = SearchMode::None;
        Phase phase = Phase::Idle;
    };

bool dijkstra(Maze &ma, vector<pair<int,int>> &path, vector<pair<int,int>> &explore ){
    path.clear();
    explore.clear();
    
    priority_queue<pair<int, pair<int,int>>, vector<pair<int, 
    pair<int,int>>>, greater<pair<int, pair<int,int>>>> pq;
    int startX = 0;
    int startY = 0;

    const int INF = 1e9;

    int H = ma.getHeight();
    int W = ma.getWidth();   
    
    vector<vector<int>> dist(H, vector<int>(W, INF)); 
    vector<vector<pair<int,int>>> parent(H, vector<pair<int,int>>(W, {-1,-1})); 
    
    dist[startY][startX] = 0;
    pq.push({0, {startY, startX}});

    int delta_X[4] = {0,0,-1,1};
    int delta_Y[4] = {1,-1,0,0}; 

    while(!pq.empty()){
        pair<int, pair<int,int>> top = pq.top();
        pq.pop();
        
        int d = top.first;
        int y = top.second.first;
        int x = top.second.second;

        if(d != dist[y][x]){
            continue;
        } else if(y == H-1 && x == W-1){
            break;
        }

        for(int i = 0; i < 4; i++){
            int new_x = x + delta_X[i];
            int new_y = y  + delta_Y[i];
            
            if(!ma.inBounds(new_y,new_x)){
                continue;
            } 
            if(ma.getCell(new_y,new_x) == 1){
                continue;
            }
            
            int weight = ma.getWeight(new_y,new_x);
            int nd = d + weight;
            
            if(nd < dist[new_y][new_x]){
                dist[new_y][new_x] = nd;
                parent[new_y][new_x] = {y, x};
                pq.push({nd, {new_y,new_x}});
                explore.push_back({new_y,new_x});
            }
        }
        
    }
    if(dist[H-1][W-1] == INF){
        return false;
    }
    pair<int,int> cur = {H-1, W-1};
    while(cur.first != startY || cur.second != startX){
        path.push_back(cur);
        cur = parent[cur.first][cur.second];
    }
    path.push_back({startY,startX});
    reverse(path.begin(), path.end());
    return true;
}


bool bfs_shortests(Maze &ma, vector<pair<int, int>> &path, vector<pair<int,int>>& explore){

    explore.clear();
    path.clear();
    
    int x_delta[4] = {0, 0, -1, 1};
    int y_delta[4] = {1, -1, 0, 0};
    
    int end_X = ma.getWidth() - 1;
    int end_Y = ma.getHeight() - 1;
    
    int start_x = 0;
    int start_y = 0;

    if (ma.getCell(start_y,start_x) == 1){
        return false;
    }
    
    if (ma.getCell(end_Y,end_X) == 1){
        return false;
    }

    queue<pair<int, int>> q;

    vector<vector<pair<int,int>>> parent(ma.getHeight(),
     vector<pair<int,int>>(ma.getWidth(), {-1,-1})
    );
    
    ma.setCell(start_y, start_x, 2);
    q.push({start_y,start_x});
    
    explore.push_back({start_y,start_x});
    
    bool found = false;

    while (!q.empty()){
       
        auto [y,x] = q.front();
        q.pop();


        if (y == end_Y && x == end_X)
        {
            found = true;
            break;
        }
        for (int i = 0; i < 4; i++){

            int nx = x + x_delta[i];

            int ny = y + y_delta[i];

            if(ma.isValid(ny,nx)){
                ma.setCell(ny, nx, 2);
                explore.push_back({ny,nx});
                parent[ny][nx] = {y, x};
                q.push({ny,nx});
            }
        }
    }
    if(found == false){
        return false;
    }
    pair<int,int> cur = {end_Y, end_X};
    while(!(cur.first == start_y && cur.second == start_x)){
        path.push_back(cur);
        cur = parent[cur.first][cur.second];
    }
    path.push_back({start_y, start_x});
    reverse(path.begin(), path.end());

    return true;
}

bool dfs(Maze &ma, int Y, int X, vector<pair<int, int>>& path)
{
    int X_movement[4] = {0, 0, -1, 1};
    int Y_movement[4] = {1, -1, 0, 0};

    int goal_x = ma.getWidth() - 1;
    int goal_y = ma.getHeight() - 1;

    if (X == goal_x && Y == goal_y){
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
        for (int i = 0; i < 4; i++){


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
void DrawMaze(Maze &ma, sf::RenderWindow &window, const Tools &tools, const sf::Font& font)
{
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
            const vector<pair<int,int>>& active = 
            (tools.phase == Phase::Exploring) ? tools.explore : tools.path;

            sf::Color highlightColor = (tools.phase == Phase::Exploring) ? sf::Color::Yellow : sf::Color::Magenta;
            bool showHighlight = false;
            for (size_t k = 0; k < tools.visible_count && k < active.size(); k++)
            {
                if (active[k].first == i && active[k].second == j)
                {
                    showHighlight = true;
                    break;
                }
            }

            // 2) Choose color (start/end override everything)
            if (i == startRow && j == startCol)
            {
                rec_ones.setFillColor(sf::Color::Green);
            }
            else if (i == end_y && j == end_x)
            {
                rec_ones.setFillColor(sf::Color::Red);
            } else if(showHighlight){
                rec_ones.setFillColor(highlightColor);
            }
            else
            {
                switch (ma.getCell(i, j))
                {
                case 0:
                    rec_ones.setFillColor(sf::Color::White);
                    break;
                case 1:
                    rec_ones.setFillColor(sf::Color(200, 200, 200));
                    break;
                case 2:
                    // visited but not part of final path
                    rec_ones.setFillColor(sf::Color::White);
                    break;
                default:
                    rec_ones.setFillColor(sf::Color::Cyan);
                    break;
                }
            }

            // 3) Draw cell
            rec_ones.setPosition(j * Tile, i * Tile);
            window.draw(rec_ones);
            
            if (ma.getCell(i, j) != 1) {
                sf::Text wt;
                wt.setFont(font);
                wt.setCharacterSize(16);
                wt.setFillColor(sf::Color::Black);

                wt.setString(to_string(ma.getWeight(i, j)));
                wt.setPosition(j * Tile + 6.f, i * Tile + 6.f);

                window.draw(wt);
            }

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
        "I-Luv-Orange-Cat's"
    );
    sf::Font font;
    if(!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")){
        cout << "failed to load font\n";
        return 1;
    }
    window.setFramerateLimit(60);
    
    tools.visible_count = 0;
    tools.stclock.restart();

    float panelX = maze.getWidth() * Tile;
    sf::RectangleShape newMazeButtton({120.f, 40.f});

    sf::RectangleShape clearPathButton({120.f, 40.f});

    sf::RectangleShape dfsButton({12.f, 40.f});

    sf::RectangleShape bfsButton({12.f, 40.f});

    sf::RectangleShape djKButton({12.f, 80.f});

    sf::RectangleShape RWButton({12.f, 80.f});

    newMazeButtton.setSize({130.f, 40.f});
    newMazeButtton.setPosition(panelX + 15.f, 20.f);
    newMazeButtton.setFillColor(sf::Color::Green);


    clearPathButton.setSize({130.f, 40.f});
    clearPathButton.setPosition({panelX + 15.f, 150.f});
    clearPathButton.setFillColor({sf::Color::Cyan});


    dfsButton.setSize({60.f, 30.f});
    dfsButton.setPosition(panelX + 15.f, 70.f);
    dfsButton.setFillColor(sf::Color::Red);


    bfsButton.setSize({60.f, 30.f});
    bfsButton.setPosition(panelX + 80.f, 70.f);
    bfsButton.setFillColor(sf::Color::Yellow);
   
    djKButton.setSize({60.f, 30.f});
    djKButton.setPosition(panelX + 15.f, 110.f);
    djKButton.setFillColor(sf::Color::Magenta);

    RWButton.setSize({60.f, 30.f});
    RWButton.setPosition(panelX + 80.f, 110.f);
    RWButton.setFillColor(sf::Color::Green);

    sf::Text aa;
    sf::Text st;
    sf::Text BFS_Name;
    sf::Text DFS_Name;
    sf::Text DJK_Name;
    sf::Text RWB_Name;

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

    DJK_Name.setFont(font);
    DJK_Name.setString("DJK");
    DJK_Name.setCharacterSize(24);
    DJK_Name.setFillColor(sf::Color::Black);

    RWB_Name.setFont(font);
    RWB_Name.setString("RDW");
    RWB_Name.setCharacterSize(24);
    RWB_Name.setFillColor(sf::Color::Black);

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

    sf::Vector2f dj = djKButton.getPosition();
    sf::Vector2f dji = djKButton.getSize();
    sf::FloatRect text_Bounds = DJK_Name.getLocalBounds();

    sf::Vector2f rw = RWButton.getPosition();
    sf::Vector2f rwd = RWButton.getSize();
    sf::FloatRect txtb = RWB_Name.getLocalBounds();

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

    DJK_Name.setPosition(
        dj.x + (dji.x - text_Bounds.width) / 2.f,
        dj.y + (dji.y - text_Bounds.height) / 2.f - 5.f
    );
    RWB_Name.setPosition(
        rw.x + (rwd.x - txtb.width) / 2.f,
        rw.y + (rwd.y - txtb.height) / 2.f - 5.f
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
                tools.explore.clear();
                
                tools.mode = SearchMode::DFS;
                tools.phase = Phase::Exploring;

                tools.visible_count = 0;
                tools.stclock.restart();
            }
       
            else if(bfsButton.getGlobalBounds().contains(mousePos)){
                maze.clearVisited();
                tools.path.clear();
                tools.explore.clear();
                
                tools.mode = SearchMode::Bfs;
                tools.phase = Phase::Exploring;

                tools.visible_count = 0;
                tools.stclock.restart();
            } else if(djKButton.getGlobalBounds().contains(mousePos)){
                maze.clearVisited();
                tools.path.clear();
                tools.explore.clear();

                tools.mode = SearchMode::DJI;
                tools.phase = Phase::Exploring;

                tools.visible_count = 0;
                tools.stclock.restart();
            } else if(RWButton.getGlobalBounds().contains(mousePos)){

                maze.randomizeWeights(1,9);

                maze.clearVisited();
                tools.path.clear();
                tools.explore.clear();
                tools.visible_count = 0;
                tools.stclock.restart();

                tools.mode = SearchMode::DJI;
                tools.phase = Phase::Exploring;
            }
          }
        }

                if(tools.phase == Phase::Exploring){
            if(tools.mode == SearchMode::DFS){
                dfs(maze,0,0,tools.path);
                tools.phase = Phase::ShowingPath;
                tools.visible_count = 0;
            } else if(tools.mode == SearchMode::Bfs){
                bfs_shortests(maze, tools.path, tools.explore);
                tools.mode = SearchMode::None;
                tools.visible_count = 0;
                tools.mode = SearchMode::None;
            } else if(tools.mode == SearchMode::DJI){
                dijkstra(maze, tools.path, tools.explore);
                tools.mode = SearchMode::None;
                tools.visible_count = 0;
                tools.stclock.restart();
            }
        }
       
        // Animated path reveal: show one more step every tick_ms
        if (tools.stclock.getElapsedTime().asMilliseconds() > tick_ms){

                if(tools.phase == Phase::Exploring){

                    if(tools.visible_count < tools.explore.size()){
                    tools.visible_count++;
                    
                } else if(tools.visible_count >= tools.explore.size()){
                    tools.phase = Phase::ShowingPath;    
                    tools.visible_count = 0;
                }
            }
               else if(tools.phase == Phase::ShowingPath){
                    if(tools.visible_count < tools.path.size()){
                        tools.visible_count++;
                    } else {
                        tools.phase = Phase::Idle;
                    }
               } 
            tools.stclock.restart();
        }

        window.clear(sf::Color::Blue);
        DrawMaze(maze, window, tools, font);
        window.draw(newMazeButtton);
        window.draw(clearPathButton);
        window.draw(dfsButton);
        window.draw(bfsButton);
        window.draw(djKButton);
        window.draw(RWButton);
        window.draw(st);
        window.draw(aa);
        window.draw(BFS_Name);
        window.draw(DFS_Name);
        window.draw(DJK_Name);
        window.draw(RWB_Name);
        window.display();
        
      }

    return 0;
}