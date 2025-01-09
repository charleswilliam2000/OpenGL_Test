#include "WindowGUI.h"

void Screen::processMouseMovement(double xInPos, double yInPos) {
    static bool firstMouse = true;
    static float lastX = Constants::WINDOW_HEIGHT / 2;
    static float lastY = Constants::WINDOW_WIDTH / 2;

    float xpos = static_cast<float>(xInPos);
    float ypos = static_cast<float>(yInPos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera->handleMouseMovement(xOffset, yOffset);
}

void Screen::associateRenderer(Renderer& in_renderer) {
    renderer = &in_renderer;
}

void Screen::insertCamera(Camera& in_camera) {
    camera = &in_camera;
}

Screen::Screen(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    : _window(glfwCreateWindow(width, height, title, monitor, share)) {
    //For future extensions, add here
    if (!_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Unable to initialize window!");
    }

    glfwMakeContextCurrent(_window);

    glfwSetWindowUserPointer(_window, this);
    glfwSetKeyCallback(_window, Callbacks::key_callback);
    glfwSetCursorPosCallback(_window, Callbacks::mouse_callback);
    glfwSetFramebufferSizeCallback(_window, Callbacks::framebuffer_size_callback);

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Screen::run() const {
    while (!glfwWindowShouldClose(_window)) {
        camera->updateFrame();
        camera->handleCameraMovement(_window);

        glm::mat4 currCameraView = camera->updateCameraView();
        glm::vec3 currCameraPos = camera->getPosition();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->render(currCameraPos, currCameraView, wireframeMode);
            
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwDestroyWindow(_window);
}


void Callbacks::framebuffer_size_callback(GLFWwindow* window, int height, int width) {
    glViewport(0, 0, height, width);
}

void Callbacks::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Screen* windowInstance = static_cast<Screen*>(glfwGetWindowUserPointer(window));
    if (!windowInstance) return;

    windowInstance->processMouseMovement(xpos, ypos);
}

void Callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Screen* windowInstance = static_cast<Screen*>(glfwGetWindowUserPointer(window));
    if (!windowInstance) return;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    static bool wireframeMode = false;

    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        wireframeMode = !wireframeMode;

        if (wireframeMode) {
            windowInstance->wireframeMode = true;
            std::cout << "Wireframe mode ON\n";
        }
        else {
            windowInstance->wireframeMode = false;
            std::cout << "Wireframe mode OFF\n";
        }
    }
}