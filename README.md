![render](https://user-images.githubusercontent.com/11539931/161137397-9b044f41-9869-4e4c-b150-be6b16ee1e6a.png)

# mandelbrot-GL
An interactive, GPU-accelerated Mandelbrot Set viewer using OpenGL.

## Details
When run, the program launches an OpenGL context in a window and begins listening for events and rendering. Every frame, the area of the set to be calcuated is sent to the GPU. The set is computed and rendered with an OpenGL fragment shader that runs the classic [escape time algorithm](https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Escape_time_algorithm) on every visible pixel. The program supports up to 64-bit precision, which allows one to zoom quite far into the set (zooming past the limits of double precision yields very interesting effects).

## Compilation
This program has the following dependencies:
- An OpenGL-compatible system
- GLFW3
- GLAD with GL_ARB_gpu_shader_fp64 extension enabled (this can be generated [here](https://glad.dav1d.de/))
- GLM

Load solution into Visual Studio and build & run.

Please note that all 3 dependencies are dynamically linked, so ensure that your Visual Studio is set up to point at the appropriate headers.

Compilation on other platforms *should* be possible as there is no Win32-specific code, but this is untested and a build system for doing so is not provided.

## Controls
Pan the view by clicking and dragging.

Zoom in and out using either the scroll wheel or by holding the `R` or `F` key.

Use the `arrow up` (↑) or `arrow down` (↓) keys to increase/decrease the maximum number of iterations the algorithm can run for. This lets you retain detail when zooming in further, but comes at a hefty performance cost when reaching high values. The current limit on iterations is displayed in the window decoration.

## Todos
- Hotkey to make a high-res offline render of the current view
- GUI with Dear ImGui (display metrics/current co-ordinates/iteration count, allow changing max iterations/colouring algorithms/other parameters)
- Other (hopefully faster) plotting algorithms
