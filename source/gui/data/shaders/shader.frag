#version 460 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Strength
{
    float ambient;
    float diffuse;
    float specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 cameraPosition;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 lightGlow;

uniform vec3 objectColor;
uniform float objectAlpha;

uniform Strength strength;

uniform bool useMaterial;
uniform Material material;

uniform bool useTexture;
uniform sampler2D texture_diffuse;

vec3 getLightDir(){
    return normalize((cameraPosition + lightPosition) - FragPos);
}

vec3 computeAmbientComponent(){
    return strength.ambient * lightColor;
}

vec3 computeDiffuseComponent(){
    vec3 norm = normalize(Normal);
    vec3 lightDir = getLightDir();
    float diff = max(dot(norm, lightDir), 0.0);
    return strength.diffuse * diff * lightColor;
}

vec3 computeSpecularComponent(){
    vec3 viewDir = normalize(cameraPosition - FragPos);
    vec3 lightDir = getLightDir();
    vec3 norm = normalize(Normal);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    return strength.specular * spec * lightColor;
}

vec3 computeBasicShading(){
    vec3 ambient = computeAmbientComponent();
    vec3 diffuse = computeDiffuseComponent();
    vec3 specular = computeSpecularComponent();
    return ambient + diffuse + specular;
}

vec3 computeGlowDirection(vec3 direction, vec3 color, vec3 normal) {
    float diff = max(dot(normal, direction), 0.0);
    vec3 diffuse  = diff * color;
    return diffuse;
}

void main()
{
    vec3 viewDir = normalize(cameraPosition - FragPos);
    vec3 norm = normalize(Normal);

    if (useTexture == true){
        vec3 color = computeBasicShading();
        color += computeGlowDirection(-viewDir, lightGlow, norm)* color;
        FragColor = vec4(color, objectAlpha) * texture(texture_diffuse, TexCoords);
    }
    else if (useMaterial == true) {
        vec3 ambient = material.ambient * computeAmbientComponent();
        vec3 diffuse = material.diffuse * computeDiffuseComponent();
        vec3 specular = material.specular * computeSpecularComponent();
        vec3 color = ambient + diffuse + specular;
        color += computeGlowDirection(-viewDir, lightGlow, norm)* color;
        FragColor = vec4(color, objectAlpha);
    } else {
        vec3 color = computeBasicShading() * objectColor;
        color += computeGlowDirection(-viewDir, lightGlow, norm)* color;
        FragColor = vec4(color, objectAlpha);
    }
}
