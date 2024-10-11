#ifndef SHADER_HPP
#define SHADER_HPP

#include<GL/glew.h>
#include"glm/glm.hpp"
#include"glm/gtc/type_ptr.hpp"
#include<iostream>
#include<cstdio>
#include<fstream>
#include<string>

class shader
{
public:
    GLuint ID; //shader program ID
    shader(const char *vpath, const char *fpath) //constructor
    {
        //read the vertex shader source code from its file
        std::ifstream fpvertex(vpath);
        if (!fpvertex.is_open())
        {
            printf("'%s' not found. Exiting...\n",vpath);
            exit(EXIT_FAILURE);
        }
        std::string vtemp;
        vtemp.assign( (std::istreambuf_iterator<char>(fpvertex)), (std::istreambuf_iterator<char>()) );
        const char *vsource = vtemp.c_str();
        
        //compile the vertex shader and check for errors
        GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vshader, 1, &vsource, NULL);
        glCompileShader(vshader);
        int success;
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            printf("Error while compiling '%s'.\n",vpath);
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        //read the fragment shader source code from its file
        std::ifstream fpfragment(fpath);
        if (!fpfragment.is_open())
        {
            printf("'%s' not found. Exiting...\n",fpath);
            exit(EXIT_FAILURE);
        }
        std::string ftemp;
        ftemp.assign( (std::istreambuf_iterator<char>(fpfragment)), (std::istreambuf_iterator<char>()) );
        const char *fsource = ftemp.c_str();
        
        //compile the fragment shader and check for errors
        GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fshader, 1, &fsource, NULL);
        glCompileShader(fshader);
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            printf("Error while compiling '%s'.\n",fpath);  
        }
        
        ////////////////////////////////////////////////////////////////////////
        
        // linking
        ID = glCreateProgram();
        glAttachShader(ID, vshader);
        glAttachShader(ID, fshader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            printf("Error while linking shader program ('%s' || '%s').\n",vpath,fpath);
        }
        
        glDeleteShader(vshader);
        glDeleteShader(fshader);
    }
    
    //activate the current shader
    void use()
    {
        glUseProgram(ID);
    }
    
    //pass to the currently active shader 1 int (uniform)
    void set_int_uniform(const std::string &name, int value)
    {
        unsigned location = glGetUniformLocation(ID, name.c_str());
        glUniform1i(location, value);
    }

    //pass to the currently active shader 1 float (uniform)
    void set_float_uniform(const std::string &name, float value)
    {
        GLuint location = glGetUniformLocation(ID, name.c_str());
        glUniform1f(location, value);
    }
    //pass to the currently active shader 1 vector of 3 floats (uniform)
    void set_vec2_uniform(const std::string &name, glm::vec2 &v)
    {
        unsigned location = glGetUniformLocation(ID, name.c_str());
        glUniform2fv(location, 1, &v[0]);
    }
    
    
   //pass to the currently active shader 1 vector of 3 floats (uniform)
    void set_vec3_uniform(const std::string &name, glm::vec3 &v)
    {
        unsigned location = glGetUniformLocation(ID, name.c_str());
        glUniform3fv(location, 1, &v[0]);
    }
    
    //pass to the currently active shader 1 vector of 4 floats (uniform)
    void set_vec4_uniform(const std::string &name, glm::vec4 &v)
    {
        unsigned location = glGetUniformLocation(ID, name.c_str());
        glUniform4fv(location, 1, &v[0]);
    }

    //pass to the currently active shader 1 4x4 float matrix (uniform)
    void set_mat4_uniform(const std::string &name, glm::mat4 &m)
    {
        GLuint location = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }
};

#endif
