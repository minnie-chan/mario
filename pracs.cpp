#include <iostream>
using namespace std;
#include <string>

struct Node {
    int val;
    //Node* prev;
    Node* next;
};

class MyStack{
    private:
    
    Node* tail;

    public:
    MyStack(): tail(nullptr){}

    void enqueue(int x){
        Node* nw = new Node;
        nw->val = x;

        if(tail == nullptr){
            nw->next = nw;
            tail = nw;
        } else {
            nw->next = tail->next;
            tail->next = nw;
            tail = nw;
        }
    }

    bool empty(){
        if(tail->next == nullptr){
            return true;
        }

        return false;
    }
};

int main(){
    Queue st;

}