#include "WindowGUI.h"

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    : window(glfwCreateWindow(width, height, title, monitor, share)) {
    //For future extensions, add here
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Unable to initialize window!");
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, WindowGUI_Methods::key_callback);
    glfwSetFramebufferSizeCallback(window, WindowGUI_Methods::framebuffer_size_callback);
}

void Window::run() const {
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Texture_Methods::activateTexture(*texture);
        Shader_Methods::useShaderProgram(*shaderProgram);
        Transform_Methods::have_3D_object_rotate(*shaderProgram);
        
        glBindVertexArray(*VAO);        
        glDrawElements(GL_TRIANGLES, Shape_Indices::Cube, GL_UNSIGNED_INT, 0);
            
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
}

void Transform_Methods::initialize3DField(glm::mat4& viewMat, glm::mat4& projectionMat, glm::mat4& modelMat)
{
    viewMat         = glm::mat4(1.0f);
    projectionMat   = glm::mat4(1.0f);
    modelMat        = glm::mat4(1.0f);
}

void Transform_Methods::have_3D_object_rotate(const uint32_t& shaderProgram) {
    glm::mat4 viewMat, projectionMat, modelMat;
    initialize3DField(viewMat, projectionMat, modelMat);

    modelMat = glm::rotate(modelMat, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(0.0f, 0.0f, -3.0f));
    projectionMat = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    uint32_t modelLoc = glGetUniformLocation(shaderProgram, "model");
    uint32_t viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projectionMat[0][0]);
}

void WindowGUI_Methods::framebuffer_size_callback(GLFWwindow* window, int height, int width) {
    glViewport(0, 0, height, width);
}

void WindowGUI_Methods::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    static bool wireframeMode = false;
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        wireframeMode = !wireframeMode;

        // Update OpenGL polygon mode
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
            std::cout << "Wireframe mode ON\n";
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            std::cout << "Wireframe mode OFF\n";
        }
    }
}


void WindowGUI_Methods::insertShader(Window* window, uint32_t* shaderProgram) {
    window->shaderProgram = shaderProgram;
}

void WindowGUI_Methods::insertVAO(Window* window, uint32_t* VAO) {
    window->VAO = VAO;
}

void WindowGUI_Methods::insertTexture(Window* window, uint32_t* Texture) {
    window->texture = Texture;
}
