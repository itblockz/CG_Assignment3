#version 330

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture2D;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 ambientLight()
{
    float ambientStrength = 0.3f;
    vec3 ambient = lightColour * ambientStrength;
    return ambient;
}

vec3 diffuseLight()
{
    float diffuseStrength = 0.8f;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);

    float diff = max(dot(lightDir, norm), 0.0f);
    vec3 diffuse = diffuseStrength * diff * lightColour;
    return diffuse;
}

vec3 specularLight()
{
    float specularStrength = 0.8f;

    float shininess = 64.0f;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);

    vec3 reflectDir = reflect(-lightDir, norm);

    // Phong Shading
    float spec = pow(max(dot(reflectDir, viewDir), 0.0f), shininess);

    // Blinn Phong Shading
    // vec3 halfDir = (viewDir + lightDir) / 2;
    // float spec = pow(max(dot(halfDir, norm), 0), shininess);


    vec3 specular = specularStrength * spec * lightColour;
    return specular;
}

void main()
{
    colour = texture(texture2D, TexCoord) * vec4(ambientLight() + diffuseLight() + specularLight(), 1.0f);
}