R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;

out vec4 frag_color;

uniform mat4 Mv;
uniform sampler2D meshTexture;

void main(void) {
	//vec4 cur = texture(meshTexture, pos.xy / pos.z);
	frag_color = outColor;
}

/////////// SHADER END //////////)"
