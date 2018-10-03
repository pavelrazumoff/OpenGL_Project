#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;

	vec4 diffuse_color;
	vec4 specular_color;
	float shininess;

	bool use_texture_diffuse;
	bool use_texture_specular;
	bool use_texture_normal;
	bool use_texture_height;
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

vec4 mat_diff;
vec4 mat_spec;

float AmbientOcclusion;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// retrieve data from gbuffer
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
	AmbientOcclusion = texture(ssao, TexCoords).r;

	vec3 viewDir = normalize(-FragPos); // viewpos is (0.0.0)

	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);

	// phase 1: Directional lighting
	int lightsCount = dirCount;
	lightsCount = min(lightsCount, NR_DIR_LIGHTS);

	for (int i = 0; i < lightsCount; i++)
		result = vec4(CalcDirLight(dirLight[i], Normal, viewDir), 1.0);

	// phase 2: Point lights
	lightsCount = pointCount;
	lightsCount = min(lightsCount, NR_POINT_LIGHTS);

	for (int i = 0; i < lightsCount; i++)
		result += vec4(CalcPointLight(pointLights[i], Normal, FragPos, viewDir), 1.0);

	// phase 3: Spot light
	lightsCount = spotCount;
	lightsCount = min(lightsCount, NR_SPOT_LIGHTS);

	for (int i = 0; i < lightsCount; i++)
		result += vec4(CalcSpotLight(spotLight[i], Normal, FragPos, viewDir), 1.0);

	result.a = mat_diff.a;

	if (result.a < 0.1)
		discard;

	FragColor = result;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir;
	lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = vec3(AmbientOcclusion) * light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	vec3 lighting;
	lighting = (ambient + diffuse + specular);

	return lighting;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir;
	lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// attenuation
	float l_distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * l_distance + light.quadratic * (l_distance * l_distance));
	//float attenuation = 1.0 / (gamma ? distance * distance : distance);

	// combine results
	vec3 ambient = vec3(AmbientOcclusion) * light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 lighting;
	lighting = (ambient + diffuse + specular);

	return lighting;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir;
	lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// combine results
	vec3 ambient = vec3(AmbientOcclusion) * light.ambient * vec3(mat_diff);
	vec3 diffuse = light.diffuse * diff * vec3(mat_diff);
	vec3 specular = light.specular * spec * vec3(mat_spec);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	vec3 lighting;
	lighting = (ambient + diffuse + specular);

	return lighting;
}
