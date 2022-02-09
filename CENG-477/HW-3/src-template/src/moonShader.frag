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

vec4 ambientReflectenceCoefficient = vec4(0.5f, 0.5f, 0.5f, 1.0f);
vec4 ambientLightColor = vec4(0.6f, 0.6f, 0.6f, 1.0f);
vec4 specularReflectenceCoefficient= vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 specularLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
float SpecularExponent = 10;
vec4 diffuseReflectenceCoefficient= vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 diffuseLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);


void main()
{
    // Calculate texture coordinate based on data.TexCoord
    vec2 textureCoordinate = data.TexCoord;
    // vec2 textureCoordinate = vec2(500, 320);
    vec4 texColor = texture(MoonTexColor, textureCoordinate);
    // vec4 texColor = vec4(0.5f, 0.5f, 0.5f, 0.5f);
    diffuseReflectenceCoefficient = vec4(texColor.x, texColor.y, texColor.z, 1.0f);

    vec3 ambient = (ambientReflectenceCoefficient * ambientLightColor).xyz;

    float cosTheta = max(dot(data.Normal, LightVector), 0.0f);
    vec3 diffuse = cosTheta * (diffuseLightColor * diffuseReflectenceCoefficient).xyz;

    vec3 reflected = reflect(-normalize(LightVector - 0.5), data.Normal);
    float cosAlpha = pow(max(dot(reflected, normalize(CameraVector + 5)), 0.0f), SpecularExponent);
    vec3 spec = cosAlpha * (specularLightColor * specularReflectenceCoefficient).xyz;

    vec3 computedSurfaceColor = ambient + diffuse + spec;
    FragColor = vec4(clamp(texColor.xyz * computedSurfaceColor, 0.0, 1.0), 1.0);

}
