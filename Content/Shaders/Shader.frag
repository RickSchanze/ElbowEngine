#version 450

layout(binding = 2) uniform sampler2D texSampler;

layout(constant_id = 0) const int MAX_POINT_LIGHTS = 1;

struct DirectionalLight {
    vec4 position;
    vec4 color;
};

layout(binding = 3) uniform PointLights {
    DirectionalLight lights[MAX_POINT_LIGHTS];
} ubo_point_lights;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPosition;
layout(location = 3) in vec3 inCameraPosition;

layout(location = 0) out vec4 outColor;

void main() {
    // 环境光
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * ubo_point_lights.lights[0].color.rgb;

    // 漫反射
    vec3 norm = normalize(inNormal);
    vec3 lightDir = normalize(ubo_point_lights.lights[0].position.rgb - inFragPosition);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * ubo_point_lights.lights[0].color.rgb;

    // 高光
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(inCameraPosition - inFragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * ubo_point_lights.lights[0].color.rgb;

    outColor = texture(texSampler, inUV) * vec4((ambient + diffuse + specular), 1.0f);
}
