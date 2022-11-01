// A wrapper around a simple Shader_Program.
// The program lets you set these uniform values:
//  - modeling transform
//  - camera's projection-view transform
//  - the vertex color, and normal
// Diffuse lighting is done.  Objects will look like matte paint.

#include "glm/gtc/type_ptr.hpp" // glm::value_ptr
#include <iostream>

#include "diffuse_lighter.hpp"
#include "gl_error.hpp"
#include "shader.hpp"

//////////////////////////
// The vertex shader.
// Apply model-view-projection transform, and set the vertex color.
//////////////////////////

const char *Diffuse_Lighter::vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
out vec4 vertexColor;

uniform mat4 proj_view;
uniform mat4 transform;
uniform vec4 color;

void main()
{
    vec4 light_position = vec4(10, 10, 10, 1);
    vec4 light_color = vec4(1, 1, 1, 1);
    vec4 N = transform * aNormal;
    gl_Position = proj_view * transform * aPos;

    vec4 P_wcs = transform * aPos;
    vec4 ambient_light = vec4(0.3, 0.2, 0, 1);
    vec4 light_direction = normalize(light_position - P_wcs);
    float brightness = max(dot(N, light_direction), 0);
    vertexColor = ambient_light + color * brightness * light_color;
}
)";

//////////////////////////
// The fragment shader.
// Just pass the vertex color through to fragment.
//////////////////////////

const char *Diffuse_Lighter::fragment_shader_source = R"(
#version 330 core
in vec4 vertexColor;
out vec4 output_color;

void main()
{
    output_color = vertexColor;
}
)";

Diffuse_Lighter::Diffuse_Lighter() {
}

void Diffuse_Lighter::init() {
    install_shader();
}

void Diffuse_Lighter::install_shader() {
    // Set up the shaders for the vertical and horizontal lines.
    Shader_Program program(vertex_shader_source,
                           fragment_shader_source);
    shader_program_handle = program.get_handle();
    GL_Error::check("Diffuse_Lighter::install_shader: get program handle");

    // Get handles for the vertex shader's uniform params
    glUseProgram(shader_program_handle);

    transform_uniform_loc = glGetUniformLocation(shader_program_handle,
                                                 "transform");
    GL_Error::check("Diffuse_Lighter::install_shader: get transform loc");

    proj_view_uniform_loc = glGetUniformLocation(shader_program_handle,
                                                 "proj_view");
    GL_Error::check("Diffuse_Lighter::install_shader: get proj_view loc");

    color_uniform_loc = glGetUniformLocation(shader_program_handle,
                                             "color");
    GL_Error::check("Diffuse_Lighter::install_shader: get proj_view loc");

}

void Diffuse_Lighter::set_model_transform(const glm::mat4& transform) const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Diffuse_Lighter::set_model_transform: glUseProgram");

    glUniformMatrix4fv(transform_uniform_loc, 1, GL_FALSE,
                       glm::value_ptr(transform));
    GL_Error::check("Diffuse_Lighter::set_model_transform: set transform uniform");
}

void Diffuse_Lighter::set_color(const glm::vec4& color) const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Diffuse_Lighter::set_color: glUseProgram");

    glUniform4fv(color_uniform_loc, 1, glm::value_ptr(color));
    GL_Error::check("Diffuse_Lighter::set_color: set color uniform");
}

void Diffuse_Lighter::set_camera(const Camera& cam) const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Diffuse_Lighter::set_camera: glUseProgram");

    cam.update_shader(shader_program_handle,
                      (GLint) proj_view_uniform_loc);
}

GLuint Diffuse_Lighter::get_program_handle() const {
    return shader_program_handle;
}

void Diffuse_Lighter::use() const {
    glUseProgram(shader_program_handle);
    GL_Error::check("Diffuse_Lighter::draw. UseProgram");
}


