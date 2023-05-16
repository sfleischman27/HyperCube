R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D albedoTexture;
uniform sampler2D replaceTexture;
uniform sampler2D normalTexture;
uniform sampler2D posTexture;
uniform vec3 vpos; // for specular only
uniform mat4 Mv; // for specular only
uniform vec3 color;
uniform vec3 lpos;
uniform float power;
uniform float numLights;

// Editable parameters for diffuse calculation
const float powerMult = 10000.0;
const float constAtten = 2000.0;
const float linearAtten = 0.0;
const float sqAtten = 1.0;

void main(void) {
    // Do not calculate if this area is cut
    if (texture(replaceTexture, outTexCoord).r == 0.0) {
        discard;
    }

    // Grab pixel components from texture
	vec3 alb = texture(albedoTexture, outTexCoord).xyz;
	vec3 norm = (texture(normalTexture, outTexCoord).xyz * 2.0) - vec3(1.0);
    float doLighting = texture(normalTexture, outTexCoord).a;
	vec3 pos = texture(posTexture, outTexCoord).xyz;

    // Diffuse calculation
    vec3 lightDir = normalize(lpos - pos);
	float lightDist = distance(lpos, pos);
    vec3 numerator = max(dot(norm, lightDir), 0.0) * alb * color * power * powerMult;
    float denominator = constAtten + (linearAtten * lightDist) + (sqAtten * lightDist * lightDist);
    vec3 diffuse = numerator / denominator;

    // Specular attempt
    //float specularStrength = 0.1;
    //vec3 viewDir = normalize(vpos - pos);
    //vec3 newVDir = (inverse(Mv) * vec4(viewDir, 1.0)).xyz;
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
    //vec3 specular = specularStrength * spec * color; 

	frag_color = vec4(diffuse, 1.0);
    if (doLighting == 0.0) {
        frag_color = vec4(alb / numLights, 1.0);
    }
}

/////////// SHADER END //////////)"
