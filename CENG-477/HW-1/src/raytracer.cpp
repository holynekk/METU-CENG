#include <iostream>
#include "parser.h"


parser::Scene created_scene;            // Created scene as global value to extern it later

int main(int argc, char* argv[])
{

    created_scene.loadFromXml(argv[1]); // Loda data

    created_scene.renderScene();        // Run raytracing algorithm
}
