#version 330

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;

uniform sampler2D texture2D;

void main()
{
    colour = texture(texture2D, TexCoord);
}