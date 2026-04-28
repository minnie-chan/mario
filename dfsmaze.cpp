using namespace std;
#include <vector>
#include <iostream>
#include <algorithm>
    
    int maze[5][5] = { {0,0,1,1,1},
                       {1,0,1,1,1},
                       {1,0,1,1,1},
                       {1,0,1,1,1},
                       {1,0,0,0,0} };
                       
                 int upndown = 5;
        int leftnright = 5;
      
      bool isValid(int xx ,int yy){
         if(xx < 0 || xx >= leftnright || yy < 0 || yy >= upndown){
                        return false;
                } 
                
                if(maze[yy][xx] == 0){
                  return true;
                }else if(maze[yy][xx] == 1){
                    return false;
                }
             return false;


            };
                  vector<pair<int,int>> path;

            bool dfs(int X , int Y , vector<pair<int,int>> &path){
             
                 int y[] = {1,-1,0,0};
            int x[] = {0,0,-1,1};
             
            if(maze[Y][X] == 0){
                        maze[Y][X] = 2;
                             path.push_back({X,Y});
                         if(X == 4 && Y == 4 ){
                   
                    return true;
                    
                  } 
                    }
                 
                  
                   
                        for(int i = 0; i < 4; i++ ){
                             int   newX =  X + x[i];
                             int   newY = Y + y[i];
                            
                             if(isValid(newX, newY) ){
                                   if(dfs(newX , newY, path) == true){
                                    return true;
                                   }
                             }
                        };
                        path.pop_back();
                        return false;
            };
int main(){

    for(int i = 0; i < leftnright; i++){
        for(int j = 0; j < upndown; j++){
            cout << maze[i][j] << " ";
        }
        cout << endl;
    }
        if(dfs(0,0, path)  == true){
            cout << "path found" << '\n'; 
        } else { cout << "path not found " << '\n';}
        
            
          for(int i = 0; i < leftnright; i++){
        for(int j = 0; j < upndown; j++){
            cout << maze[i][j] << " ";
        }
        cout << endl;
    }
    
       for(auto& p : path){
        cout << "(" << p.first << "," << p.second << ")" << '\n';
       }


                
         return 0;
}

