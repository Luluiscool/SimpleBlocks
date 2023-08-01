#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 vertexCoord;

out vec4 finalColor;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 viewPos;
uniform sampler2D texture0;

void main()
{
    finalColor = vec4(1);
}