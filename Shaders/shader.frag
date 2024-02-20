#version 330

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;

uniform sampler2D texture2D;
uniform vec3 lightColour;

vec4 ambientLight()
{
    float ambientStrength = 0.3;
    vec4 ambient = vec4(lightColour, 1.0f) * ambientStrength;
    return ambient;
}

void main()
{
    colour = texture(texture2D, TexCoord) * ambientLight();
}