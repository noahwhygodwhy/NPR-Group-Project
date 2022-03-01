#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "Model.hpp"
#include "Screen.hpp"
#include "DTriangle.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace glm;


int screenX = 1000;
int screenY = 1000;

int thousandsOfPoints = 5;

uint32_t depthFrameBuffer;
uint32_t depthTexture;
uint32_t depthRBO;

uint32_t normalFrameBuffer;
uint32_t normalTexture;
uint32_t normalRBO;

uint32_t colorFrameBuffer;
uint32_t colorTexture;
uint32_t colorRBO;

uint32_t lineFrameBuffer;
uint32_t lineTexture;
uint32_t lineRBO;

uint32_t pointFrameBuffer;
uint32_t pointTexture;
uint32_t pointRBO;

uint32_t coordFrameBuffer;
uint32_t coordTexture;
uint32_t coordRBO;


void regenFrameBuffer(uint32_t& fb, uint32_t& tex, uint32_t& rbo, int width, int height) {

    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fb);

    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
}



void genFrameBuffers() {
    regenFrameBuffer(depthFrameBuffer, depthTexture, depthRBO, screenX, screenY);
    regenFrameBuffer(normalFrameBuffer, normalTexture, normalRBO, screenX, screenY);
    regenFrameBuffer(colorFrameBuffer, colorTexture, colorRBO, screenX, screenY);
    regenFrameBuffer(lineFrameBuffer, lineTexture, lineRBO, screenX, screenY);
    regenFrameBuffer(pointFrameBuffer, pointTexture, pointRBO, 1000, thousandsOfPoints);
    //regenFrameBuffer(coordFrameBuffer, coordTexture, coordRBO, screenX, screenY);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {


    screenX = width;
    screenY = height;
    glViewport(0, 0, width, height);
    genFrameBuffers();
}void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


/*struct vVector {
    vec2 pos;
    vec2 centroid;
};

struct vTriangle {
    vVector a, b, c;

    vTriangle(vec4 i, vec4 j, vec4 k)
    {
        vec2 centroid = (vec2(i.x, i.y) + vec2(j.x, j.y) + vec2(k.x, k.y)) / 3.0f;
        this->a = vVector(vec2(i.x, i.y), centroid);
        this->b = vVector(vec2(j.x, j.y), centroid);
        this->c = vVector(vec2(k.x, k.y), centroid);
    }
};*/

vec4 calculatePolarPoints(const vec3& point, const vec2& polarCenter) {
    vec2 transPoint = vec2(point.x - 0.5, point.y - 0.5);
    double theta = atan2(transPoint.x, transPoint.y);
    if (point.y < 0.0) {
        theta += (2.0 * glm::pi<double>());
    }
    double r = length(transPoint);
     
    return vec4(point.x, point.y, r, theta);
}

//TODO: use texelfetch not texture()
unordered_map<string, Shader*> shaders;
Shader* shader;
int main()
{

    /*double x = 1;
    double y = -1;

    double z = atan2(y, x);

    if (y < 0.0) {
        z += (2.0 * glm::pi<double>());
    }

    printf("%f\n", glm::degrees(z));

    exit(-1);

    */
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

    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD init failed" << endl;
        exit(-1);
    }
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    frameBufferSizeCallback(window, screenX, screenY);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);

    Screen flatscreen;

    vector<Model*> models;
    models.push_back(new Model("backpack"));

    shaders["object"] = new Shader("object.vert", "object.frag");
    shaders["regular"] = new Shader("vertShader.glsl", "fragShader.glsl");
    shaders["normal"] = new Shader("vertShader.glsl", "normalFragShader.glsl");
    shaders["depth"] = new Shader("vertShader.glsl", "depthFragShader.glsl");
    shaders["simpleShader"] = new Shader("simpleVertShader.glsl", "simpleFragShader.glsl");
    shaders["line"] = new Shader("lineVertShader.glsl", "lineFragShader.glsl");
    shaders["point"] = new Shader("pointVertShader.glsl", "pointFragShader.glsl");
    shaders["coord"] = new Shader("tempPointDisplayVertShader.glsl", "tempPointDisplayFragShader.glsl");
    shaders["veronoi"] = new Shader("veronoiVertShader.glsl", "veronoiFragShader.glsl");




    mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);


    unsigned int fakeVAO, fakeEBO, fakeVBO;
    glGenVertexArrays(1, &fakeVAO);
    glGenBuffers(1, &fakeVBO);
    //glGenBuffers(1, &fakeEBO);
    glBindVertexArray(fakeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fakeVBO);
    glBufferData(GL_ARRAY_BUFFER, screenX*screenY, NULL, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_BYTE, GL_FALSE, sizeof(char), (void*)0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fakeEBO);
    glBindVertexArray(0);


    const size_t numberOfPoints = (1000 * thousandsOfPoints) + 4;//+4 corners 

    GLfloat* pointTexturePixels = new GLfloat[numberOfPoints *3];
    vec4* polarPoints = new vec4[numberOfPoints];


    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        /*shader = shaders["object"];
        shader.use();
        shader.setMatFour("projection", projection);
        shader.setMatFour("view", view);
        m.draw(shader);*/

        float sinit = (sin(currentFrame / 5.0f));
        float cosit = (cos(currentFrame / 5.0f));
        float ratio = (float)screenX / (float)screenY;

        vec3 eye = vec3(sinit * 5.0f, 2.0f, cosit * 5.0f);
        //vec3 eye = vec3(5.0, 5.0, 5.0);
        mat4 view = glm::lookAt(eye, vec3(0, 0, 0), vec3(0, 1, 0));

        //Draw the depth of the object
        glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["depth"];
        shader->use();
        shader->setVecThree("eyePos", eye);
        shader->setMatFour("view", view);
        for (Model* m : models) {
            float newNear = m->getNear(eye);
            float newFar = m->getFar(eye);
            shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
            shader->setFloat("near", newNear);
            shader->setFloat("far", newFar);
            m->draw(shader);


        }
        //draw the normal (really the direction) of the object
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, normalFrameBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["normal"];
        shader->use();
        shader->setVecThree("eyePos", eye);
        shader->setMatFour("view", view);
        for (Model* m : models) {
            float newNear = m->getNear(eye);
            float newFar = m->getFar(eye);
            shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
            shader->setFloat("near", newNear);
            shader->setFloat("far", newFar);
            m->draw(shader);
        }

        //draw the object regularly,like it would be drawn normally, but i can't use the word normal
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, colorFrameBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["object"];
        shader->use();
        shader->setVecThree("eyePos", eye);
        shader->setMatFour("view", view);
        for (Model* m : models) {
            float newNear = m->getNear(eye);
            float newFar = m->getFar(eye);
            shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
            shader->setFloat("near", newNear);
            shader->setFloat("far", newFar);

            //TODO: you have to give the model or mesh it's texture, and indicate to the draw function that it needs to do this only this time
            //take a look at collision test bed

            m->draw(shader);
        }


        //draw a combinatino of the previous framebuffers

