#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
#include "vec2Hash.hpp"
#include "delaunator.hpp"



#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

enum Stages {
    COLOR,
    KUWAHARA,
    MAX_STAGE


};

using namespace std;
using namespace glm;

int shaderMode = 1;
Stages stageUpTo = COLOR;
//constexpr int maxStage = 9;

double deltaTime = 0.0f;	// Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame

int screenX = 1000;
int screenY = 1000;

int numberOfPoints = 5000;


//drawn framebuffer texture
//struct DFT {
//
//    uint32_t framebuffer;
//    uint32_t texture;
//    uint32_t RBO;
//};


/*DFT depthDFT;
DFT normalDFT;
DFT colorDFT;
//DFT sobelDFT;
DFT lineDFT;
DFT pointDFT;
DFT coordDFT;
DFT veronoiDFT;

DFT stage1;
DFT stage2;
DFT stage3;


uint32_t stage4Framebuffer;
uint32_t stage4Texture1;
uint32_t stage4Texture2;
uint32_t stage4Texture3;
uint32_t stage4RBO;
*/


//void regenFrameBuffer(DFT& dft, int width = screenX, int height = screenY) {
//
//    glDeleteRenderbuffers(1, &dft.RBO);
//    glDeleteFramebuffers(1, &dft.framebuffer);
//
//    glGenFramebuffers(1, &dft.framebuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, dft.framebuffer);
//    glGenTextures(1, &dft.texture);
//    glBindTexture(GL_TEXTURE_2D, dft.texture);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dft.texture, 0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glGenRenderbuffers(1, &dft.RBO);
//    glBindRenderbuffer(GL_RENDERBUFFER, dft.RBO);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dft.RBO);
//}

void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}


