#version 330 core

out vec4 FragColor;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float inner_cutoff;
    float outer_cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform SpotLight spotLight;
uniform vec3 ViewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;

void main() {
    // TBN
    vec3 T = normalize(Tangent);
    vec3 B = normalize(Bitangent);
    vec3 N = normalize(Normal);
    mat3 TBN = mat3(T, B, N);

    // 노멀 맵에서 노멀 벡터 가져오기
    vec3 norm = texture(texture_normal1, TexCoords).rgb;

    // 벡터 범위를 [0, 1] -> [-1, 1] 로 변환
    norm = norm * 2.0 - 1.0;

    // tangent space --> world space
    norm = normalize(TBN * norm);

    // light direction
    vec3 light_direction = normalize(spotLight.position - FragPos);

    // ambient
    vec3 ambient = spotLight.ambient * vec3(texture(texture_diffuse1, TexCoords));

    // diffuse
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

    // specular
    vec3 view_direction = normalize(ViewPos - FragPos);
    vec3 reflect_direction = reflect(-light_direction, norm);
    float spec = pow(max(dot(reflect_direction, view_direction), 0.0), 32);
    vec3 specular = spotLight.specular * spec * vec3(texture(texture_specular1, TexCoords));
    
    // cutoff
    float cutoff = dot(-light_direction, spotLight.direction);
    float theta = cutoff - spotLight.outer_cutoff;
    float epsilon = spotLight.inner_cutoff - spotLight.outer_cutoff;
    float intensity = clamp((theta / epsilon), 0.0, 1.0);

    vec3 result = ambient + (diffuse + specular) * intensity;

    FragColor = vec4(result, 1.0);
}