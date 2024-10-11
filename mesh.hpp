#ifndef MESH_HPP
#define MESH_HPP

#include<GL/glew.h>
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

class meshvc
{
public:
    GLuint vao, vbo1, vbo2; //vertex array and buffer object
    GLuint draw_size;

    meshvc(const char *objpath)
    {   
        std::ifstream fp;
        fp.open(objpath);
        if (!fp.is_open())
        {
            printf("'%s' not found. Exiting...\n", objpath);
            exit(EXIT_FAILURE);
        }

        //vertices (format : v x y z)
        std::vector<float> verts;
        float x,y,z;

        //colors (format : c r g b)
        std::vector<float> colors;
        float r,g,b;

        std::string line;
        while (getline(fp, line))
        {
            if (line[0] == 'v' && line[1] == ' ') //then we have a vertex line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "v %f %f %f", &x,&y,&z);
                verts.push_back(x);
                verts.push_back(y);
                verts.push_back(z);
            }
            else if (line[0] == 'c') //then we have an color line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "c %f %f %f", &r,&g,&b);
                colors.push_back(r);
                colors.push_back(g);
                colors.push_back(b);
            }
        }

        draw_size = verts.size();
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // Create and bind the VBOs
        // Position VBO
        
        glGenBuffers(1, &vbo1);
        glBindBuffer(GL_ARRAY_BUFFER, vbo1);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), &verts[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Color VBO
        glGenBuffers(1, &vbo2);
        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(float), &colors[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0); 
    }

    //delete the mesh
    ~meshvc()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo1);
        glDeleteBuffers(1, &vbo2);
    }

    void draw(GLenum mode)
    {
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, draw_size);
        glBindVertexArray(0);
    }
};

class meshvf
{
public:
    unsigned int vao, vbo; //vertex array and buffer object
    std::vector<float> main_buffer; //final form of the geometry data to draw
    unsigned int draw_size;

    meshvf(const char *objpath)
    {
        std::ifstream fp;
        fp.open(objpath);
        if (!fp.is_open())
        {
            printf("'%s' not found. Exiting...\n", objpath);
            exit(EXIT_FAILURE);
        }

        //vertices (format : v x y z)
        std::vector<std::vector<float>> verts;
        float x,y,z;

        //indices (format : f i1 i2 i3)
        std::vector<std::vector<unsigned int>> inds;
        unsigned int i1,i2,i3;

        std::string line;
        while (getline(fp, line))
        {
            if (line[0] == 'v' && line[1] == ' ') //then we have a vertex line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "v %f %f %f", &x,&y,&z);
                verts.push_back({x,y,z});
            }
            else if (line[0] == 'f') //then we have an indices line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "f %d %d %d", &i1,&i2,&i3);
                inds.push_back({i1-1, i2-1, i3-1});
            }
        }

        //Now combine verts[][] and inds[][] to construct the main_buffer[]
        //which will have all the data needed for drawing.
        for (int i = 0; i < inds.size(); ++i)
        {
            main_buffer.push_back( verts[ inds[i][0] ][0] );
            main_buffer.push_back( verts[ inds[i][0] ][1] );
            main_buffer.push_back( verts[ inds[i][0] ][2] );

            main_buffer.push_back( verts[ inds[i][1] ][0] );
            main_buffer.push_back( verts[ inds[i][1] ][1] );
            main_buffer.push_back( verts[ inds[i][1] ][2] );

            main_buffer.push_back( verts[ inds[i][2] ][0] );
            main_buffer.push_back( verts[ inds[i][2] ][1] );
            main_buffer.push_back( verts[ inds[i][2] ][2] );
        }
        //Now main_buffer[] has the form : {x1,y1,z1, x2,y2,z2, x3,y3,z3, ... }, where
        //consecutive triads of vertices form triangles-faces of the object to be rendered

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, main_buffer.size()*sizeof(float), &main_buffer[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        draw_size = (int)(main_buffer.size()/3); //to draw only faces
    }

    //delete the mesh
    ~meshvf()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    //draw the mesh (triangles)
    void draw(GLenum mode)
    {
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, draw_size);
        glBindVertexArray(0);
    }

    void wireframe(void)
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glLineWidth(2.0f);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, draw_size);
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
};

class meshvft{
public:
    unsigned int vao, vbo, vbo2,tao; //vertex array and buffer object
    std::vector<float> vertex_buffer; //final form of the geometry data to draw
    std::vector<float> normals_buffer; //final form of the geometry data to draw
    unsigned int draw_size;

   meshvft(const char *objpath, const char *imgpath)
	{
    	std::ifstream fp;
    	fp.open(objpath);
    	if (!fp.is_open())
      	{
            printf("'%s' not found. Exiting...\n", objpath);
            exit(EXIT_FAILURE);
       }

        //vertices (format : v x y z)
	    std::vector<std::vector<float>> verts;
	    std::vector<std::vector<float>> normals;
	    float x,y,z;
	    float vx,vy,vz;
	
	    //indices (format : f i1 i2 i3)
	    std::vector<std::vector<unsigned int>> inds;
	  //  std::vector<std::vector<unsigned int>> tinds;
	   // std::vector<std::vector<unsigned int>> ninds;
	    std::vector<std::vector<bool>> fcorrector;
        unsigned int i1,i2,i3,i4,i44;
        unsigned int ni1,ni2,ni3,ni4;

        std::string line;
        
     
        //Now main_buffer[] has the form : {x1,y1,z1, x2,y2,z2, x3,y3,z3, ... }, where
        //consecutive triads of vertices form triangles-faces of the object to be rendered
        

       
      

        //texture (format : vt tx ty)
        std::vector<std::vector<float>> texs;
        float tx,ty;
        std::vector<float> main_buffer;
          
        unsigned int i11,i22,i33;
        if (!fp.is_open())
        {
            printf("'%s' not found. Exiting...\n", objpath);
            exit(EXIT_FAILURE);
        }
        while (getline(fp, line)){
        	int n=0;
        	if (line[0] == 'v' && line[1] == ' ') //then we have a vertex line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "v %f %f %f", &x,&y,&z);
                verts.push_back({x,y,z});
            }
            else if (line[0] == 'v' && line[1] == 't') //then we have a texture line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "vt %f %f", &tx,&ty);
                texs.push_back({tx,ty});
            }
            
