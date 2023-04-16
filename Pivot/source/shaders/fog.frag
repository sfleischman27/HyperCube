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
	//vec4 n = texture(normalTexture, outTexCoord);
	//vec3 trueN = normalize(n.xyz * 2.0 - vec3(1.0, 1.0, 1.0));
	//frag_color = vec4(trueN, 1.0);
	vec3 diff = texture(cutTexture, outTexCoord).xyz;
	float d = texture(dataTexture, outTexCoord).r;
	d = d / 1.5; // longer reach
	vec3 fadeColor = vec3(1.0, 1.0, 1.0);
	frag_color.xyz = diff - fadeColor * d;
	frag_color.a = 1.0;
	//frag_color = vec4(0, 0, 0, 1); //TEMP
}

/////////// SHADER END //////////)"
