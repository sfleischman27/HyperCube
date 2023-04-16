R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D dataTexture; // can skip lighting calculation on "toCut" to save time
uniform sampler2D normalTexture;
uniform sampler2D posTexture;
uniform vec3 color;
uniform vec3 lpos;
uniform float power;
uniform vec3 vpos;

void main(void) {
	vec3 pos = texture(posTexture, outTexCoord).xyz * 500.0;
	vec3 norm = (texture(normalTexture, outTexCoord).xyz * 2.0) - vec3(1.0, 1.0, 1.0);
	vec3 alb = texture(cutTexture, outTexCoord).xyz;

    vec3 viewDir = normalize(vpos - pos);
    vec3 lightDir = normalize(lpos - pos);
	float lightDist = distance(lpos, pos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * alb * color * power * 10000 / (lightDist * lightDist);
	frag_color = vec4(diffuse, 1.0);
	if (norm == vec3(0.0, 0.0, 0.0)) {
		frag_color = vec4(1.0, 0.0, 0.0, 1.0);
	}
}

/////////// SHADER END //////////)"