//draws edges, white 
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, lineFrameBuffer);
        glClearColor(0.05f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["line"];
        shader->use();
        shader->setMatFour("projection", ortho);
        shader->setVecTwo("screenResolution", vec2(screenX, screenY));
        //shader.setMatFour("view", view);

        uint32_t normalLocation = glGetUniformLocation(shader->program, "normalTexture");
        uint32_t depthLocation = glGetUniformLocation(shader->program, "depthTexture");

        glUniform1i(normalLocation, 0);
        glUniform1i(depthLocation, 1);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        flatscreen.draw(shader);

 
        
 //use line data to create a texture of coordinates distributed weighted twoards the edges of objects
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, pointFrameBuffer);
        glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["point"];
        shader->use();
        shader->setVecThree("eyePos", eye);
        shader->setMatFour("projection", ortho);
        //shader.setMatFour("projection", glm::ortho(0.0f, 1.0f, 0.0f, 1.0f));
        //shader.setVecTwo("screenResolution", vec2(screenX, screenY));
        //shader.setMatFour("view", view);

        uint32_t lineLocation = glGetUniformLocation(shader->program, "lineTexture");

        glUniform1i(lineLocation, 0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, lineTexture);

        flatscreen.draw(shader);

//TEMPORARY --- draws the theoretical points to th screen to confirm that they're in correct locations
        /*glBindFramebuffer(GL_FRAMEBUFFER, coordFrameBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["coord"];
        shader->use();
        shader->setMatFour("projection", ortho);
        shader->setInt("width", screenX);
        shader->setInt("height", screenY);

        uint32_t pointLocation = glGetUniformLocation(shader->program, "pointTexture");

        glUniform1i(pointLocation, 0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, pointTexture);


        glBindVertexArray(fakeVAO);
        glDrawArrays(GL_POINTS, 0, screenX* thousandsOfPoints);
        glBindVertexArray(0);*/


//draw veronoi diagram
        /*glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["veronoi"];
        shader->use();
        shader->setMatFour("projection", ortho);
        shader->setInt("width", screenX);
        shader->setInt("height", screenY);

        uint32_t pointLocation = glGetUniformLocation(shader->program, "pointTexture");
        glUniform1i(pointLocation, 0);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, pointTexture);

        uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
        glUniform1i(colorLocation, 1);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, colorTexture);

        flatscreen.drawInstances(shader, 1000);*/



        //GLfloat* pixels = new GLfloat[screenX * screenY * 3];
        
        glBindTexture(GL_TEXTURE_2D, pointTexture);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pointTexturePixels);
        vec3* coords = (vec3*)pointTexturePixels;
        size_t baseIndex = numberOfPoints - 4;
        coords[baseIndex+0] = vec3(0.0, 0.0, 0.0);
        coords[baseIndex+1] = vec3(1.0, 0.0, 0.0);
        coords[baseIndex+2] = vec3(0.0, 1.0, 0.0);
        coords[baseIndex+3] = vec3(1.0, 1.0, 0.0);
        vec2 polarOrigin(0.5, 0.5);
        for (size_t i = 0; i < numberOfPoints; i++) {
            polarPoints[i] = calculatePolarPoints(coords[i], polarOrigin);
        }
        
        sort(polarPoints + 1, polarPoints + numberOfPoints, [](vec4 a, vec4 b) {
            if (glm::epsilonEqual(a.z, b.z, glm::epsilon<float>())) {
                return a.w < b.w;
            }
            return a.z < b.z;
        });

        unordered_set<DTriangle*> triangles = triangulate(polarPoints, numberOfPoints);


        /*vector<DTriangle*> triangles;
        triangles.push_back(new DTriangle(0, 1, 2, polarPoints));

        DFrontier frontier(triangles.at(0));
        size_t pIndex = 3;
        vec2 L, R;
        for (size_t i = 3; i < numberOfPoints; i++) {
            frontier.findEdge(polarPoints, polarOrigin, polarPoints[i], L, R);
            //create the triangle L, R, polarPoints[i]
            //give that triangle 
        }*/

        




        /*
        vec3 x0 = coords[0];

        sort(coords + 1, coords + numberOfPoints, [x0](vec3 a, vec3 b) {
            float adist = glm::length2(a - x0);
            float bdist = glm::length2(b - x0);
            return adist < bdist;
        });
        */




        cin.get();



        exit(-1);


//delaunay GPU_DT https://www.comp.nus.edu.sg/~tants/delaunay2DDownload_files/cao_hyp_2009.pdf
        


        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}