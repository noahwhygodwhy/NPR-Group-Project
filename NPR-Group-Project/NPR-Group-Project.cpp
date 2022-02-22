// NPR-Group-Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>`
#include "Shader.hpp"

using namespace std;
using namespace glm;

struct Vertex  {
    vec3 pos;
    vec3 color;
    Vertex(vec3 p, vec3 c) {
        this->pos = p;
        this->color = c;
    }
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, 1);
    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Title Goes here", NULL, NULL);

    if (window == NULL)
    {
        cout << "Window creation failed" << endl;
        exit(-1);
    }

    glfwMakeContextCurrent(window);



    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD init failed" << endl;
        exit(-1);
    }

    vector<Vertex> vertices = {
        Vertex(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0)),
        Vertex(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0)),
        Vertex(vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)),
    };
    vector<int> indices = {
        0, 1, 2
    };



    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glBindVertexArray(0);

    Shader simpleShader("simple.vert", "simple.frag");
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        mat4 projection = glm::perspective(radians(70.0), 1.0, 0.1, 100.0);
        vec3 eye = vec3(sin(currentTime) * 2.0, 0.0, cos(currentTime) * 2.0);
        mat4 view = glm::lookAt(eye, vec3(0.0), vec3(0.0, 1.0, 0.0));

        simpleShader.use();
        simpleShader.setMatFour("projection", projection);
        simpleShader.setMatFour("view", view);
        simpleShader.setMatFour("model", mat4(1.0));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        //processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
