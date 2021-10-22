#version 400 core

in vec4 gl_FragCoord;

uniform int u_maxIter;
uniform dvec2 u_pixTL;
uniform dvec2 u_pixBR;
uniform dvec2 u_fracTL;
uniform dvec2 u_fracBR;

out vec4 FragColour;

int calc_iterations()
{
	double x_scale = (u_fracBR.x - u_fracTL.x) / (double(u_pixBR.x) - double(u_pixTL.x));
	double y_scale = (u_fracBR.y - u_fracTL.y) / (double(u_pixBR.y) - double(u_pixTL.y));
	int iteration = 0;
	// c = x + iy
	double c_x = gl_FragCoord.x * x_scale + u_fracTL.x;
	double c_y = gl_FragCoord.y * y_scale + u_fracTL.y;
	// z = x + iy
	double z_x = 0.0;
	double z_y = 0.0;
	// for optimisation
	double x2  = 0.0;
	double y2  = 0.0;

	// z(n+1) = z(n)^2 + c with optimisations
	while(x2 + y2 <= 4 && iteration < u_maxIter) {
		z_y = 2 * z_x * z_y + c_y;
		z_x = x2 - y2 + c_x;
		x2 = z_x * z_x;
		y2 = z_y * z_y;
		iteration++;
	}

	return iteration;
}

// basic colouring
// vec4 palette(int iterations)
// {
// 	if(iterations >= u_maxIter) {
// 		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
// 	}
// 	else {
// 		float iter_normalised = float(iterations) / float(u_maxIter);
// 		return vec4(0.0f, iter_normalised, iter_normalised, 1.0f);
// 	}
// }

// trig function colouring
vec4 palette(int iterations)
{
	if(iterations >= u_maxIter)
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);

	float n = float(iterations);
	float a = 0.1f;
	// return vec4(
	// 	0.15f * cos(a * n),
	// 	0.15f * max(min(cos(a * n + 2.094f),1),-1) + 0.5f,
	// 	0.33f * max(min(cos(a * n),1),-1) + 0.5f,
	// 	1.0f
	// );
	return vec4(
		0.5f * cos(a * n + 4.188f) + 0.25f,
		0.5f * sin(a * n + 4.188f) + 0.25f,
		0.5f * sin(a * n + 1.188f) + 0.25f,
		// 0.5f * sin(a * n + 1.188f) + 0.25f,
		1.0f
	);
	// return vec4(
	// 	0.5f * sin(a * n) + 0.5f,
	// 	0.5f * sin(a * n + 2.094f) + 0.5f,
	// 	0.5f * sin(a * n + 4.188f) + 0.5f,
	// 	1.0f
	// );
}

void main()
{
	// FragColour = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	FragColour = palette(calc_iterations());
}