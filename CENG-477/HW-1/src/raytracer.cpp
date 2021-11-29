#include <iostream>
#include "parser.h"

// #include <chrono>
// using namespace std::chrono;

parser::Scene created_scene;             // Created scene as global value to extern it later

int main(int argc, char* argv[]) {

    // auto start = high_resolution_clock::now();

    created_scene.loadFromXml(argv[1]);  // Loda data
    created_scene.render_scene();        // Run raytracing algorithm

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // std::cout << "Time taken by function: <<< " << duration.count() << " >>> microseconds" << std::endl;
}
