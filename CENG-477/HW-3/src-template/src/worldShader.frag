#version 430

in Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;
in vec3 LightVector;
in vec3 CameraVector;

uniform vec3 lightPosition;
uniform sampler2D TexColor;
uniform sampler2D MoonTexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

out vec4 FragColor;

vec3 ambientReflectenceCoefficient = vec3(1.0f);
vec3 ambientLightColor = vec3(1.0f);
vec3 specularReflectenceCoefficient= vec3(1.0f);
vec3 specularLightColor = vec3(1.0f);
float SpecularExponent = 1;
vec3 diffuseReflectenceCoefficient= vec3(1.0f);
vec3 diffuseLightColor = vec3(1.0f);


void main()
{
    // Calculate texture coordinate based on data.TexCoord
    vec2 textureCoordinate = vec2(0, 0);
    vec4 texColor = texture(TexColor, textureCoordinate);

    vec3 ambient = vec3(0, 0, 0);    
    vec3 diffuse = vec3(0, 0, 0);
    vec3 spec = vec3(0, 0, 0);

    FragColor = vec4(ambient+diffuse+spec, 1.0f);


}
