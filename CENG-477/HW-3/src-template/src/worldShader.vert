#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTex;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 MVP;

uniform sampler2D TexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

uniform float heightFactor;
uniform float imageWidth;
uniform float imageHeight;

out Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;


out vec3 LightVector;// Vector from Vertex to Light;
out vec3 CameraVector;// Vector from Vertex to Camera;


float get_height(in vec2 xy) {
    vec4 value = texture(TexGrey, xy);
    float height = value.r;
    return height * heightFactor;
}


void main()
{
    
    // Calculate texture coordinate based on data.TexCoord
    data.TexCoord = VertexTex;
    data.TexCoord.x += textureOffset * (1.0 / 250.0);
    data.Normal = VertexNormal;
    vec3 heightOffset = VertexNormal * get_height(data.TexCoord);
    vec3 calculated_pos = VertexPosition + heightOffset;
    CameraVector = normalize(cameraPosition - calculated_pos);
    LightVector = normalize(lightPosition - calculated_pos);
    gl_Position = MVP * vec4(calculated_pos.xyz, 1.0f);
}