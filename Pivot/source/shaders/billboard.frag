R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;
flat in int texBindPoint;
in vec2 outTexCoord;

out vec4 frag_color;

uniform mat4 Mv;
uniform sampler2DArray textureList;

void main(void) {
	
	//frag_color = texture(textureList[texBindPoint], outTexCoord);

	// TODO added depth stuff here but should use a depth buffer in FSQ
	float d = -(Mv * pos).z;
	float maxDepth = 35.0;
	float ratio = d / maxDepth;
	vec4 fadeColor = vec4(0.1, 0.1, 0.1, 1.0);
	frag_color = frag_color - fadeColor * ratio;
	frag_color.a = 1.0;

	//frag_color = vec4(outTexCoord, 0, 1);
	frag_color = texture(textureList, vec3(.5, .5, texBindPoint));
}

/////////// SHADER END //////////)"
