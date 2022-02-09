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


void main()
{
    // Assign pre-computed texture coordinates
    data.TexCoord = VertexTex;
    data.TexCoord.x += textureOffset * (1.0 / 250.0);
    // Assign pre-computed vertex normal
    data.Normal = VertexNormal;
    // Calculate new coordinates of vertices
    vec4 value = texture(TexGrey, data.TexCoord);
    float height = value.r;
    vec3 heightOffset = data.Normal * value.r * heightFactor;
    vec3 new_vertex_position = VertexPosition + heightOffset;
    // Assign vertex to light and vertex to camera vectors
    LightVector = normalize(lightPosition - new_vertex_position);
    CameraVector = normalize(cameraPosition - new_vertex_position);
    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP * vec4(new_vertex_position.xyz, 1.0f);
}