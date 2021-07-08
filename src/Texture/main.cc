#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>

#include <iostream>
#include <memory>
#include <vector>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;

struct Vertex {
  glm::vec3 point;
  glm::vec2 texcoord;
};

std::vector<Vertex> vertices = {
    {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},   // top right
    {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}}, // bottom left
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},  // top left
};

std::vector<GLushort> indices = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

const char *vertex_shader_source = u8R"##(#version 400
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texcoord;

out vec2 texcoord;

uniform mat4 MVP;

void main() {
  texcoord = vertex_texcoord;
  gl_Position = MVP * vec4(vertex_position, 1.0);
}
)##";

const char *fragment_shader_source = u8R"##(#version 400
in vec2 texcoord;
out vec4 frag_color;

uniform sampler2D texture0;

void main() {
  frag_color = texture2D(texture0, texcoord);
}
)##";

void HandleGLFWError(int error, const char *description) {
  spdlog::error("GLFW Error: {}", description);
}

void LogShaderInfo(GLuint shader_id) {
  int max_length = 2048;
  int actual_length = 0;
  char buffer[2048];
  glGetShaderInfoLog(shader_id, max_length, &actual_length, buffer);
  spdlog::error("shader info log for GL index {}:\n{}", shader_id, buffer);
}

void LogProgramInfo(GLuint program) {
  int max_length = 2048;
  int actual_length = 0;
  char program_log[2048];
  glGetProgramInfoLog(program, max_length, &actual_length, program_log);
  spdlog::error("program info log for GL index {}:\n{}", program, program_log);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
    return 1;
  }

  stbi_set_flip_vertically_on_load(true);

  int image_width, image_height, image_nchannel;
  std::unique_ptr<unsigned char, decltype(&stbi_image_free)> image_data(
      stbi_load(argv[1], &image_width, &image_height, &image_nchannel, 0),
      stbi_image_free);

  if ((!image_data) || (0 >= image_width) || (0 >= image_height) ||
      (0 >= image_nchannel)) {
    spdlog::error("could not read image");
    return 1;
  }

  glfwSetErrorCallback(HandleGLFWError);

  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    spdlog::error("could not start GLFW3");
    return 1;
  }

  // Anti-Aliasing
  glfwWindowHint(GLFW_SAMPLES, 4);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Matrix", NULL, NULL);

  // Full-Screen
  // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  // const GLFWvidmode *vmode = glfwGetVideoMode(monitor);
  // GLFWwindow *window = glfwCreateWindow(vmode->width, vmode->height,
  //                                       "Extended GL Init", monitor, NULL);

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

  glEnable(GL_CULL_FACE); // cull face
  glCullFace(GL_BACK);    // cull back face
  glFrontFace(GL_CW);

  // glClearColor(/* GLfloat red   = */ 0.2f,
  //              /* GLfloat green = */ 0.2f,
  //              /* GLfloat blue  = */ 0.2f,
  //              /* GLfloat alpha = */ 0.0f);

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0],
               GL_STATIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        BUFFER_OFFSET(12));

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(),
               &indices[0], GL_STATIC_DRAW);

  GLuint texture = 0;
  glGenTextures(1, &texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image_data.get());
  glGenerateMipmap(GL_TEXTURE_2D);

  int params = -1;
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    spdlog::error("GL shader index {} did not compile", vertex_shader);
    LogShaderInfo(vertex_shader);
    return 1; // or exit or something
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    spdlog::error("GL shader index {} did not compile", fragment_shader);
    LogShaderInfo(fragment_shader);
    return 1; // or exit or something
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, fragment_shader);
  glAttachShader(program, vertex_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &params);
  if (GL_TRUE != params) {
    spdlog::error("could not link shader program GL index {}", program);
    LogProgramInfo(program);
    return 1;
  }

  GLuint uniform_mvp = glGetUniformLocation(program, "MVP");
  GLuint uniform_texture = glGetUniformLocation(program, "texture0");

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  float aspect_ratio = WINDOW_WIDTH / (float)WINDOW_HEIGHT;
  glm::mat4 view = glm::ortho(-1.0f * aspect_ratio, 1.0f * aspect_ratio, -1.0f,
                              1.0f, -100.0f, 100.0f);
  glm::mat4 mvp = view;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, &mvp[0][0]);
    glUniform1i(uniform_texture, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    // put the stuff we've been drawing onto the display
    glfwSwapBuffers(window);
    // update other events like input handling
    glfwPollEvents();

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
  }

  glDetachShader(program, vertex_shader);
  glDetachShader(program, fragment_shader);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
