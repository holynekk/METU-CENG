#ifndef SHADER_H
#define SHADER_H
using namespace std;

#include <iostream>
#include <string>
#include <fstream>
#include <jpeglib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint initShaders(const string& vertexShaderName, const string& fragmentShaderName);

GLuint initVertexShader(const string& filename);

GLuint initFragmentShader(const string& filename);

bool readDataFromFile(const string& fileName, string &data);

void initTexture(char *filename,int *w, int *h);

using namespace std;

#endif