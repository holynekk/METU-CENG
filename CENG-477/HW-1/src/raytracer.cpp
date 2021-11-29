#include <iostream>
#include "parser.h"

parser::Scene created_scene;             // Created scene as global value to extern it later

int main(int argc, char* argv[]) {
    created_scene.loadFromXml(argv[1]);  // Loda data
    created_scene.render_scene();        // Run raytracing algorithm
}

// To calculate execution time

/*

    #include <chrono>

    using namespace std;
    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    <function> that you want to calculate execution

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: <<< " << duration.count() << " >>> microseconds" << endl;
*/