#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float inner_cutoff;
    float outer_cutoff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define NR_POINT_LIGHTS 4

uniform vec3 cameraPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 camera_dir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 camera_dir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 camera_dir);

void main() {
    vec3 normal = normalize(Normal);
    vec3 camera_dir = normalize(cameraPos - FragPos);

    vec3 result = calcDirLight(dirLight, normal, camera_dir);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calcPointLight(pointLights[i], normal, camera_dir);
    }
    result += calcSpotLight(spotLight, normal, camera_dir);

    vec3 emission = vec3(texture(material.emission, TexCoords));
    if (vec3(texture(material.specular, TexCoords)) == vec3(0.0)) {
        result += emission;
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 camera_dir) {
    vec3 light_dir = normalize(-light.direction);
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // diffuse
    float diff = max(dot(normal, light_dir), 0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(reflect_dir, camera_dir), 0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 camera_dir) {
    vec3 light_dir = normalize(light.position - FragPos);
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // diffuse
    float diff = max(dot(normal, light_dir), 0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(reflect_dir, camera_dir), 0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // attenuation
    float distance = length(FragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 camera_dir) {
    vec3 light_dir = normalize(light.position - FragPos);
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // diffuse
    float diff = max(dot(normal, light_dir), 0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(reflect_dir, camera_dir), 0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    // attenuation
    float distance = length(FragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    // cutoff
    float cutoff = dot(-light_dir, normalize(light.direction));
    float intensity = clamp((cutoff - light.outer_cutoff) / (light.inner_cutoff - light.outer_cutoff), 0.0, 1.0);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}
