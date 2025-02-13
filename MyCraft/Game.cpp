#include "Game.h"

void Game::processMouseMovement(double xInPos, double yInPos) {
    static bool firstMouse = true;
    static float lastX = CONSTANTS::WINDOW_HEIGHT / 2;
    static float lastY = CONSTANTS::WINDOW_WIDTH / 2;

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

    _camera->handleMouseMovement(xOffset, yOffset);
}

void Game::insertCamera(Camera* in_camera) {
    _camera = in_camera;
}

Game::Game(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
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

void Game::insertWorld(World* game) {
    _world = game;
}

void Game::run() const {
    if (!_window)
        throw std::runtime_error("Window not initialized!");

    if (!_camera)
        throw std::runtime_error("Camera not initialized!");
    
    Frustum cameraFrustum;
    

    while (!glfwWindowShouldClose(_window)) {
        _camera->updateFrame();
        _camera->handleCameraMovement(_window);
        cameraFrustum.createFrustumFromCamera(*_camera);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _world->render(*_camera, cameraFrustum, wireframeMode);

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    if (!_window)
        throw std::runtime_error("Window is null during destruction.");

    glfwDestroyWindow(_window);
}


void Callbacks::framebuffer_size_callback(GLFWwindow* window, int height, int width) {
    glViewport(0, 0, height, width);
}

void Callbacks::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Game* windowInstance = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!windowInstance) return;

    windowInstance->processMouseMovement(xpos, ypos);
}

void Callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    static bool VSYNC = true;
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        VSYNC = !VSYNC;

        if (VSYNC) {
            glfwSwapInterval(1);
            std::cout << "\nVSYNC ON";
        }
        else {
            glfwSwapInterval(0);
            std::cout << "\nVSYNC OFF";
        }
    }
}