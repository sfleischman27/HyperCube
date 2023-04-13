R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D dataTexture;
uniform sampler2D normalTexture;

void main(void) {
	//float depth = texture(dataTexture, outTexCoord).r;
	//frag_color = vec4(depth, depth, depth, 1);
	vec4 n = texture(normalTexture, outTexCoord);
	vec3 trueN = normalize(n.xyz * 2.0 - vec3(1.0, 1.0, 1.0));
	frag_color = vec4(trueN, 1.0);
	frag_color = texture(cutTexture, outTexCoord);
}

/////////// SHADER END //////////)"
