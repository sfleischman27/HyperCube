R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D replaceTexture; // can skip lighting calculation on "toCut" to save time
uniform sampler2D normalTexture;
uniform sampler2D posTexture;
uniform vec3 color;
uniform vec3 lpos;
uniform float power;
uniform vec3 vpos;
uniform mat4 Mv;

float unpackFloat(const vec4 value) {
    const vec4 bit_shift = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
    float transF = dot(value, bit_shift);
    return transF * 50000.0 - 2500.0;
}

void main(void) {
    if (texture(replaceTexture, outTexCoord).r == 0.0) {
        discard;
    }
	vec3 pos = texture(posTexture, outTexCoord).xyz;
	vec3 norm = (texture(normalTexture, outTexCoord).xyz * 2.0) - vec3(1.0);
	vec3 alb = texture(cutTexture, outTexCoord).xyz;

    // Diffuse
    vec3 lightDir = normalize(lpos - pos);
	float lightDist = distance(lpos, pos);
    float powerMult = 10000.0;
    float constAtten = 0.0;
    float linearAtten = 0.0;
    float sqAtten = 1.0;
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
}

/////////// SHADER END //////////)"
