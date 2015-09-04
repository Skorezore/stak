#include <string>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

#include "icicle/detail/glsl_program.hpp"
#include "icicle/camera.hpp"

using namespace icicle;
using namespace icicle::detail;

glsl_program::glsl_program() : program_handle(0), link_status(GL_FALSE) {}

glsl_program::glsl_program(const vertex_shader& vertex_shader_, const fragment_shader& fragment_shader_) : program_handle(glCreateProgram()), link_status(GL_FALSE), vertex_shader_info_log(vertex_shader_.info_log()), fragment_shader_info_log(fragment_shader_.info_log())
{
    glAttachShader(program_handle, vertex_shader_);
    glAttachShader(program_handle, fragment_shader_);

    glBindAttribLocation(program_handle, 0, "position");
    glBindAttribLocation(program_handle, 1, "normal");
    glBindAttribLocation(program_handle, 2, "texcoord");

    glLinkProgram(program_handle);

    glDetachShader(program_handle, vertex_shader_);
    glDetachShader(program_handle, fragment_shader_);

    glGetProgramiv(program_handle, GL_LINK_STATUS, &link_status);

    GLint log_length = 0;
    glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 1)
    {
        info_log_.reserve(log_length);
        glGetProgramInfoLog(program_handle, log_length, &log_length, &info_log_[0]);
    }

    //GLuint mvp_handle = glGetUniformLocation(program_handle, "mvp");
    //glUniformMatrix4fv(mvp_handle, 1, GL_FALSE, glm::value_ptr(mvp));
}

glsl_program::glsl_program(glsl_program&& other) noexcept : program_handle(other.program_handle), vertex_shader_info_log(move(other.vertex_shader_info_log)), fragment_shader_info_log(move(other.fragment_shader_info_log)), link_status(other.link_status), info_log_(move(other.info_log_))
{
    other.program_handle = 0;
    other.link_status = GL_FALSE;
}

glsl_program::~glsl_program()
{
    glDeleteProgram(program_handle);
}

glsl_program& glsl_program::operator=(glsl_program&& other) noexcept
{
    program_handle = other.program_handle;
    matrix_handle = other.matrix_handle;
    link_status = other.link_status;
    info_log_ = std::move(other.info_log_);
    vertex_shader_info_log = std::move(other.vertex_shader_info_log);
    fragment_shader_info_log = std::move(other.fragment_shader_info_log);

    other.program_handle = 0;
    other.matrix_handle = 0;
    other.link_status = 0;

    return *this;
}

glsl_program::operator bool() const noexcept
{
    return link_status == GL_TRUE;
}

glsl_program::operator GLuint() const noexcept
{
    return program_handle;
}

const std::string& glsl_program::info_log() const noexcept
{
    return info_log_;
}

const std::string& glsl_program::shader_info_log(shader_type type) const
{
    switch (type)
    {
        case shader_type::vertex:
            return vertex_shader_info_log;

        case shader_type::fragment:
            return fragment_shader_info_log;

        default:
            throw std::invalid_argument("Shader type " + std::to_string(static_cast<GLenum>(type)) + " is not a proper shader_type.");
    }
}

void glsl_program::bind() noexcept
{
    glUseProgram(program_handle);
}
