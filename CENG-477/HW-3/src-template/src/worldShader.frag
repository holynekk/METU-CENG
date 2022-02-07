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

vec3 ambientReflectenceCoefficient = vec3(0.5f, 0.5f, 0.5f);
vec3 ambientLightColor = vec3(0.6f, 0.6f, 0.6f);
vec3 specularReflectenceCoefficient= vec3(1.0f, 1.0f, 1.0f);
vec3 specularLightColor = vec3(1.0f, 1.0f, 1.0f);
float SpecularExponent = 10;
vec3 diffuseReflectenceCoefficient= vec3(1.0f, 1.0f, 1.0f);
vec3 diffuseLightColor = vec3(1.0f, 1.0f, 1.0f);


void main()
{
    vec2 textureCoordinate = data.TexCoord;
    vec4 texColor = texture(TexColor, textureCoordinate);

    vec3 ambient = (ambientReflectenceCoefficient * ambientLightColor).xyz;

    float cosTheta = max(dot(data.Normal, LightVector), 0.0f);

    vec3 diffuse = cosTheta * (diffuseReflectenceCoefficient * diffuseLightColor).xyz;

    vec3 reflected = reflect(-normalize(LightVector - 0.5), data.Normal);
    float cosAlpha = pow(max(dot(reflected, normalize(CameraVector + 5)), 0.0f), SpecularExponent);

    vec3 spec = cosAlpha * (specularReflectenceCoefficient * specularLightColor).xyz;

    vec3 computedSurfaceColor = ambient + diffuse + spec;
    FragColor = vec4(clamp(texColor.xyz * computedSurfaceColor, 0.0, 1.0), 1.0);
}
