#include <iostream>
#include "parser.h"

typedef unsigned char RGB[3];

parser::Scene created_scene;

int main(int argc, char* argv[])
{

    created_scene.loadFromXml(argv[1]);

    created_scene.renderScene();
}
