R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 outColor;
in vec4 pos;
in vec2 outTexCoord;

layout (location = 0) out vec4 frag_color;
//layout (location = 1) out vec4 frag_replace; // if r == 0, then cut, else if r == 1.0 keep same
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_pos_x;
layout (location = 4) out vec4 frag_pos_y;
layout (location = 5) out vec4 frag_pos_z;
layout (location = 6) out vec4 frag_depth; // stored in r

uniform mat4 Mv;
uniform sampler2D billTex;

vec4 packFloat(const float value) {
	float transF = (value + 2500.0) / 50000.0;
	const vec4 bit_shift = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
    const vec4 bit_mask  = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
    vec4 res = fract(transF * bit_shift);
    res -= res.xxyz * bit_mask;
    return res;
}

void main(void) {

	frag_color = texture(billTex, outTexCoord);

	if (frag_color.a < 0.5) {
		discard;
	}
	frag_color.a = 1.0;
	//frag_replace = vec4(0.0, 0.0, 0.0, 1.0);
	frag_depth = vec4(gl_FragCoord.z * 50, 0.0, 0.0, 1.0);
	frag_normal = vec4(0.0, 0.0, 0.0, 1.0);
	frag_pos_x = packFloat(pos.x);
	frag_pos_y = packFloat(pos.y);
	frag_pos_z = packFloat(pos.z);
}

/////////// SHADER END //////////)"
