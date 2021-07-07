#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <vector>

std::vector<glm::vec3> points = {
    {0.0f, 0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
};

const char *vertexShader = u8R"##(#version 400
in vec3 vp;

void main() {
  gl_Position = vec4(vp, 1.0);
}
)##";

const char *fragmentShader = u8R"##(#version 400
out vec4 frag_colour;

void main() {
  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);
}
)##";

int main() {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    spdlog::error("could not start GLFW3");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    spdlog::error("could not open window with GLFW3");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  if (!gladLoadGL()) {
    spdlog::error("failed to initialize OpenGL loader");
    return 1;
  }

  // get version info
  spdlog::info("Renderer: {}", glGetString(GL_RENDERER));
  spdlog::info("OpenGL version supported: {}", glGetString(GL_VERSION));

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

  // glClearColor(/* GLfloat red   = */ 0.2f,
  //              /* GLfloat green = */ 0.2f,
  //              /* GLfloat blue  = */ 0.2f,
  //              /* GLfloat alpha = */ 0.0f);

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0],
               GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
  glCompileShader(vertexShaderID);
  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderID, 1, &fragmentShader, NULL);
  glCompileShader(fragmentShaderID);

  GLuint programID = glCreateProgram();
  glAttachShader(programID, fragmentShaderID);
  glAttachShader(programID, vertexShaderID);
  glLinkProgram(programID);

  while (!glfwWindowShouldClose(window)) {
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);
    glBindVertexArray(vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers(window);
    // update other events like input handling
    glfwPollEvents();
  }

  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);

  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