            else if (line[0] == 'f') //then we have an indices line
            {
                const char *tmp_line = line.c_str();
                  for(int j=0; j<line.size(); j++) {
		            if (line[j] == ' ') n++;}
		            //printf("%d\n",n);
		            if(n==4){       
               			 sscanf(tmp_line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &i1,&i11,&ni1,&i2,&i22,&ni2,&i3,&i33,&ni3,&i4,&i44,&ni4);
               			 inds.push_back({i1-1,i11-1,ni1-1, i2-1,i22-1,ni2-1, i3-1,i33-1,ni3-1});
               			inds.push_back({ i3-1,i33-1,ni3-1,i4-1,i44-1,ni4-1,i1-1,i11-1,ni1-1});
               			 
              		
					}else{ 
						sscanf(tmp_line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &i1,&i11,&ni1,&i2,&i22,&ni2,&i3,&i33,&ni3);
                		inds.push_back({i1-1,i11-1,ni1-1, i2-1,i22-1,ni2-1, i3-1,i33-1,ni3-1});
                		
                        }
                    
            }
            else if (line[0] == 'v' && line[1] == 'n') //then we have a vertex line
            {
                const char *tmp_line = line.c_str();
                sscanf(tmp_line, "vn %f %f %f", &vx,&vy,&vz);
                normals.push_back({vx,vy,vz});
            }
		}
            
		int i;
        //Now combine verts[][], texs[][] and inds[][] to construct the main buffer main_buffer[]
        //which will have all the main_buffer needed for drawing.
       for (i = 0; i < inds.size(); ++i)
        {
            main_buffer.push_back( verts[ inds[i][0] ][0] );
            main_buffer.push_back( verts[ inds[i][0] ][1] );
            main_buffer.push_back( verts[ inds[i][0] ][2] );
            main_buffer.push_back(  texs[ inds[i][1] ][0] );
            main_buffer.push_back(  texs[ inds[i][1] ][1] );
            main_buffer.push_back( normals[ inds[i][2] ][0] );
            main_buffer.push_back( normals[ inds[i][2] ][1] );
            main_buffer.push_back( normals[ inds[i][2] ][2] );
            

            main_buffer.push_back( verts[ inds[i][3] ][0] );
            main_buffer.push_back( verts[ inds[i][3] ][1] );
            main_buffer.push_back( verts[ inds[i][3] ][2] );
            main_buffer.push_back(  texs[ inds[i][4] ][0] );
            main_buffer.push_back(  texs[ inds[i][4] ][1] );
            main_buffer.push_back( normals[ inds[i][5] ][0] );
            main_buffer.push_back( normals[ inds[i][5] ][1] );
            main_buffer.push_back( normals[ inds[i][5] ][2] );
       

            main_buffer.push_back( verts[ inds[i][6] ][0] );
            main_buffer.push_back( verts[ inds[i][6] ][1] );
            main_buffer.push_back( verts[ inds[i][6] ][2] );
            main_buffer.push_back(  texs[ inds[i][7] ][0] );
            main_buffer.push_back(  texs[ inds[i][7] ][1] );
            main_buffer.push_back( normals[ inds[i][8] ][0] );
            main_buffer.push_back( normals[ inds[i][8] ][1] );
            main_buffer.push_back( normals[ inds[i][8] ][2] );
            
      
        }
        
	
     
       
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);       
        glBufferData(GL_ARRAY_BUFFER, main_buffer.size()*sizeof(float), &main_buffer[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        	
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1); 
		draw_size = (int)(main_buffer.size()/5); //to draw faces and textures       
        
      
        
		// Normals VBO
        glGenBuffers(1, &vbo2);
        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glBufferData(GL_ARRAY_BUFFER, main_buffer.size()*sizeof(float), &main_buffer[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
        glEnableVertexAttribArray(2);
        
        
        
        //load the image-texture
        int imgwidth, imgheight, imgchannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *imgdata = stbi_load(imgpath, &imgwidth, &imgheight, &imgchannels, 0);
        if (!imgdata)
        {
            printf("'%s' not found. Exiting...\n", imgpath);
            exit(EXIT_FAILURE);
        }

        // load and create a texture 
        // -------------------------
        glGenTextures(1, &tao);
        glBindTexture(GL_TEXTURE_2D, tao);
        // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // create texture and generate mipmaps
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgdata);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(imgdata); //free image resources
        
    }

    //delete the mesh
    ~meshvft()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &vbo2);
        glDeleteTextures(1, &tao);
    }

    //draw the mesh (triangles)
    void draw(GLenum mode)
    {
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, tao);
        glDrawArrays(mode, 0, draw_size);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

};

#endif
