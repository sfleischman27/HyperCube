R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec2 outTexCoord;

out vec4 frag_color;

uniform sampler2D screenTexture;
uniform float blackFrac;
uniform float pixelFrac;

void main(void) {
	frag_color = texture(screenTexture, outTexCoord);
	frag_color.xyz = mix(frag_color.xyz, vec3(0.0), pixelFrac);
	frag_color.a = 1.0;
}

/////////// SHADER END //////////)"
