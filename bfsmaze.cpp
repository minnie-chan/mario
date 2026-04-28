using namespace std;
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
    #include <queue>
    int maze[5][5] = { {0,0,1,1,1},
                       {1,0,0,0,1},
                       {1,0,1,0,0},
                       {1,0,1,1,0},
                       {1,0,0,0,0} };

                     int  upnown = 5;
                 int leftnright = 5;

      int startrow = 0;
      int startcol = 0;
      
      int endRow = 4;
      int endCol = 4;

      
      
     bool endpointsAreValid( ){
        
        if(maze[startrow][startcol] == 0 && maze[endRow][endCol] == 0 ){
          
            return true;
        } 
         else {
            return false;
        }
       
     }

       
       queue<pair<int,int>> mq; 

     bool movement(  queue<pair<int,int>>& mq ){
         if(endpointsAreValid() == false ){
              return false;
            } 
                int nr; 
                int nc;
        int X[] = {0,0,-1,1};
      int Y[] = {1,-1,0,0};
           

           mq.push({startrow,startcol});
          
           int ROWS = 5;

           int COLS = 5;

            if(maze[startrow][startcol] == 0){
                maze[startrow][startcol] = 2;
            }
            while(!mq.empty()){
                auto [r,c] = mq.front();
                mq.pop();
                if(r == endRow && c == endCol){
                    return true;
                }
                

           for(int i = 0; i < 4; i++){
                nr = r + X[i]; 
                nc = c + Y[i];
                if(nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS ) { continue; }
                if(maze[nr][nc] == 1 || maze[nr][nc] == 2){
                    continue;
                } else if( maze[nr][nc] == 0){
                    maze[nr][nc] = 2;
                    mq.push({nr, nc});
                }
           }
        }
        return false;
     }
      
                 


int main(){

    
           if(maze[0][0] == 1){
                 cout << "invalid start \n";
        }
        if(maze[4][4]  == 1){
             cout << "invalid ending \n";
        }
        
       
       cout << "Start: (" << startrow << "," << startcol << ")  "
         << "End: (" << endRow << "," << endCol << ")\n\n";

  
    cout << "Initial maze (0=open, 1=wall):\n";
    for (int r = 0; r < leftnright; ++r) {
        for (int c = 0; c < upnown; ++c) {
            cout << maze[r][c];
            if (c + 1 < upnown) cout << ' ';
        }
        cout << '\n';
    }
    cout << '\n';

    // Run BFS
    bool found = movement(mq);

    // Result
    cout << (found ? "PATH FOUND\n\n" : "NO PATH\n\n");

    // Print maze after BFS (2 marks visited)
    cout << "Maze after BFS ( 2+ =visited):\n";
    for (int r = 0; r < leftnright; ++r) {
        for (int c = 0; c < upnown; ++c) {
            cout << maze[r][c];
            if (c + 1 < upnown) cout << ' ';
        }
        cout << '\n';

    }

      


                
         return 0;
}

