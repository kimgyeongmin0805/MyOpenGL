#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <iostream>
#include <vector>

#include "shader.h"
#include "camera.h"

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Shader cube_shader("cube.vs", "cube.fs");
    Shader light_shader("light.vs", "light.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    // VAO, VBO
    // --------
    // cube
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // light cube
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture
    // -------
    unsigned int texture_diffuse = load_texture("./resources/textures/container2_specular.png");
    unsigned int texture_specular = load_texture("./resources/textures/container2_specular.png");
    unsigned int texture_emission = load_texture("./resources/textures/matrix.jpg");
    cube_shader.use();
    cube_shader.setInt("material.diffuse", 0);
    cube_shader.setInt("material.specular", 1);
    cube_shader.setInt("material.emission", 2);
    cube_shader.setFloat("material.shininess", 32.0f);

	// render loop
	while (!glfwWindowShouldClose(window)) {
        float time_now = static_cast<float>(glfwGetTime());
        time_delta = time_now - time_last;
        time_last = time_now;

		processInput(window, time_delta);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_specular);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_emission);

        cube_shader.use();
        // directional light
        cube_shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        cube_shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        cube_shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        cube_shader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        // point light 1
        cube_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        cube_shader.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        cube_shader.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        cube_shader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cube_shader.setFloat("pointLights[0].constant", 1.0f);
        cube_shader.setFloat("pointLights[0].linear", 0.09f);
        cube_shader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        cube_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        cube_shader.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        cube_shader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        cube_shader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cube_shader.setFloat("pointLights[1].constant", 1.0f);
        cube_shader.setFloat("pointLights[1].linear", 0.09f);
        cube_shader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        cube_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        cube_shader.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        cube_shader.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        cube_shader.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cube_shader.setFloat("pointLights[2].constant", 1.0f);
        cube_shader.setFloat("pointLights[2].linear", 0.09f);
        cube_shader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        cube_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        cube_shader.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        cube_shader.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        cube_shader.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cube_shader.setFloat("pointLights[3].constant", 1.0f);
        cube_shader.setFloat("pointLights[3].linear", 0.09f);
        cube_shader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        cube_shader.setVec3("spotLight.position", camera.getPosition());
        cube_shader.setVec3("spotLight.direction", camera.getFront());
        cube_shader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        cube_shader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        cube_shader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cube_shader.setFloat("spotLight.constant", 1.0f);
        cube_shader.setFloat("spotLight.linear", 0.09f);
        cube_shader.setFloat("spotLight.quadratic", 0.032f);
        cube_shader.setFloat("spotLight.inner_cutoff", glm::cos(glm::radians(10.0f)));
        cube_shader.setFloat("spotLight.outer_cutoff", glm::cos(glm::radians(12.0f)));

        // draw cube
        cube_shader.use();
        glm::mat4 projection = camera.getPerspective();
        glm::mat4 view = camera.getLookAt();
        cube_shader.setMat4("projection", projection);
        cube_shader.setMat4("view", view);

        glBindVertexArray(cubeVAO);
        for (int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            cube_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // draw light
        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);
        
        glBindVertexArray(lightVAO);
        for (int i = 0; i < 4; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            light_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

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

unsigned int load_texture(const std::string& path) {
    unsigned int texture;
    int width, height, nrChannels;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (image) {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else {
        std::cout << "ERROR::STB_IMAGE::Failed to load texture image at path: " << path << std::endl;
    }
    stbi_image_free(image);

    return texture;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scrollProcess(static_cast<float>(yoffset));
}