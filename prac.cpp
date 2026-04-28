#include <iostream>
using namespace std;
#include <chrono>
#include <cstdlib>
#include <ctime>

    void  fill_array_random(int array[], int size, int lbounds, int ubounds){
        
        for(int i = 0; i < size; i++){
            array[i] = (rand() % (ubounds - lbounds)) + lbounds;
        }
    }
    void fill_array_inc(int array[], int size){
        for(int i = 0; i < size; i++){
            array[i] = i + 1;
        }
    }
    void fill_array_dec(int array[], int size){
        for (int i = 0; i < size; i++) {
            array[i] = size - i;
        }
    }
    void show_array(int array[], int size) {
        for (int i = 0; i < size; i++) {
            cout << array[i] << " ";
        }
        cout << endl;
    }
    void bubble_sort(int array[], int size){
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - 1 - i; j++) {
                if (array[j] > array[j + 1]) {
                    int temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }   
            }
        }
    }
    void insertion_sort(int array[], int size){
        for (int i = 1; i < size; i++) {
            int key = array[i];
            int j = i - 1;

            while (j >= 0 && array[j] > key) {
                array[j + 1] = array[j];
                j--;
            }

            array[j + 1] = key;
        }
    }

int main()
{
  srand(time(NULL));
  // more code here
    int arraysize;
    cout << "Enter array size: ";
    cin >> arraysize;

    int* array = new int[arraysize];
  cout << "Bubble sort (Random) took ";
  fill_array_random(array, arraysize, 1000, 1000000);

  auto start = chrono::steady_clock::now();
  bubble_sort(array, arraysize);
  auto end = chrono::steady_clock::now();
  cout << chrono::duration_cast<chrono::milliseconds>(end-start).count() << endl;

  cout << "Bubble sort (Increasing) took ";
  fill_array_inc(array, arraysize);

  start = chrono::steady_clock::now();
  bubble_sort(array, arraysize);
  end = chrono::steady_clock::now();
  cout << chrono::duration_cast<chrono::milliseconds>(end-start).count() << endl;

  cout << "Bubble sort (Decreasing) took ";
  fill_array_dec(array, arraysize);

  start = chrono::steady_clock::now();
  bubble_sort(array, arraysize);
  end = chrono::steady_clock::now();
  cout << chrono::duration_cast<chrono::milliseconds>(end-start).count() << endl;

  delete[] array;
}
