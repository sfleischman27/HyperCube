R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D cutTexture;
uniform sampler2D depthTexture;
uniform int numLights;

void main(void) {
	//float depth = texture(depthTexture, outTexCoord).r;
	//frag_color = vec4(depth, depth, depth, 1);
	frag_color = texture(cutTexture, outTexCoord);
	//frag_color.a = 1.0 / numLights;
}

/////////// SHADER END //////////)"
