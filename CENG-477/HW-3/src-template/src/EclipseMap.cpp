#include "EclipseMap.h"

using namespace std;

glm::mat4 MVP, M_model, M_view, M_projection;

bool increase_heightFactor = false, 
    decrease_heightFactor = false, 
    increase_pitch = false, 
    decrease_pitch = false, 
    increase_yaw = false, 
    decrease_yaw = false,
    increase_speed = false, 
    decrease_speed = false,
    stop_cam = false,
    return_initial = false, 
    switch_full_screen = false;



void EclipseMap::Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath) {
    // Open window
    GLFWwindow *window = openWindow(windowName, screenWidth, screenHeight);

    // Moon commands
    // Load shaders
    GLuint moonShaderID = initShaders("moonShader.vert", "moonShader.frag");

    initMoonColoredTexture(moonTexturePath, moonShaderID);

    
    // TODO: Set moonVertices ----------------------------------------------------------------------
    
    // TODO: Configure Buffers ----------------------------------------------------------------------
    

    // World commands
    // Load shaders
    GLuint worldShaderID = initShaders("worldShader.vert", "worldShader.frag");

    initColoredTexture(coloredTexturePath, worldShaderID);
    initGreyTexture(greyTexturePath, worldShaderID);

    // TODO: Set worldVertices ----------------------------------------------------------------------
    float x, y, z, xy, u, v, alpha, beta;
	for (int i = 0; i <= verticalSplitCount; i++) {
		beta = M_PI / 2 - (i * M_PI / verticalSplitCount); // pi/2 to -pi/2
		xy = radius * cosf(beta);
		z = radius * sinf(beta);
		for (int j = 0; j <= horizontalSplitCount; j++) {
			alpha = j * 2 * M_PI / horizontalSplitCount;  // 0 to 2pi
			x = xy * cosf(alpha);
			y = xy * sinf(alpha);
			u = (float)j / horizontalSplitCount;
			v = (float)i / verticalSplitCount;

			vertex vertex;
			vertex.position = glm::vec3(x, y, z);
			vertex.normal = glm::normalize(glm::vec3(x/radius, y/radius, z/radius));
			vertex.texture = glm::vec2(u, v);

			worldVertices.push_back(vertex);
		}
	}

    int k1, k2;
	for(int i = 0; i < verticalSplitCount; ++i) {
		k1 = i * (horizontalSplitCount + 1);
		k2 = k1 + horizontalSplitCount + 1;
		for(int j = 0; j < horizontalSplitCount; ++j, ++k1, ++k2) {
			// 2 triangles per sector excluding first and last stacks
			/* Provide indices for the first triangle with a correct winding order that suits RH-rule */
			if(i != 0) {
				worldIndices.push_back(k1);
				worldIndices.push_back(k2);
				worldIndices.push_back(k1 + 1);
			}
			/* Provide indices for the second triangle with a correct winding order that suits RH-rule */
			if(i != (horizontalSplitCount - 1)) {
				worldIndices.push_back(k1 + 1);
				worldIndices.push_back(k2);
				worldIndices.push_back(k2 + 1);
			}
		}
	}
    
    // TODO: Configure Buffers ----------------------------------------------------------------------

    glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/* Init VBOs */
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/* Construct Terrain primitive data on CPU memory and pass it onto GPU memory */
	glBufferData(GL_ARRAY_BUFFER, worldVertices.size() * sizeof(vertex), worldVertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	/* Construct Terrain primitives index data on CPU memory and pass it onto GPU memory */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, worldIndices.size() * sizeof(int), worldIndices.data(), GL_STATIC_DRAW);

	/* glVertexAttribPointer(array_index, #_of_coods_per_Vertex, type, need_normalization?,
	 * Byte_offset_between_consecutive_vertices, offset_of_fields_in_vertex_structure) */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (offsetof(vertex, texture)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    do {
        glViewport(0, 0, screenWidth, screenHeight);

        glClearStencil(0);
        glClearDepth(1.0f);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);



        // TODO: Handle key presses ----------------------------------------------------------------------
        handleKeyPress(window);

        // TODO: Manipulate rotation variables ----------------------------------------------------------------------

        // TODO: Bind textures
        glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, textureColor);

        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, textureGrey);

        /************* WORLD *************/

        // TODO: Use worldShaderID program ----------------------------------------------------------------------
        glUseProgram(worldShaderID);
        
        // TODO: Update camera at every frame ----------------------------------------------------------------------
        cameraPosition += speed * cameraDirection;
        float rotation_speed = 0.5/horizontalSplitCount;
        M_model = glm::rotate(M_model, rotation_speed, glm::vec3(0, 0, 1));
        M_view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
        M_projection = glm::perspective(projectionAngle, aspectRatio, near, far);
        MVP = M_projection * M_view * M_model;

        // TODO: Update uniform variables at every frame ----------------------------------------------------------------------
        glUniform3fv(glGetUniformLocation(worldShaderID, "lightPosition"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(worldShaderID, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	    glUniformMatrix4fv(glGetUniformLocation(worldShaderID, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	    glUniform1f(glGetUniformLocation(worldShaderID, "textureOffset"), textureOffset);
	    glUniform1f(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);


        // TODO: Bind world vertex array ----------------------------------------------------------------------
        // Did it before getting into main loop
	
        // TODO: Draw world object ----------------------------------------------------------------------
        glDrawElements(GL_TRIANGLES, worldIndices.size(), GL_UNSIGNED_INT, nullptr);
        /************* WORLD *************/




        /************* MOON *************/
        
        // TODO: Use moonShaderID program ----------------------------------------------------------------------
        
        // TODO: Update camera at every frame ----------------------------------------------------------------------
        
        // TODO: Update uniform variables at every frame ----------------------------------------------------------------------
        
        // TODO: Bind moon vertex array ----------------------------------------------------------------------

        // TODO: Draw moon object ----------------------------------------------------------------------
        
        /************* MOON *************/
        

        if (stop_cam) {
            speed = 0.0;
        }
        if (increase_heightFactor) {
            heightFactor += 10;
            glUniform1f(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);
        }
        if (decrease_heightFactor) {
            heightFactor -= 10;
            glUniform1f(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);
        }
        if (increase_speed) {
            speed += 0.01;
        } if (decrease_speed) {
            speed -= 0.01;
        }
        if (increase_pitch) {
            float aa = -0.05;
            cameraUp = glm::rotate(cameraUp, -0.05f, cameraLeft);
		    cameraDirection = glm::rotate(cameraDirection, -0.05f, cameraLeft);
        } if (decrease_pitch) {
            cameraUp = glm::rotate(cameraUp, 0.05f, cameraLeft);
		    cameraDirection = glm::rotate(cameraDirection, 0.05f, cameraLeft);
        }
        if (increase_yaw) {
            cameraLeft = glm::rotate(cameraLeft, -0.05f, cameraUp);
		    cameraDirection = glm::rotate(cameraDirection, -0.05f, cameraUp);
        } if (decrease_yaw) {
            cameraLeft = glm::rotate(cameraLeft, 0.05f, cameraUp);
		    cameraDirection = glm::rotate(cameraDirection, 0.05f, cameraUp);
        }
        if (return_initial) {
            cameraPosition = glm::vec3(0, 4000, 4000);
            cameraDirection = glm::vec3(0, -1, -1);
            cameraUp = glm::vec3(0, 0, 1);
            cameraLeft = glm::cross(cameraUp, cameraDirection);
            textureOffset = 0;
            glUniform1i(glGetUniformLocation(worldShaderID, "textureOffset"), textureOffset);
            speed = 0;
            heightFactor = 0.0;
            glUniform1i(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);
            
        }
		
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));

    // Delete buffers
    glDeleteBuffers(1, &moonVAO);
    glDeleteBuffers(1, &moonVBO);
    glDeleteBuffers(1, &moonEBO);

    
    // Delete buffers
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
   
    glDeleteProgram(moonShaderID);
    glDeleteProgram(worldShaderID);

    // Close window
    glfwTerminate();
}

void EclipseMap::handleKeyPress(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // Increase heightFactor
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        increase_heightFactor = true;
    }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        increase_heightFactor = false;
    }
    // Decrease heightFactor
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        decrease_heightFactor = true;
    }
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        decrease_heightFactor = false;
    }
    // Increase pitch
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        increase_pitch = true;
    }
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        increase_pitch = false;
    }
    // Decrease pitch
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        decrease_pitch = true;
    }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        decrease_pitch = false;
    }
    // Increase yaw
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        increase_yaw = true;
    }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        increase_yaw = false;
    }
    // Decrease yaw
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        decrease_yaw = true;
    }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        decrease_yaw = false;
    }
    // Increase speed
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        increase_speed = true;
    }
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
        increase_speed = false;
    }
    // Decrease speed
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        decrease_speed = true;
    }
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
        decrease_speed = false;
    }	
    // Stop camera (speed = 0)
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        stop_cam = true;
    }
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) {
        stop_cam = false;
    }
    // Return back to initial cam position
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        return_initial = true;
    }
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
        return_initial = false;
    }
    // Full screen mode on/off
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (switch_full_screen) {
            switch_full_screen = false;
            screenWidth = 1000;
            screenHeight = 1000;
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, nullptr, 0, 0, screenWidth, screenHeight, mode->refreshRate);
        } else {
            switch_full_screen = true;
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }

}

GLFWwindow *EclipseMap::openWindow(const char *windowName, int width, int height) {
    if (!glfwInit()) {
        getchar();
        return 0;
    }

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    glfwSetWindowMonitor(window, NULL, 1, 31, screenWidth, screenHeight, mode->refreshRate);

    if (window == NULL) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0, 0, 0, 0);

    return window;
}


void EclipseMap::initColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &textureColor);
    cout << shader << endl;
    glBindTexture(GL_TEXTURE_2D, textureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
   

    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexColor"), 0);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initGreyTexture(const char *filename, GLuint shader) {

    glGenTextures(1, &textureGrey);
    glBindTexture(GL_TEXTURE_2D, textureGrey);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height;

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
  



    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexGrey"), 1);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initMoonColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &moonTextureColor);
    cout << shader << endl;
    glBindTexture(GL_TEXTURE_2D, moonTextureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);
   

    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "MoonTexColor"), 2);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}