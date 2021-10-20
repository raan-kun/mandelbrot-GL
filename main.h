#pragma once

// Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// std
#include <iostream>

// Local headers
#include "Shader.h"
//#include "stb_image_write.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void camera_movement(GLFWwindow* window);
void screenToWorld(glm::ivec2& screen, glm::dvec2& world);
void worldToScreen(glm::dvec2& world, glm::ivec2& screen);
void hq_render();