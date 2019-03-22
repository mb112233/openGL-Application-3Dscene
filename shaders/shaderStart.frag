#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec2 fragTexCoords;
in vec4 fragLight;//for shadow computation
out vec4 fColor;

//point lighting
float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

//lighting
uniform	mat3 normalMatrix;
uniform float fogDensity;
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform	vec3 lightDir2;
uniform	vec3 lightColor2;
uniform sampler2D specularTexture;//sampler2D e textura
uniform sampler2D diffuseTexture;
uniform sampler2D depthMap;//for shadow computation
uniform mat4 view;
uniform mat4 model;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
		
	//compute light direction
	vec3 lightDirN = normalize(normalMatrix * lightDir);
	vec3 lightDir2N = normalize(normalMatrix * lightDir2);
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor / 2+max(dot(normalEye, lightDir2N), 0.0f) * lightColor2 / 2;
	
	
	//compute specular light
	//vec3 reflection = reflect(-lightDirN, normalEye);
	
	
	vec3 halfVector = normalize(lightDirN + viewDirN);
	vec3 halfVector2 = normalize(lightDir2N + viewDirN);
	//float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	float specCoeff2 = pow(max(dot(normalEye, halfVector2), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor / 2+specularStrength * specCoeff2 * lightColor2 / 2;
}
float computeFog()
{

 float fragmentDistance = length(fragPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}
void main() 
{
	vec4 colorFromTexture = texture(diffuseTexture, fragTexCoords);
	//if(colorFromTexture.a < 0.1)//.a este transparenta obiectului ,cand e1 e opac, cand e 0 e total transparent
	//	discard;//anuleaza colorarea bucatii aleia de fragment

	
	computeLightComponents();
	
	vec3 fragLightm = fragLight.xyz / fragLight.w * 0.5f + 0.5f;

    fragLightm.z = min(1.0f, max(0.0f, fragLightm.z));//clamp for z component

    float ambientO = min(1.0f, max(0.0f, (texture(depthMap, fragLightm.xy).r - fragLightm.z) * 10000.0f + 4.0f));
	
	vec3 baseColor = vec3(1.0f, 0.55f, 0.0f);//orange
	//.xyz transforma din vec4 in vec3
	ambient *= texture(diffuseTexture, fragTexCoords).xyz;
	diffuse *=  texture(diffuseTexture, fragTexCoords).xyz;
	specular *= texture(specularTexture, fragTexCoords).xyz;
	
	vec3 color = min((ambient + diffuse) + specular, 1.0f);
    //fColor = vec4(color,colorFromTexture.a );

    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	fColor = mix(fogColor, vec4(color,colorFromTexture.a ), fogFactor);
	//in loc de 1.0f=fully opaque, am adaugat componenta de transparenta
}
