R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D dataTexture; // can skip lighting calculation on "toCut" to save time
uniform sampler2D normalTexture;
uniform sampler2D posTextureX;
uniform sampler2D posTextureY;
uniform sampler2D posTextureZ;
uniform vec3 color;
uniform vec3 lpos;
uniform float power;
uniform vec3 vpos;

float unpackFloat(const vec4 value) {
    const vec3 bitSh = vec3(1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    return dot(value.xyz, bitSh) * 50000.0 - 2500.0;
}

void main(void) {
	vec4 packedX = texture(posTextureX, outTexCoord);
	vec4 packedY = texture(posTextureY, outTexCoord);
	vec4 packedZ = texture(posTextureZ, outTexCoord);
	vec3 pos = vec3(unpackFloat(packedX), unpackFloat(packedY), unpackFloat(packedZ));
	vec3 norm = (texture(normalTexture, outTexCoord).xyz * 2.0) - vec3(1.0, 1.0, 1.0);
	vec3 alb = texture(cutTexture, outTexCoord).xyz;

    vec3 viewDir = normalize(vpos - pos);
    vec3 lightDir = normalize(lpos - pos);
	float lightDist = distance(lpos, pos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * alb * color * power * 10000 / (lightDist * lightDist);
	frag_color = vec4(diffuse, 1.0);
}

/////////// SHADER END //////////)"
