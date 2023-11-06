#version 450 core

out vec4 FragColor;
// Structs
struct Light {
    vec3    position;
    vec3    color;
    float   intensity;              // Brightness factor
    int     type;                   // 1: Point light. 2: Directional light. 3: Spotlight.
    float   attenuationDistance;    // Light distance (1, 3)
    vec3    direction;              // Light direction (2, 3)
    float   innerAngle;             // Angle of ilumination cone (3)
    float   outerAngle;             // Angle of transition zone between ilumination and dark zone (3)
};

struct TextureUniform {
    sampler2D sTexture;
    // int       coefficient;
    // bool      useTexture;
};

uniform TextureUniform tDiffuse;
uniform TextureUniform tRoughness;
uniform TextureUniform tMetallic;
uniform TextureUniform tNormal;

// IN Variables
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;        // Vertex position in world coordinates

// Uniform
uniform sampler2D   texture_diffuse;
uniform vec3        camPosition;        // Camera position

// Light Uniforms
#define LIGHT_NUMBER 4
uniform Light lights[LIGHT_NUMBER];
uniform Light sunLight;
uniform vec4 ambientLight;

// Light macros
#define TYPE_POINT_LIGHT        1
#define TYPE_DIRECTIONAL_LIGHT  2
#define TYPE_SPOTLIGHT          3
#define LIGHT_DECAY_FACTOR      2

const float PI = 3.14159265359;

// Function declaration
vec4 calculateLightFromDirection(Light light, vec3 lightVector);
vec4 calculateLightFromDirectionPBR(Light light, vec3 lightVector, float roughness_texture, vec4 diffuse_texture, float metallic_texture, vec4 Lo, vec4 F0);
float calculateSpotlightAttenuation(Light light, vec3 lightVector);
float calculateDistanceAttenuation(Light light);

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec4 CookTorranceBRDF(vec3 H, vec3 N, vec3 L, vec3 V, float roughness_texture, vec4 diffuse_texture, float metallic_texture, vec4 diffuseLight, vec4 Lo, vec4 F0);
   
void main()
{
    // material properties
    vec4 diffuse_texture        = texture(tDiffuse.sTexture, TexCoords);
    float roughness_texture     = texture(tRoughness.sTexture, TexCoords).r;
    float metallic_texture      = texture(tMetallic.sTexture, TexCoords).r;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec4 F0 = vec4(0.04);
    //F0 = mix(F0, diffuse_texture, metallic_texture);

    // reflectance equation
    vec4 Lo = vec4(0.0);

    vec4 totalLight = vec4(0);

    // Check lights in the scene
    for(int i=0; i < LIGHT_NUMBER; i++) {
        if(lights[i].type == TYPE_POINT_LIGHT) {
            // Point lights
            vec3 lightVector    = normalize(lights[i].position - FragPos);
            vec4 lightAdd       = calculateLightFromDirectionPBR(lights[i], lightVector, roughness_texture, diffuse_texture, metallic_texture, Lo, F0);
            //calculateLightFromDirectionPBR(lights[i], lightVector, roughness_texture, diffuse_texture, metallic_texture, Lo, F0);//calculateLightFromDirection(lights[i], lightVector);
            float attenuation   = calculateDistanceAttenuation(lights[i]);

            totalLight += lightAdd * attenuation;
        }
        else if(lights[i].type == TYPE_DIRECTIONAL_LIGHT) {
            // Directionals lights
            totalLight          += calculateLightFromDirectionPBR(lights[i], normalize(lights[i].direction), roughness_texture, diffuse_texture, metallic_texture, Lo, F0);//calculateLightFromDirection(lights[i], normalize(lights[i].direction));
        }
        else if(lights[i].type == TYPE_SPOTLIGHT) {
            // Spotlight
            vec3 lightVector    = normalize(lights[i].position - FragPos);
            vec4 lightAdd       = calculateLightFromDirectionPBR(lights[i], lightVector, roughness_texture, diffuse_texture, metallic_texture, Lo, F0);//calculateLightFromDirection(lights[i], lightVector);
            float attenuation   = calculateSpotlightAttenuation(lights[i], lightVector);

            totalLight += lightAdd * attenuation;
        }
    }

    // Add sun light
    totalLight  += calculateLightFromDirectionPBR(sunLight, normalize(sunLight.direction), roughness_texture, diffuse_texture, metallic_texture, Lo, F0);//calculateLightFromDirection(sunLight, normalize(sunLight.direction));

    // Calculate final color
    //vec4 finalColor = texture(texture_diffuse, TexCoords) * (ambientLight + totalLight);
    vec4 finalColor = ambientLight + totalLight;
    vec3 finalColor_ = vec3(finalColor);
    // HDR tonemapping
    finalColor_ = finalColor_ / (finalColor_ + vec3(1.0));
    // gamma correct
    finalColor_ = pow(finalColor_, vec3(1.0/2.2));
    float alpha = finalColor.a;
    vec4 outColor = vec4(finalColor_, alpha);
    if(outColor.a < 0.03)
        discard;
    FragColor       = outColor;
}


