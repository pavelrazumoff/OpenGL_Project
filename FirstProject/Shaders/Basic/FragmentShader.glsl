#version 430 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	vec4 diffuse_color;
	vec4 specular_color;
	float shininess;

	bool use_texture_diffuse;
	bool use_texture_specular;
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
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_DIR_LIGHTS		4
#define NR_POINT_LIGHTS		4
#define NR_SPOT_LIGHTS		4

uniform DirLight dirLight[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight[NR_SPOT_LIGHTS];

uniform int dirCount;
uniform int pointCount;
uniform int spotCount;

uniform Material material;

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec4 mat_diff;
vec4 mat_spec;

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);

	mat_diff = material.diffuse_color;
	if(material.use_texture_diffuse)
		mat_diff = texture(material.texture_diffuse1, TexCoords);

	mat_spec = material.specular_color;
	if(material.use_texture_specular)
		mat_spec = texture(material.texture_specular1, TexCoords);

	// phase 1: Directional lighting
	int lightsCount = dirCount;
	lightsCount = min(lightsCount, NR_DIR_LIGHTS);

	for(int i = 0; i < lightsCount; i++)
		result = vec4(CalcDirLight(dirLight[i], norm, viewDir), 1.0);

	// phase 2: Point lights
	lightsCount = pointCount;
	lightsCount = min(lightsCount, NR_POINT_LIGHTS);

	for(int i = 0; i < lightsCount; i++)
		result += vec4(CalcPointLight(pointLights[i], norm, FragPos, viewDir), 1.0);

	// phase 3: Spot light
	lightsCount = spotCount;
	lightsCount = min(lightsCount, NR_SPOT_LIGHTS);

	for(int i = 0; i < lightsCount; i++)
		result += vec4(CalcSpotLight(spotLight[i], norm, FragPos, viewDir), 1.0);

	result.a = mat_diff.a;

	if(result.a < 0.1)
		discard;

	FragColor = result;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
	light.quadratic * (distance * distance));

	// combine results
	vec3 ambient = light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * vec3(mat_diff);
    vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
    vec3 specular = light.specular * spec * vec3(mat_spec);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}