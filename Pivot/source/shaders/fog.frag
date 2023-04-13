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
	frag_color = texture(depthTexture, outTexCoord);
	frag_color = texture(cutTexture, outTexCoord);
	//frag_color.a = 1.0 / numLights;
}

/////////// SHADER END //////////)"
