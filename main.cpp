#include "main.h"

const unsigned int SCREEN_WIDTH  = 1280;
const unsigned int SCREEN_HEIGHT = 720;

bool zoomingIn = false;
bool zoomingOut = false;
bool panning = false;

int maxIterations = 64;
glm::dvec2 startPan = { 0.0, 0.0 };
glm::dvec2 screenOffset = { 0.0, 0.0 };
glm::dvec2 screenScale = { double(SCREEN_WIDTH) * 0.5, double(SCREEN_HEIGHT) * 1.0 };
glm::dvec2 d_mouseLoc;
glm::ivec2 mouseLoc;

glm::ivec2 pixelTL = { 0,0 };
glm::ivec2 pixelBR = { SCREEN_WIDTH, SCREEN_HEIGHT };
glm::dvec2 fractalTL = { -4.0,  2.0 };
glm::dvec2 fractalBR = {  1.0, -2.0 };

int main()
{
	// GLFW init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE); // disable double buffering to unlock framerate

	// GLFW window creation
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot Explorer", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	// load our shaders
	Shader ourShader("vertex.glsl", "fragment.glsl");

	// generate a quad that will cover the screen
	float screen_quad[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), screen_quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int fps = 0;
	int frameCount = 0;
	double previousTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// grab FPS
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - previousTime >= 1.0) {
			fps = frameCount;
			frameCount = 0;
			previousTime = currentTime;
		}

		// calculate region to render
		glfwGetCursorPos(window, &d_mouseLoc.x, &d_mouseLoc.y);
		mouseLoc = { floor(d_mouseLoc.x), floor(SCREEN_HEIGHT - d_mouseLoc.y) };
		camera_movement(window);
		screenToWorld(pixelTL, fractalTL);
		screenToWorld(pixelBR, fractalBR);

		// send data to GPU
		ourShader.use();
		ourShader.setInt("u_maxIter", maxIterations);
		ourShader.setVec2d("u_pixTL", pixelTL.x, pixelTL.y);
		ourShader.setVec2d("u_pixBR", pixelBR.x, pixelBR.y);
		ourShader.setVec2d("u_fracTL", fractalTL.x, fractalTL.y);
		ourShader.setVec2d("u_fracBR", fractalBR.x, fractalBR.y);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		double afterTime = glfwGetTime();
		double frameTime = afterTime - currentTime;

		std::stringstream s;
		s << "FPS: " << fps <<// " FrameTime: " << frameTime <<
			" // Iterations: " << maxIterations <<
			" // Region: " << fractalTL.x << "+" << fractalTL.y << "i, " <<
			fractalBR.x << "+" << fractalBR.y << "i" <<
			" // Mouse loc: (" << mouseLoc.x << ", " << mouseLoc.y << ")";
		glfwSetWindowTitle(window, s.str().c_str());

		glfwSwapBuffers(window);
		//glFlush(); // must manually flush since not double buffering
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	return 0;
}

// framebuffer resize callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// key press callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_R:
			zoomingIn = true;
			break;
		case GLFW_KEY_F:
			zoomingOut = true;
			break;
		case GLFW_KEY_W:
			screenOffset.y += 0.1;
			break;
		case GLFW_KEY_S:
			screenOffset.y -= 0.1;
			break;
		case GLFW_KEY_A:
			screenOffset.x -= 0.1;
			break;
		case GLFW_KEY_D:
			screenOffset.x += 0.1;
			break;
		case GLFW_KEY_UP:
			maxIterations += 64;
			break;
		case GLFW_KEY_DOWN:
			maxIterations -= 64;
			if (maxIterations < 64) maxIterations = 64;
			break;
		case GLFW_KEY_LEFT:
			maxIterations = (maxIterations + 1) / 2;
			break;
		case GLFW_KEY_RIGHT:
			maxIterations *= 2;
			break;
		case GLFW_KEY_F1:
			hq_render();
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		};
	}

	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_R:
			zoomingIn = false;
			break;
		case GLFW_KEY_F:
			zoomingOut = false;
			break;
		};
	}
}

// mouse press callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		panning = true;
		startPan = d_mouseLoc;
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		panning = false;
	}
}

// mouse wheel callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	glm::dvec2 mouseBeforeZoom;
	screenToWorld(mouseLoc, mouseBeforeZoom);
	if (yoffset < 0)
		screenScale *= 0.7f;
	if (yoffset > 0)
		screenScale *= 1.3f;
	glm::dvec2 mouseAfterZoom;
	screenToWorld(mouseLoc, mouseAfterZoom);
	screenOffset += (mouseBeforeZoom - mouseAfterZoom);
}

void camera_movement(GLFWwindow* window)
{
	glm::dvec2 mouseBeforeZoom;
	screenToWorld(mouseLoc, mouseBeforeZoom);

	if (zoomingIn)
		screenScale *= 1.03;
	if (zoomingOut)
		screenScale *= 0.97;

	glm::dvec2 mouseAfterZoom;
	screenToWorld(mouseLoc, mouseAfterZoom);
	screenOffset += (mouseBeforeZoom - mouseAfterZoom);

	if (panning) {
		screenOffset.x -= (d_mouseLoc.x - startPan.x) / screenScale.x;
		screenOffset.y += (d_mouseLoc.y - startPan.y) / screenScale.y;
		startPan = d_mouseLoc;
	}
}

void worldToScreen(glm::dvec2& world, glm::ivec2& screen)
{
	screen.x = int((world.x - screenOffset.x) * screenScale.x);
	screen.y = int((world.y - screenOffset.y) * screenScale.y);
}

void screenToWorld(glm::ivec2& screen, glm::dvec2& world)
{
	world.x = float(screen.x) / screenScale.x + screenOffset.x;
	world.y = float(screen.y) / screenScale.y + screenOffset.y;
}

void hq_render()
{
	glm::ivec3* pixels = new glm::ivec3[3840 * 2160];

	for (int y = 0; y < 2160; y++) {
		for (int x = 0; x < 3840; x++) {
			double x_scale = (fractalBR.x - fractalTL.x) / (double(pixelBR.x) - double(pixelTL.x));
			double y_scale = (fractalBR.y - fractalTL.y) / (double(pixelBR.y) - double(pixelTL.y));

			double cReal = x * x_scale + fractalTL.x;
			double cImag = y * y_scale + fractalTL.y;
			int iteration = 0;
			double zReal = 0.0;
			double zImag = 0.0;

			while (iteration < 1024) {
				double temp = zReal * zReal - zImag * zImag + cReal;
				zImag = zReal * zImag * 2.0 + cImag;
				zReal = temp;

				if ((zReal * zReal + zImag * zImag) > 4.0)
					break;

				iteration++;
			}

			float n = float(iteration);
			float a = 0.1f;
			pixels[y*x + x] = glm::ivec3(
				int((0.5f * sin(a * n) + 0.5f) * 255),
				int((0.5f * sin(a * n + 2.094f) + 0.5f) * 255),
				int((0.5f * sin(a * n + 4.188f) + 0.5f) * 255)
			);
		}
	}

	//stbi_write_png("out.png", 3840, 2160, 1, pixels, sizeof(glm::ivec3) * 3840);

	delete [] pixels;
}