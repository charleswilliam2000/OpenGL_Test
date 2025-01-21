#ifndef GAME_H
#define GAME_H

#include "stdafx.h"
#include "World.h"

namespace Callbacks {
    void framebuffer_size_callback(GLFWwindow* window, int height, int width);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}

class Game {
private:
    World* _world = nullptr;
    Camera* _camera = nullptr;
    GLFWwindow* _window = nullptr;
public:
    bool wireframeMode = false;

    Game(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

    void insertWorld(World* world);
    void run() const;
    void insertCamera(Camera* in_camera);
    void processMouseMovement(double xOffset, double yOffset);

    ~Game() {}
};

#endif // GAME_H