#include "GL/glew.h" // include GLEW and new version of GL on Windows
#include "GL/glfw3.h" // GLFW helper library
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.hpp"
#include "mesh.hpp"

#include <iostream>

unsigned int material = 1;


int main(void) {
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    } 
    int width = 800; 
    int height = 800;

    GLFWwindow* window = glfwCreateWindow(width, height, "Textures and Lighting", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    // start GLEW extension handler
    glewInit();
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    // keyboard callback
    
    glClearColor(0.3,0.3,0.4,0.0);
    // Initialise and activate shaders
    // Using standard MVP vertex shader and color fragment shader
    shader shaderProgram("shaders/mvp_texture.vert","shaders/texture.frag");
    shaderProgram.use();
    glEnable(GL_DEPTH_TEST); //enable depth - testing to correctly render 3D stuff
    // load the cube
    
    meshvft texsphere("obj/spherevfnt.obj","textures/earth720x360.jpg");
    // view matrix 
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f,-8.0f,5.0f),glm::vec3(0.0f),glm::vec3(0.0f,0.0f,1.0f));
    shaderProgram.set_mat4_uniform("view",view);
    // perspective matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)width/height, 0.01f,100.0f);
    shaderProgram.set_mat4_uniform("projection",projection); 
	glm::vec3 lightPos = glm::vec3(2.0 , - 5.0 ,  2.0f);
    // convert light’s position to view space
    glm::vec3 lightPosV = glm::vec3(view * glm::vec4(lightPos, 1.0));
    // update light’s position in shader
    shaderProgram.set_vec3_uniform("light_position", lightPosV);
	
	  
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float t = glfwGetTime();
        
        shaderProgram.set_int_uniform("material", material);
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::degrees(0.01f*t), glm::vec3(0.0f, 0.0f, 1.0f));
        shaderProgram.set_mat4_uniform("model",model);
        glm::mat4 norm_matrix = glm::transpose(glm::inverse(view * model));
        shaderProgram.set_mat4_uniform("norm_matrix",norm_matrix);
		texsphere.draw(GL_TRIANGLES);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    } 
    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}


