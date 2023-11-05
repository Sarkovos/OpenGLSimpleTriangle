//*******Code from learnOpenGL.com and Miguel Kunkle (2023)*********

//Yo. These are notes for how OpenGL runs for reference. Titles of sections are capitalized.

//OPENGL AS A STATE MACHINE
//***********************************************************
/*
  OpenGL is designed as a state machine for rendering graphics. The OpenGL state machine has various states that control how it processes vertices, fragments (pixels), textures, shaders, and other aspects of rendering. Some key points about OpenGL's state machine:

  Global State:
  OpenGL maintains a global state, which represents the current configuration of the graphics pipeline. This global state includes things like the currently bound shader program, the current viewport, the active textures, and so on.

  State Changes:
  When you make OpenGL calls, you are essentially changing the state of the OpenGL context. For example, binding a VBO changes the state related to vertex data, and setting a shader program changes the state related to shaders.

  Immutable Objects:
  Some OpenGL objects, once created, are considered immutable. This means their state cannot be changed directly. Instead, you often create a new object with the desired state. For example, when you create a texture, you set its parameters, and if you want to change those parameters, you typically create a new texture.

  Commands and Queries:
  OpenGL provides commands to change the state and queries to retrieve the current state. Commands are actions you take to change the state (like glBindBuffer or glUseProgram), and queries allow you to retrieve information about the current state (like glGet functions).
*/

/*UNDERSTANDING OPENGL OBJECTS
************************************************************
* To create an object, you generate an object's name (an integer), which creates a reference to the object.
* The function to generate object names is:
* glGen* (GLsizei n, GLuint *objects);
* where * is the object's type in plural form. This function generates n objects, storing them
* in an array given by the objects parameter.
*
* Example: 
* Gluint vbo;
* glGenBuffers(1, &vbo);

* Essentially, you are creating a variable to store the ID, and then glGen* creates the ID as well as reserves memory and creates an internal data structure for certain functionalities.
* The ID is simply to reference and manipulate the object, like a key to the object.
* bjects in OpenGL are much more abstract, and are not to be confused with OOP objects that come from classes like in C.
* Binding is the next step. Using the ID you created, you bind that ID to a target, also called a binding point. There are multiple targets in OpenGL, used for different purposes. For instance,
* for our vbo object, we can bind it to the GL_ARRAY_BUFFER so that if any other OpenGL function works with the GL_ARRAY_BUFFER, those functions will work with vbo. In this way, you can switch what object is being binded so you can switch which data you are working with.
* 
* Next we need to allocate and fill a buffer object with data by copying it to the target where space in the GPU will be allocated for it
* void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
* target: same target as with the binding where data is stored. size: size in bytes of the data you want to allocate data: pointer to the data that you want to copy to the buffer. usage: specifies expected usage of the data, chosen from list of enums.
* 
* To connect the attributes specified in our vertex shader to the vertices that are currently stored in our VBO, we have to use glVertexAttribPointer. This function takes the vertex shader attribute, and tells OpenGL what vertex data corresponds to that attribute.
* For instance, the simplest info we can provide is position. In our vertex shader we have "layout (location = 0) in vec3 aPos;". This specifies an attriubute with an index 0 in the vertex shader. Next, glVertexAttribPointer tells OpenGL that every 3 positions in the VBO
* is a set of position data (X,Y,Z in this case). If glVertexAttribPointer is tightly packed, you can just take incremental steps of 3 through the array to find each vertex.
* 
* This process works, but for multiple objects you would have to write a lot of lines of code.  To solve this, we use Vertex Array Objects. This object will take the data you made in your glVertexAttribPointer function and store it in one conventient place, almost like
* a template configuration. That way, if you plug in the right data from your vbo, it will automatically know how you want it formatted.
* NOTE: the VAO must be bound first, and all subsequence vertex attribute calls will be stored inside it. You can then unbind it and its state will be saved.
*/


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Set the size of the rendering window.
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    //First 2 parameters set the location of the lower left corner of the window
    //The 3rd and 4th set the width and height of the rendering window in pixels
    glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