//void genFrameBuffers() {
//    regenFrameBuffer(depthDFT);
//    regenFrameBuffer(normalDFT);
//    regenFrameBuffer(colorDFT);
//    //regenFrameBuffer(sobelDFT);
//    regenFrameBuffer(lineDFT);
//    regenFrameBuffer(pointDFT, numberOfPoints, 1);
//    regenFrameBuffer(veronoiDFT);
//
//    regenFrameBuffer(stage1);
//    regenFrameBuffer(stage2);
//    regenFrameBuffer(stage3);
//
//    glDeleteRenderbuffers(1, &stage4RBO);
//    glDeleteFramebuffers(1, &stage4Framebuffer);
//
//    glGenFramebuffers(1, &stage4Framebuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, stage4Framebuffer);
//
//    glGenTextures(1, &stage4Texture1);
//    glBindTexture(GL_TEXTURE_2D, stage4Texture1);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenX, screenY, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stage4Texture1, 0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glGenTextures(1, &stage4Texture2);
//    glBindTexture(GL_TEXTURE_2D, stage4Texture2);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenX, screenY, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, stage4Texture2, 0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glGenTextures(1, &stage4Texture3);
//    glBindTexture(GL_TEXTURE_2D, stage4Texture3);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenX, screenY, 0, GL_RGB, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, stage4Texture3, 0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    glGenRenderbuffers(1, &stage4RBO);
//    glBindRenderbuffer(GL_RENDERBUFFER, stage4RBO);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenX, screenY);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, stage4RBO);
//
//    
//}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {


    screenX = width;
    screenY = height;
    glViewport(0, 0, width, height);
    //genFrameBuffers();
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {

        stageUpTo = Stages((stageUpTo + 1) % (MAX_STAGE));
        if (stageUpTo < 0) {
            stageUpTo = Stages(MAX_STAGE-1);
        }
        printf("stageupto: %i\n", stageUpTo);
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        stageUpTo = Stages((stageUpTo - 1) % (MAX_STAGE));
        if (stageUpTo < 0) {
            stageUpTo = Stages(MAX_STAGE-1);
        }
        printf("stageupto: %i\n", stageUpTo);
    }

}

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
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
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

    glfwSetKeyCallback(window, keyCallback);

    //glFrontFace(GL_CCW);
    //glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);


    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);

    Screen flatscreen;

    vector<Model*> models;
    models.push_back(new Model("backpack"));
    //models.push_back(new Model("dedust"));

    shaders["object"] = new Shader("object.vert", "object.frag");
    //shaders["regular"] = new Shader("vertShader.glsl", "fragShader.glsl");
    shaders["normal"] = new Shader("vertShader.glsl", "normalFragShader.glsl");
    shaders["depth"] = new Shader("vertShader.glsl", "depthFragShader.glsl");
    shaders["sobel"] = new Shader("sobelVertShader.glsl", "sobelFragShader.glsl");
    //shaders["simpleShader"] = new Shader("simpleVertShader.glsl", "simpleFragShader.glsl");
    shaders["line"] = new Shader("lineVertShader.glsl", "lineFragShader.glsl");
    shaders["point"] = new Shader("pointVertShader.glsl", "pointFragShader.glsl");
    shaders["coord"] = new Shader("tempPointDisplayVertShader.glsl", "tempPointDisplayFragShader.glsl");

    //shaders["veronoi"] = new Shader("veronoiVertShader.glsl", "veronoiFragShader.glsl");
    //shaders["vPoints"] = new Shader("veronPointVertShader.glsl", "veronPointFragShader.glsl");

    shaders["thirdPartyDelaunay"] = new Shader("thirdVertShader.glsl", "thirdFragShader.glsl");


    shaders["kuwahara"] = new Shader("KuwaharaVertShader.glsl", "KuwaharaFragShader.glsl");



   // shaders["vPoints"] = new Shader("veronPointVertShader.glsl", "veronPointFragShader.glsl");





    mat4 ortho = glm::ortho(0.0f, 1.0f, 0.0f, float(screenX)/float(screenY));


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


    unsigned int triangleVAO, triangleVBO, triangleEBO;






    GLfloat* pointTexturePixels = new GLfloat[numberOfPoints *2];
    //vec4* polarPoints = new vec4[numberOfPoints];

    //vec2* pointsArray = new vec2[numberOfPoints];

    int frameCounter = -1;
    float frameTimes[30](0);
    int lastSecondFrameCount = -1;

    printf("begining of draw loop\n");

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameCounter++;
        if (int(currentFrame) > lastSecondFrameCount) {
            lastSecondFrameCount = int(currentFrame);
            float sum = 0;
            for (float f : frameTimes) {
                sum += f;
            }

            printf("fps: %f\n", sum / 30.0f);
        }
        frameTimes[frameCounter % 30] = 1.0f / float(deltaTime);





        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        float sinit = (sin(currentFrame / 5.0f));
        float cosit = (cos(currentFrame / 5.0f));
        float ratio = (float)screenX / (float)screenY;

        vec3 eye = vec3(sinit * 5.0f, 5.0f, cosit * 5.0f);
        //vec3 eye = vec3(5.0f, 200.0f, 5.0f);//for dedust
        //vec3 eye = vec3(5.0, 5.0, 5.0);
        mat4 view = glm::lookAt(eye, vec3(0, 0, 0), vec3(0, 1, 0));


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader = shaders["kuwahara"];
        shader->use();
        shader->setVecThree("eyePos", eye);
        shader->setMatFour("view", view);
        for (Model* m : models) {
            shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, 0.01f, 100.0f));
            m->draw(shader);
        }
        




        //a
