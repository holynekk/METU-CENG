#include "EclipseMap.h"

using namespace std;

glm::mat4 M_model_light;
glm::mat4 MVP, M_model, M_view, M_projection, moon_MVP, moon_M_model(1);

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

    // Load shaders and textures
    GLuint moonShaderID = initShaders("moonShader.vert", "moonShader.frag");
    initMoonColoredTexture(moonTexturePath, moonShaderID);

    // Temporary variables to set vertices and indices data 
    float x, y, z, xy, u, v, alpha, beta;
    int k1, k2;

    // Set moonVertices ----------------------------------------------------------------------
	for (int i = 0; i <= verticalSplitCount; i++) {
		beta = (PI / 2) - (i * PI / verticalSplitCount);
		xy = moonRadius * cosf(beta);
		z = moonRadius * sinf(beta);
		for (int j = 0; j <= horizontalSplitCount; j++) {
			alpha = j * 2 * PI / horizontalSplitCount;
			x = xy * cosf(alpha);
			y = xy * sinf(alpha);
			u = (float)j / horizontalSplitCount;
			v = (float)i / verticalSplitCount;

			vertex vertex(
                glm::vec3(x, y + 3200, z),
                glm::normalize(glm::vec3(x/moonRadius, y/moonRadius, z/moonRadius)),
                glm::vec2(u, v)
            );
			moonVertices.push_back(vertex);
		}
	}

    // Set moon indices
	for(int i = 0; i < verticalSplitCount; ++i) {
		k1 = i * (horizontalSplitCount + 1);
		k2 = k1 + horizontalSplitCount + 1;
		for(int j = 0; j < horizontalSplitCount; ++k1, ++k2, ++j) {
			if(i != 0) {
				moonIndices.push_back(k1);
				moonIndices.push_back(k2);
				moonIndices.push_back(k1 + 1);
			}
			if(i != (horizontalSplitCount - 1)) {
				moonIndices.push_back(k1 + 1);
				moonIndices.push_back(k2);
				moonIndices.push_back(k2 + 1);
			}
		}
	}
    
    // Configure moon buffers ----------------------------------------------------------------------

    // Create buffers
    glGenVertexArrays(1, &moonVAO);
    glGenBuffers(1, &moonVBO);
    glGenBuffers(1, &moonEBO);

	// Bind buffers
	glBindVertexArray(moonVAO);
	glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moonEBO);

    // Set buffer data
	glBufferData(GL_ARRAY_BUFFER, moonVertices.size() * sizeof(vertex), moonVertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, moonIndices.size() * sizeof(int), moonIndices.data(), GL_STATIC_DRAW);

	// Assign attibutes to buffers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (offsetof(vertex, texture)));

    // Enable buffers
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


    // Load world shaders and textures
    GLuint worldShaderID = initShaders("worldShader.vert", "worldShader.frag");
    initColoredTexture(coloredTexturePath, worldShaderID);
    initGreyTexture(greyTexturePath, worldShaderID);

    // Set worldVertices ----------------------------------------------------------------------
	for (int i = 0; i <= verticalSplitCount; i++) {
		beta = PI / 2 - (i * PI / verticalSplitCount); // pi/2 to -pi/2
		xy = radius * cosf(beta);
		z = radius * sinf(beta);
		for (int j = 0; j <= horizontalSplitCount; j++) {
			alpha = j * 2 * PI / horizontalSplitCount;  // 0 to 2pi
			x = xy * cosf(alpha);
			y = xy * sinf(alpha);
			u = (float)j / horizontalSplitCount;
			v = (float)i / verticalSplitCount;

			vertex vertex(
                glm::vec3(1.5*x, 1.5*y, 1.5*z),
                glm::normalize(glm::vec3(x/radius, y/radius, z/radius)),
                glm::vec2(u, v)
            );
			worldVertices.push_back(vertex);
		}
	}

	for(int i = 0; i < verticalSplitCount; ++i) {
		k1 = i * (horizontalSplitCount + 1);
		k2 = k1 + horizontalSplitCount + 1;
		for(int j = 0; j < horizontalSplitCount; ++j, ++k1, ++k2) {
			if(i != 0) {
				worldIndices.push_back(k1);
				worldIndices.push_back(k2);
				worldIndices.push_back(k1 + 1);
			}
			if(i != (horizontalSplitCount - 1)) {
				worldIndices.push_back(k1 + 1);
				worldIndices.push_back(k2);
				worldIndices.push_back(k2 + 1);
			}
		}
	}
    
    // Configure world buffers ----------------------------------------------------------------------

    // Create buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
	

	// Bind buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// Set buffer data
	glBufferData(GL_ARRAY_BUFFER, worldVertices.size() * sizeof(vertex), worldVertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, worldIndices.size() * sizeof(int), worldIndices.data(), GL_STATIC_DRAW);

	// Assign pointers to those buffers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) offsetof(vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (offsetof(vertex, texture)));

    // Enable Buffers
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

        // Handle key presses ----------------------------------------------------------------------
        handleKeyPress(window);

        // TODO: Manipulate rotation variables ----------------------------------------------------------------------
        float rotation_speed = 3.0/horizontalSplitCount;
        M_model_light = glm::mat4(1.0f);
        M_model_light = glm::rotate(M_model_light, -rotation_speed, glm::vec3(0, 0, 1));
        lightPos = M_model_light * glm::vec4(lightPos, 1.0f);
        // Bind all textures

        // Texture for colored earth
        glActiveTexture(GL_TEXTURE0 + 0); 
        glBindTexture(GL_TEXTURE_2D, textureColor);

        // Texture for earth's heightmap
        glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
        glBindTexture(GL_TEXTURE_2D, textureGrey);

        // Texture for colored moon
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, moonTextureColor);

        /************* WORLD *************/

        // Use worldShaderID program ----------------------------------------------------------------------
        glUseProgram(worldShaderID);
        
        // Update camera at every frame ----------------------------------------------------------------------
        cameraPosition += speed * cameraDirection;
        M_model = glm::rotate(M_model, rotation_speed, glm::vec3(0, 0, 1));
        M_view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
        M_projection = glm::perspective(projectionAngle, aspectRatio, near, far);
        MVP = M_projection * M_view * M_model;

        // Update uniform variables at every frame ----------------------------------------------------------------------
        glUniform3fv(glGetUniformLocation(worldShaderID, "lightPosition"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(worldShaderID, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	    glUniformMatrix4fv(glGetUniformLocation(worldShaderID, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	    glUniform1f(glGetUniformLocation(worldShaderID, "textureOffset"), textureOffset);
	    glUniform1f(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);


        // Bind world vertex array ----------------------------------------------------------------------
        glBindVertexArray(VAO);
	
        // Draw world object ----------------------------------------------------------------------
        glDrawElements(GL_TRIANGLES, worldIndices.size(), GL_UNSIGNED_INT, nullptr);

        /************* WORLD *************/

        /************* MOON *************/
        
        // Use moonShaderID program ----------------------------------------------------------------------
        glUseProgram(moonShaderID);
        // TODO: Update camera at every frame ----------------------------------------------------------------------
        float bumbum = rotation_speed * (-1.5);
        moon_M_model = glm::rotate(moon_M_model, bumbum, glm::vec3(0, 0, 1));
        M_view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
        M_projection = glm::perspective(projectionAngle, aspectRatio, near, far);
        moon_MVP = M_projection * M_view * moon_M_model;
        // Update uniform variables at every frame ----------------------------------------------------------------------
        glUniform3fv(glGetUniformLocation(moonShaderID, "lightPosition"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(moonShaderID, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	    glUniformMatrix4fv(glGetUniformLocation(moonShaderID, "MVP"), 1, GL_FALSE, glm::value_ptr(moon_MVP));
	    glUniform1f(glGetUniformLocation(moonShaderID, "textureOffset"), textureOffset);
	    glUniform1f(glGetUniformLocation(moonShaderID, "heightFactor"), heightFactor);
        // Bind moon vertex array ----------------------------------------------------------------------
        glBindVertexArray(moonVAO);

        // Draw moon object ----------------------------------------------------------------------
        glDrawElements(GL_TRIANGLES, moonIndices.size(), GL_UNSIGNED_INT, nullptr);
        /************* MOON *************/
        
        // Update scene for every frame
        // Cam speed to zero
        if (stop_cam) {
            speed = 0.0;
        }
        // Change heightFactor
        if (increase_heightFactor) {
            heightFactor += 10;
            glUniform1f(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);
        } if (decrease_heightFactor) {
            heightFactor -= 10;
            glUniform1f(glGetUniformLocation(worldShaderID, "heightFactor"), heightFactor);
        }
        // Change camera speed
        if (increase_speed) {
            speed += 0.01;
        } if (decrease_speed) {
            speed -= 0.01;
        }
        // Change camera pitch
        if (increase_pitch) {
            float aa = -0.05;
            cameraUp = glm::rotate(cameraUp, -0.05f, cameraLeft);
		    cameraDirection = glm::rotate(cameraDirection, -0.05f, cameraLeft);
        } if (decrease_pitch) {
            cameraUp = glm::rotate(cameraUp, 0.05f, cameraLeft);
		    cameraDirection = glm::rotate(cameraDirection, 0.05f, cameraLeft);
        }
        // Change camera yaw
        if (increase_yaw) {
            cameraLeft = glm::rotate(cameraLeft, -0.05f, cameraUp);
		    cameraDirection = glm::rotate(cameraDirection, -0.05f, cameraUp);
        } if (decrease_yaw) {
            cameraLeft = glm::rotate(cameraLeft, 0.05f, cameraUp);
		    cameraDirection = glm::rotate(cameraDirection, 0.05f, cameraUp);
        }
        // Restart
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
            glfwSetWindowMonitor(window, nullptr, 0, 0, screenWidth, screenHeight, 0);
        } else {
            switch_full_screen = true;
            screenWidth = defaultScreenWidth;
            screenHeight = defaultScreenHeight;
            GLFWmonitor * primaryMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode * mode = glfwGetVideoMode(primaryMonitor);
            glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
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