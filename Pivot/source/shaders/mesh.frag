R"(////////// SHADER BEGIN /////////

#ifdef CUGLES
precision mediump float;
#endif

in vec4 pos;
in vec2 outTexCoord;
in vec3 outNormal;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 frag_replace; // if r == 0, then cut, else if r == 1.0 keep same
layout (location = 2) out vec4 frag_normal;
layout (location = 3) out vec4 frag_pos_x;
layout (location = 4) out vec4 frag_pos_y;
layout (location = 5) out vec4 frag_pos_z;
layout (location = 6) out vec4 frag_depth; // stored in r

uniform mat4 Mv;
uniform sampler2D uTexture;
uniform vec3 uDirection;

vec4 packFloat(const float value) {
    const vec3 bitSh = vec3(256.0 * 256.0, 256.0, 1.0);
    const vec3 bitMsk = vec3(0.0, 1.0 / 256.0, 1.0 / 256.0);
    vec3 res = fract((value + 2500.0) / 50000.0 * bitSh);
    res -= res.xxy * bitMsk;
    return vec4(res, 1.0);
}

void main(void) {
	vec3 transNormal = outNormal;
    float cullOutside = 0.0; // set to 0.0 for cuts, set to 999.0 for visualization
	if (dot(uDirection, transNormal) <= cullOutside) {
        frag_color = vec4(1.0, 0.0, 1.0, 1.0);
		frag_replace = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
		frag_color = texture(uTexture, outTexCoord);

		frag_color.a = 1.0;

		frag_replace = vec4(1.0, 0.0, 0.0, 1.0);
	}
	frag_depth = vec4(gl_FragCoord.z * 50, 0.0, 0.0, 1.0);
	frag_normal = vec4((outNormal + vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
	frag_pos_x = packFloat(pos.x);
	frag_pos_y = packFloat(pos.y);
	frag_pos_z = packFloat(pos.z);
}

/////////// SHADER END //////////)"