vec4 calculateLightFromDirection(Light light, vec3 lightVector) {
    // Diffuse light
    vec3 normal             = normalize(Normal);

    float diffuseIntensity  = max(dot(normal, lightVector), 0.0);
    vec4 diffuseLight       = vec4(diffuseIntensity * light.intensity * light.color, 1.0);

    // Specular light
    vec3 viewVector         = normalize(camPosition - FragPos);
    vec3 reflectionVector   = reflect(-lightVector, normal);

    float specularIntensity =  pow(max(dot(viewVector, reflectionVector), 0.0), 32);
    vec4 specularLight      = vec4(specularIntensity * light.intensity * light.color, 1.0);

    return (specularLight + diffuseLight);
}

vec4 calculateLightFromDirectionPBR(Light light, vec3 lightVector, float roughness_texture, vec4 diffuse_texture, float metallic_texture, vec4 Lo, vec4 F0) {
    // Diffuse light
    vec3 normal             = normalize(Normal);

    float diffuseIntensity  = max(dot(normal, lightVector), 0.0);
    vec4 diffuseLight       = vec4(diffuseIntensity * light.intensity * light.color, 1.0);

    // Specular light
    vec3 viewVector         = normalize(camPosition - FragPos);
    vec3 reflectionVector   = reflect(-lightVector, normal);

    vec3 halfwayVector      = normalize(viewVector + lightVector);

    Lo              = CookTorranceBRDF(halfwayVector, normal, lightVector, viewVector, roughness_texture, diffuse_texture, metallic_texture, diffuseLight, Lo, F0);

    /*float specularIntensity = 0;
    if(tRoughness.useTexture == false){
        specularIntensity =  pow(max(dot(viewVector, reflectionVector), 0.0), tRoughness.coefficient);
    }else{
        specularIntensity =  pow(max(dot(viewVector, reflectionVector), 0.0), tRoughness.coefficient);
    }*/

    //vec4 specularLight      = vec4(specularIntensity * light.intensity * light.color, 1.0);

    //return (specularLight + diffuseLight);
    return Lo;
}

float calculateDistanceAttenuation(Light light) {
    float lightDistance     = length(light.position - FragPos);
    
    float linearFactor      = 32.0*0.14 / light.attenuationDistance;
    float quadraticFactor   = 0.2 / pow(light.attenuationDistance/20.0, 2);
    float attenuation       = 1.0 / (1.0 + linearFactor * lightDistance + quadraticFactor * lightDistance * lightDistance);
  
    return attenuation;
}

float calculateSpotlightAttenuation(Light light, vec3 lightVector) {
    float attenuation   = 0;
    float decayAngle    = dot(-lightVector, normalize(light.direction));

    if(decayAngle > light.outerAngle) {
        // Fragment inside spotlight area
        attenuation = calculateDistanceAttenuation(light);

        if(decayAngle < light.innerAngle) {
            // Fragment inside transition zone
            attenuation *= (decayAngle - light.outerAngle) / (light.innerAngle - light.outerAngle);
        }
    }

    return attenuation;
}

// //Cook Torrance Methods for specular and roughness calculation
// // ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// // ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// // ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// // ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// // ----------------------------------------------------------------------------

vec4 CookTorranceBRDF(vec3 H, vec3 N, vec3 L, vec3 V, float roughness_texture, vec4 diffuse_texture, float metallic_texture, vec4 diffuseLight, vec4 Lo, vec4 F0){
    vec3 F0_ = vec3(F0);
    vec3 Lo_ = vec3(Lo);
    vec3 diffuseLight_ = vec3(diffuseLight);
    vec3 diffuse_texture_ = vec3(diffuse_texture);
    float NDF = DistributionGGX(N, H, roughness_texture);
    float G   = GeometrySmith(N, V, L, roughness_texture);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0_);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specularBRDF = numerator / denominator;
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic_texture;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo_ += (kD * diffuse_texture_ / PI + specularBRDF) * diffuseLight_ * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    Lo = vec4(Lo_, 1.0);
    return Lo;
}