//Settings
//----------------------------------------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//----------------------------------------


//Vertex and fragment shader code
//----------------------------------------
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";
//----------------------------------------

int main()
{
    // glfw: initialize and configure
    //----------------------------------------
    glfwInit(); //Initialize GLFW

    //We configure GLFW with glfwWindowHint
    //First argument says what to configure, second argument is value of our option. Possible options: https://www.glfw.org/docs/latest/window.html#window_hints
    //In this case, we are setting the OpenGL version to 3.3
    //The last line is setting GLFW to use the Core-profile of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //----------------------------------------

    
    
    // glfw window creation
    //----------------------------------------

    //first 2 arguments are width and height. The 3rd is the name of the window. Ignore the last 2 for now.
    //Returns a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL); 

    //Checks that window is created.
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); //Tell GLFW to make the context of our window the main context.
    // ----------------------------------------

    //Set the size of the rendering window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    //----------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //----------------------------------------


    // vertex shader
    // ------------------------------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates the variable for the name and sets it all in one line. glCreateShader creates a shader based on the enum parameter
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //links the shader code to our shader objects. 1st parameter: the shader object, 2nd: how many strings being passed in. 3rd: source code of shader. 4th: null
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // ------------------------------------


    // fragment shader
    // ------------------------------------
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//Creates the variable for the name and sets it all in one line. glCreateShader creates a shader based on the enum parameter
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//links the shader code to our shader objects. 1st parameter: the shader object, 2nd: how many strings being passed in. 3rd: source code of shader. 4th: null
    glCompileShader(fragmentShader);
   
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // ------------------------------------


    // link shaders togther
    // ------------------------------------

    //When linking shaders, the outputs of each shader is linked to the inputs of the next shader. Errors arise if inputs and outputs don't match.
    unsigned int shaderProgram = glCreateProgram(); //creates program object called shaderProgram
    glAttachShader(shaderProgram, vertexShader); //add shader
    glAttachShader(shaderProgram, fragmentShader); //add shader
    glLinkProgram(shaderProgram); //link shaders
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glUseProgram(shaderProgram); //Sets the program to use whatever is passed in. 

    //Shaders can be deleted after they are linked to program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // ------------------------------------


    // set up vertices for our triangle
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };
    // ------------------------------------------------------------------
 

    //Vertex buffer object (VBO)
    // ------------------------------------
    unsigned int VBO; //declares varible to store ID for our VBO
    glGenBuffers(1, &VBO); //creates the ID that is stored in VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //VBO binded to GL_ARRAY_BUFFER so that anything working the GL_ARRAY_BUFFER is now working with VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //memory is allocated on the GPU for data and vertex data is uploaded to VBO, since it is binded
    // ------------------------------------   

    //Vertex Array Objects
    // ------------------------------------
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); //same as before, create an ID for an object (the VAO here)
    glBindVertexArray(VAO); //Binds the VAO
    // ------------------------------------

    //glVertexAttribPointer
    // ------------------------------------
    ////1st parameter: index/location of vertex attribute. 2nd: size of the vertex attribute. 3rd: type of the data. 4th: bool for normalized data. 5th: stride, AKA space between consectutive vertex attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); //enables vertex attribute, they are disabled by default
    // ------------------------------------

    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbinds the VBO
    glBindVertexArray(0); //We unbind the VAO to not make any more changes to it. Optional, but good practice.

    //Render loop.
    while (!glfwWindowShouldClose(window))
    {
        // input
        // ------------------------------------
        processInput(window);
        // ------------------------------------

        //Background color
        // ------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // ------------------------------------
               
        // draw our first triangle
        // ------------------------------------
        glUseProgram(shaderProgram); //specificy which program to use
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3); //OpenGL functions based on the first parameter which is an enum.
        // ------------------------------------
      
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window); //swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) that is used to render to during this render iteration and show it as output to the screen.
        glfwPollEvents(); //checks if any events are triggered, updates window state, and calls corresponding functions (which we regist via callback methods)
        // -------------------------------------------------------------------------------
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
    // ------------------------------------------------------------------ 
}

