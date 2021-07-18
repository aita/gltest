#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <spdlog/spdlog.h>

#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;

using Vertex = glm::vec3;

struct Triangle {
  uint16_t vertices[3];
};

using TriangleList = std::vector<Triangle>;
using VertexList = std::vector<Vertex>;

// https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
namespace icosahedron {
const float X = .525731112119133606f;
const float Z = .850650808352039932f;
const float N = 0.f;

static const VertexList vertices = {
    {-X, N, Z}, {X, N, Z},   {-X, N, -Z}, {X, N, -Z}, {N, Z, X},  {N, Z, -X},
    {N, -Z, X}, {N, -Z, -X}, {Z, X, N},   {-Z, X, N}, {Z, -X, N}, {-Z, -X, N}};

static const TriangleList triangles = {
    {0, 4, 1},  {0, 9, 4},  {9, 5, 4},  {4, 5, 8},  {4, 8, 1},
    {8, 10, 1}, {8, 3, 10}, {5, 3, 8},  {5, 2, 3},  {2, 7, 3},
    {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
    {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11}};

using Lookup = std::map<std::pair<uint16_t, uint16_t>, uint16_t>;

uint16_t VertexForEdge(Lookup &lookup, VertexList &vertices, uint16_t first,
                       uint16_t second) {
  Lookup::key_type key(first, second);
  if (key.first > key.second)
    std::swap(key.first, key.second);

  auto [it, inserted] = lookup.insert({key, vertices.size()});
  if (inserted) {
    auto &edge0 = vertices[first];
    auto &edge1 = vertices[second];
    auto point = normalize(edge0 + edge1);
    vertices.push_back(point);
  }

  return it->second;
}

TriangleList Subdivide(VertexList &vertices, TriangleList triangles) {
  Lookup lookup;
  TriangleList result;

  for (auto &&each : triangles) {
    std::array<uint16_t, 3> mid;
    for (int edge = 0; edge < 3; ++edge) {
      mid[edge] = VertexForEdge(lookup, vertices, each.vertices[edge],
                                each.vertices[(edge + 1) % 3]);
    }

    result.push_back({each.vertices[0], mid[0], mid[2]});
    result.push_back({each.vertices[1], mid[1], mid[0]});
    result.push_back({each.vertices[2], mid[2], mid[1]});
    result.push_back({mid[0], mid[1], mid[2]});
  }

  return result;
}

using IndexedMesh = std::pair<VertexList, TriangleList>;

IndexedMesh MakeIcosphere(int subdivisions) {
  VertexList vertices = icosahedron::vertices;
  TriangleList triangles = icosahedron::triangles;

  for (int i = 0; i < subdivisions; ++i) {
    triangles = Subdivide(vertices, triangles);
  }

  return {vertices, triangles};
}
} // namespace icosahedron

const char *vertex_shader_source = u8R"##(#version 400
layout(location = 0) in vec3 vertex_position;
uniform mat4 MVP;

void main() {
  gl_Position = MVP * vec4(vertex_position, 1.0);
}
)##";

const char *fragment_shader_source = u8R"##(#version 400
out vec4 frag_color;

void main() {
  frag_color = vec4(1.0, 1.0, 1.0, 1.0);
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

int level = 0;
icosahedron::IndexedMesh mesh;

GLuint vbo, ebo;

void HandleKeyEvents(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    ++level;
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && level > 0) {
    --level;
  }

  mesh = icosahedron::MakeIcosphere(level);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.first.size(),
               &mesh.first[0], GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * mesh.second.size(),
               &mesh.second[0], GL_DYNAMIC_DRAW);
}

int main() {
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

  glfwSetKeyCallback(window, HandleKeyEvents);

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

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  mesh = icosahedron::MakeIcosphere(level);

  // GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.first.size(),
               &mesh.first[0], GL_DYNAMIC_DRAW);

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, BUFFER_OFFSET(0));

  // GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * mesh.second.size(),
               &mesh.second[0], GL_DYNAMIC_DRAW);

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

  glm::vec3 camera_position(3.0f, 2.0f, 2.0f);
  glm::vec3 camera_target(0.0f, 0.0f, 0.0f);
  glm::vec3 up_vector(0.0f, 1.0f, 0.0f);
  glm::mat4 view = glm::lookAt(camera_position, camera_target, up_vector);
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,
      100.0f);
  float angular_velocity = glm::pi<float>() * 0.1f;

  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    float angle = angular_velocity * time;
    glm::mat4 model = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));

    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    auto mvp = projection * view * model;
    glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    for (int i = 0; i < mesh.second.size(); ++i)
      glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_SHORT,
                     BUFFER_OFFSET(sizeof(GLushort) * 3 * i));
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
