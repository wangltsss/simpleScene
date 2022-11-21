//
//  main.cpp
//  assignment3
//
//  Created by 王沈同 on 2022-11-20.
//

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <tiny_obj_loader.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height){
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte * pixels = new GLubyte [totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id ++;
    delete [] pixels;
    fout.flush();
    fout.close();
}

//key board control
void processInput(GLFWwindow *window){
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    //press p to capture screen
    if(glfwGetKey(window, GLFW_KEY_P) ==GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("/Users/wangst/Documents/McMaster/2022 Fall/CS3GC3/assignment3/submission/", buffer_width, buffer_height);
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
/* ----------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------- No Need to Configure End ----------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */


// Shaders Configuration
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    //"layout (location = 1) in vec3 aNorm;\n"
    "layout (location = 2) in vec2 aTex;\n"
    "uniform mat4 MVP;\n"
    //"out vec3 Normal;\n"
    "out vec2 UV;\n"
    "void main()\n"
    "{\n"
    "   UV = aTex;\n"
    //"   Normal = aNorm;\n"
    "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "uniform sampler2D myTexture;\n"
    //"in vec3 Normal;\n"
    "in vec2 UV;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(myTexture, UV);\n"
    "}\0";


/* ----------------------------------------------------------------------------------------------------------- */
/* -------------------------------------- No Need to Configure Start ----------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
int main()
{
    // version info
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
/* ----------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------- No Need to Configure End ----------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
    
    
    // resolution and window title
    int windowWidth = 1024;
    int windowHeight = 768;
    char ttl[] = "Assignment2";
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, ttl, NULL, NULL);
    
    
/* ----------------------------------------------------------------------------------------------------------- */
/* -------------------------------------- No Need to Configure Start ----------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
    // window creation and excpetion catch
    if (window == NULL)
    {
        std::cout << "GLFW Window Failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return -1;
    }
    
    
    // shaders creation and exception catch
    int success;
    char error_msg[512];
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vs, 512, NULL, error_msg);
        std::cout << "Vertex Shader Failed: " << error_msg << std::endl;
    }
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fs, 512, NULL, error_msg);
        std::cout << "Fragment Shader Failed: " << error_msg << std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    
    
    // link program and exception catch
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_msg);
        std::cout << "Program Link Error: " << error_msg << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
/* ----------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------- No Need to Configure End ----------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
    
    
    // get a handle for MVP uniform
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    
    
    // model, view, projection matrices definition
    glm::mat4 Model = glm::mat4(1);
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f/3.0f, 0.1f, 1000.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(50, 100, 200),
                                 glm::vec3(0, 80, 0),
                                 glm::vec3(0, 1, 0));
    glm::mat4 MVP = Projection * View * Model;
    
/* ----------------------------------------------------------------------------------------------------------- */


    std::vector<float> vbuffer0;
    std::vector<float> nbuffer0;
    std::vector<float> tbuffer0;
    
    char filename0[] = "/Users/wangst/Documents/McMaster/2022 Fall/CS3GC3/assignment3/asset/bucket.obj";
    tinyobj::attrib_t attrib0;
    std::vector<tinyobj::shape_t> shapes0;
    std::vector<tinyobj::material_t> materials0;
    // tinyobj load obj
    std::string warn0, err0;
    bool bTriangulate0 = true;
    bool bSuc0 = tinyobj::LoadObj(&attrib0, &shapes0, &materials0, &warn0, &err0, filename0, nullptr, bTriangulate0);
    for (auto id: shapes0[0].mesh.indices){
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        
        vbuffer0.push_back(attrib0.vertices[vid*3]);
        vbuffer0.push_back(attrib0.vertices[vid*3+1]);
        vbuffer0.push_back(attrib0.vertices[vid*3+2]);
        nbuffer0.push_back(attrib0.normals[nid*3]);
        nbuffer0.push_back(attrib0.normals[nid*3+1]);
        nbuffer0.push_back(attrib0.normals[nid*3+2]);
        tbuffer0.push_back(attrib0.texcoords[tid*2]);
        tbuffer0.push_back(attrib0.texcoords[tid*2+1]);
    }
    
    int x0, y0, n0;
    char tex_path0[] = "/Users/wangst/Documents/McMaster/2022 Fall/CS3GC3/assignment3/asset/bucket.jpg";
    stbi_info(tex_path0, &x0, &y0, &n0);
    stbi_set_flip_vertically_on_load(true);
    unsigned char *tdata0 = stbi_load(tex_path0, &x0, &y0, &n0, 0);
    
    unsigned int texture0;
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x0, y0, 0, GL_RGB,
    GL_UNSIGNED_BYTE, tdata0);//define the texture using image data
    stbi_image_free(tdata0);//don’t forget to release the image data
    
    
    unsigned int VAO0;
    glGenVertexArrays(1, &VAO0);
    
    GLuint position_VBO0;
    glGenBuffers(1, &position_VBO0);
    glBindBuffer(GL_ARRAY_BUFFER, position_VBO0);
    glBufferData(GL_ARRAY_BUFFER, vbuffer0.size() * sizeof(float), &vbuffer0[0], GL_STATIC_DRAW);

    GLuint normal_VBO0;
    glGenBuffers(1, &normal_VBO0);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO0);
    glBufferData(GL_ARRAY_BUFFER, nbuffer0.size() * sizeof(float), &nbuffer0[0], GL_STATIC_DRAW);
    
    GLuint texcoord_VBO0;
    glGenBuffers(1, &texcoord_VBO0);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_VBO0);
    glBufferData(GL_ARRAY_BUFFER, tbuffer0.size() * sizeof(float), &tbuffer0[0], GL_STATIC_DRAW);
    
    glBindVertexArray(VAO0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, position_VBO0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_VBO0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    
    std::vector<float> vbuffer1;
    std::vector<float> nbuffer1;
    std::vector<float> tbuffer1;
    
    char filename1[] = "/Users/wangst/Documents/McMaster/2022 Fall/CS3GC3/assignment3/asset/floor.obj";
    tinyobj::attrib_t attrib1;
    std::vector<tinyobj::shape_t> shapes1;
    std::vector<tinyobj::material_t> materials1;
    // tinyobj load obj
    std::string warn1, err1;
    bool bTriangulate1 = true;
    bool bSuc1 = tinyobj::LoadObj(&attrib1, &shapes1, &materials1, &warn1, &err1, filename1, nullptr, bTriangulate1);
    for (auto id: shapes1[0].mesh.indices){
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        
        vbuffer1.push_back(attrib1.vertices[vid*3]);
        vbuffer1.push_back(attrib1.vertices[vid*3+1]);
        vbuffer1.push_back(attrib1.vertices[vid*3+2]);
        nbuffer1.push_back(attrib1.normals[nid*3]);
        nbuffer1.push_back(attrib1.normals[nid*3+1]);
        nbuffer1.push_back(attrib1.normals[nid*3+2]);
        tbuffer1.push_back(attrib1.texcoords[tid*2]);
        tbuffer1.push_back(attrib1.texcoords[tid*2+1]);
    }
    
    int x1, y1, n1;
    char tex_path1[] = "/Users/wangst/Documents/McMaster/2022 Fall/CS3GC3/assignment3/asset/floor.jpeg";
    stbi_info(tex_path1, &x1, &y1, &n1);
    stbi_set_flip_vertically_on_load(true);
    unsigned char *tdata1 = stbi_load(tex_path1, &x1, &y1, &n1, 0);
    
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x0, y0, 0, GL_RGB,
    GL_UNSIGNED_BYTE, tdata1);//define the texture using image data
    stbi_image_free(tdata1);//don’t forget to release the image data
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    unsigned int VAO1;
    glGenVertexArrays(1, &VAO1);
    
    GLuint position_VBO1;
    glGenBuffers(1, &position_VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, position_VBO1);
    glBufferData(GL_ARRAY_BUFFER, vbuffer1.size() * sizeof(float), &vbuffer1[0], GL_STATIC_DRAW);

    GLuint normal_VBO1;
    glGenBuffers(1, &normal_VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO1);
    glBufferData(GL_ARRAY_BUFFER, nbuffer1.size() * sizeof(float), &nbuffer1[0], GL_STATIC_DRAW);
    
    GLuint texcoord_VBO1;
    glGenBuffers(1, &texcoord_VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_VBO1);
    glBufferData(GL_ARRAY_BUFFER, tbuffer1.size() * sizeof(float), &tbuffer1[0], GL_STATIC_DRAW);
    
    glBindVertexArray(VAO1);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, position_VBO1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, texcoord_VBO1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        
        //draw things
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        glBindTexture(GL_TEXTURE_2D, texture0);
        glBindVertexArray(VAO0);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer0.size());
        
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, vbuffer1.size());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //release resource
    glDeleteVertexArrays(1, &VAO0);
    glDeleteBuffers(1, &position_VBO0);
    glDeleteBuffers(1, &normal_VBO0);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