// //Draw the depth of the object
//        if (stageUpTo >= DEPTH) {
//            if (stageUpTo > DEPTH) glBindFramebuffer(GL_FRAMEBUFFER, depthDFT.framebuffer);
//            if (stageUpTo == DEPTH) glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            if(false)printf("doing stage DEPTH\n");
//            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["depth"];
//            shader->use();
//            shader->setVecThree("eyePos", eye);
//            shader->setMatFour("view", view);
//            for (Model* m : models) {
//                float newNear = m->getNear(eye);
//                float newFar = m->getFar(eye);
//                shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
//                shader->setFloat("near", newNear);
//                shader->setFloat("far", newFar);
//                m->draw(shader);
//            }
//
//        }
//
//
////draw the normal (really the direction) of the object
//
//        if (stageUpTo >= NORMALS) {
//            if (stageUpTo > NORMALS) glBindFramebuffer(GL_FRAMEBUFFER, normalDFT.framebuffer);
//            if (stageUpTo == NORMALS) glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            if(false)printf("doing stage NORMALS\n");
//            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["normal"];
//            shader->use();
//            shader->setVecThree("eyePos", eye);
//            shader->setMatFour("view", view);
//            for (Model* m : models) {
//                float newNear = m->getNear(eye);
//                float newFar = m->getFar(eye);
//                shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
//                shader->setFloat("near", newNear);
//                shader->setFloat("far", newFar);
////                m->draw(shader);
////            } 
////        }
//
////draw the object regularly,like it would be drawn normally, but i can't use the word normal
//
//        if (stageUpTo >=COLOR) {
//            if (stageUpTo > COLOR) glBindFramebuffer(GL_FRAMEBUFFER, colorDFT.framebuffer);
//            if (stageUpTo == COLOR) glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            if(false)printf("doing stage COLOR\n");
//            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["object"];
//            shader->use();
//            shader->setVecThree("eyePos", eye);
//            shader->setMatFour("view", view);
//            for (Model* m : models) {
//                float newNear = m->getNear(eye);
//                float newFar = m->getFar(eye);
//                shader->setMatFour("projection", glm::perspective(glm::radians(70.0f), ratio, glm::max(newNear, 0.01f), newFar));
//                shader->setFloat("near", newNear);
//                shader->setFloat("far", newFar);
//                m->draw(shader);
//            }
//        }
//
////get edges using sobel operator
//
//
//        /*if (stageUpTo >= SOBEL) {
//            if (stageUpTo > SOBEL) { glBindFramebuffer(GL_FRAMEBUFFER, sobelDFT.framebuffer); }
//            if (stageUpTo == SOBEL) { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
//
//            if(false)printf("doing stage SOBEL\n");
//            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["sobel"];
//            shader->use();
//            shader->setMatFour("projection", ortho);
//            //shader->setVecTwo("screenResolution", vec2(screenX, screenY));
//
//            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
//
//            glUniform1i(colorLocation, 0);
//
//            glActiveTexture(GL_TEXTURE0 + 0);
//            glBindTexture(GL_TEXTURE_2D, colorDFT.texture);
//
//            flatscreen.draw(shader);
//        }*/
//
//
////draws edges, white 
//        if (stageUpTo >= LINE) {
//            if (stageUpTo > LINE) glBindFramebuffer(GL_FRAMEBUFFER, lineDFT.framebuffer);
//            if (stageUpTo == LINE) glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            if(false)printf("doing stage LINE\n");
//            glClearColor(0.02f, 0.0f, 0.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["line"];
//            shader->use();
//            shader->setMatFour("projection", ortho);
//            shader->setVecTwo("screenResolution", vec2(screenX, screenY));
//            //shader.setMatFour("view", view);
//
//            uint32_t normalLocation = glGetUniformLocation(shader->program, "normalTexture");
//            uint32_t depthLocation = glGetUniformLocation(shader->program, "depthTexture");
//            uint32_t colorLocation = glGetUniformLocation(shader->program, "colorTexture");
//
//            glUniform1i(normalLocation, 0);
//            glUniform1i(depthLocation, 1);
//            glUniform1i(colorLocation, 2);
//
//            glActiveTexture(GL_TEXTURE0 + 0);
//            glBindTexture(GL_TEXTURE_2D, normalDFT.texture);
//
//            glActiveTexture(GL_TEXTURE0 + 1);
//            glBindTexture(GL_TEXTURE_2D, depthDFT.texture);
//
//            glActiveTexture(GL_TEXTURE0 + 2);
//            glBindTexture(GL_TEXTURE_2D, colorDFT.texture);
//
//
//            flatscreen.draw(shader);
//        }
//        
// //use line data to create a texture of coordinates distributed weighted twoards the edges of objects
//        if (stageUpTo >= POINTS) {
//            //printf("doing points\n");
//            if (stageUpTo > POINTS) glBindFramebuffer(GL_FRAMEBUFFER, pointDFT.framebuffer);
//            if (stageUpTo == POINTS) glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            //if(false)printf("doing stage 4\n");
//            glClearColor(0.00f, 0.50f, 0.00f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["point"];
//            shader->use();
//            shader->setVecThree("eyePos", eye);
//            shader->setMatFour("projection", ortho);
//
//            uint32_t lineLocation = glGetUniformLocation(shader->program, "lineTexture");
//            glUniform1i(lineLocation, 0);
//            glActiveTexture(GL_TEXTURE0 + 0);
//            glBindTexture(GL_TEXTURE_2D, lineDFT.texture);
//
//            flatscreen.draw(shader);
//        }
//
////draws the theoretical points to th screen to confirm that they're in correct locations
//
//        if (stageUpTo == STATIC) {
//            glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            printf("doing static\n");
//            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["coord"];
//            shader->use();
//            shader->setMatFour("projection", ortho);
//            shader->setInt("width", screenX);
//            shader->setInt("height", screenY);
//
//            uint32_t pointLocation = glGetUniformLocation(shader->program, "pointTexture");
//            glUniform1i(pointLocation, 0);
//            glActiveTexture(GL_TEXTURE0 + 0);
//            glBindTexture(GL_TEXTURE_2D, pointDFT.texture);
//
//            glBindVertexArray(fakeVAO);
//            glDrawArrays(GL_POINTS, 0, numberOfPoints);
//            glBindVertexArray(0);
//        }
//        if (stageUpTo >= TRIANGLES) {
//            //printf("doing triangles\n");
//
//            glReadPixels(0, 0, numberOfPoints, 1, GL_RG, GL_FLOAT, pointTexturePixels);
//            unordered_set <dvec2> pointsSet;
//
//            for (size_t i = 0; i < numberOfPoints * 2; i+=2) {
//                pointsSet.insert(dvec2(pointTexturePixels[i], pointTexturePixels[i+1]));
//            }
//
//            vector<double> points;
//
//            for (dvec2 x : pointsSet) {
//                points.push_back(x.x);
//                points.push_back(x.y);
//            }
//
//            delaunator::Delaunator d = delaunator::Delaunator(points);
//
//            vector<float> verts;
//            vector<unsigned int> indexes;
//
//
//
//            for (size_t i = 0; i < d.triangles.size(); i += 3) {
//                size_t v1 = d.triangles[i + 0];
//                size_t v2 = d.triangles[i + 1];
//                size_t v3 = d.triangles[i + 2];
//
//                float centX = (d.coords[(v1 * 2) + 0] + d.coords[(v2 * 2) + 0] + d.coords[(v3 * 2) + 0]) / 3.0;
//                float centY = (d.coords[(v1 * 2) + 1] + d.coords[(v2 * 2) + 1] + d.coords[(v3 * 2) + 1]) / 3.0;
//
//                verts.push_back(d.coords[(v1 * 2) + 0]);
//                verts.push_back(d.coords[(v1 * 2) + 1]);
//                verts.push_back(centX);
//                verts.push_back(centY);
//                verts.push_back(d.coords[(v2 * 2) + 0]);
//                verts.push_back(d.coords[(v2 * 2) + 1]);
//                verts.push_back(centX);
//                verts.push_back(centY);
//                verts.push_back(d.coords[(v3 * 2) + 0]);
//                verts.push_back(d.coords[(v3 * 2) + 1]);
//                verts.push_back(centX);
//                verts.push_back(centY);
//                
//            }
//
//            glGenVertexArrays(1, &triangleVAO);
//            glBindVertexArray(triangleVAO);
//
//            glGenBuffers(1, &triangleVBO);
//            glGenBuffers(1, &triangleEBO);
//
//            glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
//            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), verts.data(), GL_STATIC_DRAW);
//
//            glEnableVertexAttribArray(0);
//            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4Ui64, (void*)0);
//
//            glEnableVertexAttribArray(1);
//            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4Ui64, (void*)(sizeof(float) * 2Ui64));
//
//            glBindVertexArray(0);
//
//
//            glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
//
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shader = shaders["thirdPartyDelaunay"];
//            shader->use();
//
//            shader->setMatFour("projection", ortho);
//
//            uint32_t colorLocationTri = glGetUniformLocation(shader->program, "colorTexture");
//            glUniform1i(colorLocationTri, 0);
//            glActiveTexture(GL_TEXTURE0 + 0);
//            glBindTexture(GL_TEXTURE_2D, colorDFT.texture);
//
//
//            glBindVertexArray(triangleVAO);
//            glDrawArrays(GL_TRIANGLES, 0, verts.size());
//
//        }

        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
        //cin.get();
    }
}