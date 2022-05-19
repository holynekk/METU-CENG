#include <iostream>

using namespace std;

int main(void) {
    // Grid sizes
    int grid_x, grid_y;
    // Take grid size input
    cin >> grid_x;
    // Create grid with inputs
    int** grid = new int*[grid_x];
    for(int i = 0; i < grid_x; ++i) {
        grid[i] = new int[grid_y];
    }
    // Fill grid with the input
    for(int i = 0; i < grid_x; ++i) {
        for(int j = 0; j < grid_y; ++j) {
            cin >> grid[i][j];
        }
    }
    // Take number of proper privates input
    int N_p;
    cin >> N_p;
        
    return 0;
}