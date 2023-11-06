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

// IN Variables
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;        // Vertex position in world coordinates

// Uniform
uniform sampler2D   texture_diffuse;
uniform vec3        camPosition;        // Camera position

// Light Uniforms
#define LIGHT_NUMBER 8
uniform Light lights[LIGHT_NUMBER];
uniform Light sunLight;
uniform vec4 ambientLight;

// Light macros
#define TYPE_POINT_LIGHT        1
#define TYPE_DIRECTIONAL_LIGHT  2
#define TYPE_SPOTLIGHT          3
#define LIGHT_DECAY_FACTOR      2

// Function declaration
vec4 calculateLightFromDirection(Light light, vec3 lightVector);
float calculateSpotlightAttenuation(Light light, vec3 lightVector);
float calculateDistanceAttenuation(Light light);

void main()
{
    vec4 totalLight = vec4(0);

    // Check lights in the scene
    for(int i=0; i < LIGHT_NUMBER; i++) {
        if(lights[i].type == TYPE_POINT_LIGHT) {
            // Point lights
            vec3 lightVector    = normalize(lights[i].position - FragPos);
            vec4 lightAdd       = calculateLightFromDirection(lights[i], lightVector);
            float attenuation   = calculateDistanceAttenuation(lights[i]);

            totalLight += lightAdd * attenuation;
        }
        else if(lights[i].type == TYPE_DIRECTIONAL_LIGHT) {
            // Directionals lights
            totalLight          += calculateLightFromDirection(lights[i], normalize(lights[i].direction));
        }
        else if(lights[i].type == TYPE_SPOTLIGHT) {
            // Spotlight
            vec3 lightVector    = normalize(lights[i].position - FragPos);
            vec4 lightAdd       = calculateLightFromDirection(lights[i], lightVector);
            float attenuation   = calculateSpotlightAttenuation(lights[i], lightVector);

            totalLight += lightAdd * attenuation;
        }
    }

    // Add sun light
    totalLight  += calculateLightFromDirection(sunLight, normalize(sunLight.direction));

    // Clamp max light
    totalLight.x = min(ambientLight.x + totalLight.x, 1);
    totalLight.y = min(ambientLight.y + totalLight.y, 1);
    totalLight.z = min(ambientLight.z + totalLight.z, 1);

    // Calculate final color
    vec4 finalColor = texture(texture_diffuse, TexCoords) * (totalLight);
    FragColor       = finalColor;
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