#version 330

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 finalColor;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 viewPos;
uniform float brightness;

void main()
{
    vec3 col = vec3(0.0, 1.0, 1.0); // Setting material color

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * col;

    float d = 2.0;

    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 diffuse = max(dot(fragNormal, lightDir), 0.0) * lightCol * (d / distance(lightPos, fragPosition));

    // vec3 viewDir = normalize(viewPos - fragPosition);
    // float specularStrength = 0.5;
    // vec3 reflectDir = reflect(-lightDir, fragNormal);

    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // vec3 specular = specularStrength * spec * lightCol;


    vec3 final = (ambient + diffuse) * col;

    finalColor = vec4(final, 1);
}