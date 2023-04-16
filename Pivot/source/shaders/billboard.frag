R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;
in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 frag_data; // depth, tocut, N/A, 1.0
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_pos_x;
layout (location = 4) out vec4 frag_pos_y;
layout (location = 5) out vec4 frag_pos_z;

uniform mat4 Mv;
uniform sampler2D billTex;

vec4 packFloat(const float value) {
    const vec3 bitSh = vec3(256.0 * 256.0, 256.0, 1.0);
    const vec3 bitMsk = vec3(0.0, 1.0 / 256.0, 1.0 / 256.0);
    vec3 res = fract((value + 2500.0) / 5000.0 * bitSh);
    res -= res.xxy * bitMsk;
    return vec4(res, 1.0);
}

void main(void) {

	frag_color = texture(billTex, outTexCoord);

	if (frag_color.a < 0.5) {
		discard;
	}
	frag_color.a = 1.0;
	frag_data = vec4(gl_FragCoord.z * 50, 1.0, 0.0, 1.0);
	frag_normal = vec4(0.0, 0.0, 0.0, 1.0);
	frag_pos_x = packFloat(pos.x);
	frag_pos_y = packFloat(pos.y);
	frag_pos_z = packFloat(pos.z);
}

/////////// SHADER END //////////)"
