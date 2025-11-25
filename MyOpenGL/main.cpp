#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <iostream>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, float delta_time);
unsigned int load_texture(const std::string& path);

const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float xpos_last;
float ypos_last;
bool firstMouse = true;
float time_last = 0.0f;
float time_delta = 0.0f;

int main() {
	// glfw init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "openGL", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader our_shader("shader.vs", "shader.fs");

    Model object("./resources/objects/backpack/backpack.obj");

	// render loop
	while (!glfwWindowShouldClose(window)) {
        float time_now = static_cast<float>(glfwGetTime());
        time_delta = time_now - time_last;
        time_last = time_now;

		processInput(window, time_delta);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        our_shader.use();
        our_shader.setVec3("spotLight.position", camera.getPosition());
        our_shader.setVec3("spotLight.direction", camera.getFront());
        our_shader.setFloat("spotLight.inner_cutoff", cos(glm::radians(8.0f)));
        our_shader.setFloat("spotLight.outer_cutoff", cos(glm::radians(10.0f)));
        our_shader.setVec3("spotLight.ambient", glm::vec3(0.1f));
        our_shader.setVec3("spotLight.diffuse", glm::vec3(1.0f));
        our_shader.setVec3("spotLight.specular", glm::vec3(1.0f));
        our_shader.setVec3("ViewPos", camera.getPosition());
        glm::mat4 projection = camera.getPerspective();
        glm::mat4 view = camera.getLookAt();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
        our_shader.setMat4("projection", projection);
        our_shader.setMat4("view", view);
        our_shader.setMat4("model", model);
        object.Draw(our_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float delta_time) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.movementProcess(CameraMovement::FORWARD, delta_time);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.movementProcess(CameraMovement::BACKWARD, delta_time);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.movementProcess(CameraMovement::RIGHT, delta_time);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.movementProcess(CameraMovement::LEFT, delta_time);
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.movementProcess(CameraMovement::UP, delta_time);
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.movementProcess(CameraMovement::DOWN, delta_time);
    }
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    if (firstMouse) {
        xpos_last = xpos;
        ypos_last = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - xpos_last;
    float yoffset = ypos_last - ypos;

    xpos_last = xpos;
    ypos_last = ypos;

    camera.angleProcess(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scrollProcess(static_cast<float>(yoffset));
}