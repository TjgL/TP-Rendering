#include <iostream>

#include "Settings.h"
#include "opengl-framework/opengl-framework.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Settings settings;

gl::Camera camera;
glm::mat4 projection_matrix;

gl::Texture* object_texture;
gl::Shader* object_shader;

gl::RenderTarget* render_target;
gl::Shader* screen_shader;

void init();
void mainLoop();
void drawRenderTarget();
void render();
void frameBufferResizedEvent(gl::FramebufferResizedEvent const& e);
void cleanup();

gl::Mesh load_mesh(std::filesystem::path const& path);

int main() {
    init();
    mainLoop();
    cleanup();
}

void init() {
    gl::init("TPs de Rendering");
    gl::maximize_window();

    glEnable(GL_DEPTH_TEST);

    camera = gl::Camera{};

    switch (settings.projectionType) {
        case ProjectionType::Orthographic:
            projection_matrix = glm::ortho(-2.0f, 2.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        break;

        case ProjectionType::Perspective:
            projection_matrix = glm::infinitePerspective(settings.fieldOfView, gl::framebuffer_aspect_ratio(), 0.001f);
        break;
    }

    render_target = new gl::RenderTarget{gl::RenderTarget_Descriptor {
        .width = gl::framebuffer_width_in_pixels(),
        .height = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor {
                .format = gl::InternalFormat_Color::RGBA8,
                .options = {
                    .minification_filter = gl::Filter::NearestNeighbour,
                    .magnification_filter = gl::Filter::NearestNeighbour,
                    .wrap_x = gl::Wrap::ClampToEdge,
                    .wrap_y = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x = gl::Wrap::ClampToEdge,
                .wrap_y = gl::Wrap::ClampToEdge,
            }
        }
    }};

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = frameBufferResizedEvent},
    });

    object_texture = new gl::Texture {
        gl::TextureSource::File {
            .path = "res/texture.png",
            .flip_y = true,
            .texture_format = gl::InternalFormat::RGBA8
        },
        gl::TextureOptions {
            .minification_filter = gl::Filter::Linear,
            .magnification_filter = gl::Filter::Linear,
            .wrap_x = gl::Wrap::Repeat,
            .wrap_y = gl::Wrap::Repeat,
        }
    };

    object_shader = new gl::Shader{
    {
        .vertex = gl::ShaderSource::File{"res/object.vert"},
        .fragment = gl::ShaderSource::File{"res/object.frag"}
    }};

    screen_shader = new gl::Shader{
    {
        .vertex = gl::ShaderSource::File{"res/screen.vert"},
        .fragment = gl::ShaderSource::File{"res/screen.frag"}
    }};

    auto boat_mesh = load_mesh("res\\fourareen.obj");
}


void mainLoop() {
    while (gl::window_is_open()) {
        render_target->render(drawRenderTarget);
        render();
    }
}

void drawRenderTarget() {
    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 const view_matrix = camera.view_matrix();
    glm::mat4 const view_projection_matrix = projection_matrix * view_matrix;

    auto const cube_mesh = gl::Mesh{
        {
            .vertex_buffers = {
                {
                    .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}},
                    .data = {
                        -0.25f, -0.25f, -0.25f,   0.f, 0.f,
                        0.25f, -0.25f, -0.25f,    1.f, 0.f,
                        0.25f, 0.25f, -0.25f,     1.f, 1.f,
                        -0.25f, 0.25f, -0.25f,    0.f, 1.f,

                        -0.25f, -0.25f, 0.25f,    0.f, 0.f,
                        0.25f, -0.25f, 0.25f,     1.f, 0.f,
                        0.25f, 0.25f, 0.25f,      1.f, 1.f,
                        -0.25f, 0.25f, 0.25f,     0.f, 1.f,
                    },
                }},
            .index_buffer = {
                // Bottom
                0, 1, 2,
                2, 3, 0,

                // Front
                0, 1, 5,
                5, 4, 0,

                // Left
                0, 3, 7,
                7, 4, 0,

                // Back
                2, 3, 7,
                7, 6, 2,

                // Right
                1, 2, 6,
                6, 5, 1,

                // Top
                4, 5, 6,
                6, 7, 4
            },
        }};

    object_shader->bind();
    object_shader->set_uniform("view_projection_matrix", view_projection_matrix);
    object_shader->set_uniform("my_texture", *object_texture);

    cube_mesh.draw();
}

void render() {
    glClearColor(0.12f, 0.16f, 0.52f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto const plane = gl::Mesh {
        {
            .vertex_buffers = {
                {
                    .layout = {gl::VertexAttribute::Position2D{0}, gl::VertexAttribute::UV{1}},
                    .data = {
                        -1, 1,      0, 1,
                        1, 1,       1, 1,
                        -1, -1,     0, 0,
                        1, -1,      1, 0,
                    },
                }},
            .index_buffer = {
                0, 1, 2,
                1, 2, 3
            },
        }
    };

    screen_shader->bind();
    screen_shader->set_uniform("render_target_texture", render_target->color_texture(0));

    plane.draw();
}

void frameBufferResizedEvent(gl::FramebufferResizedEvent const& e) {
    if(e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
        render_target->resize(e.width_in_pixels, e.height_in_pixels);
}

void cleanup() {
    delete render_target;
    delete object_texture;
    delete object_shader;
    delete screen_shader;
}

gl::Mesh load_mesh(std::filesystem::path const& path) {
    auto reader = tinyobj::ObjReader();
    reader.ParseFromFile(gl::make_absolute_path(path).string(), {});

    if (!reader.Error().empty())
        throw std::runtime_error("Failed to read 3D model:\n" + reader.Error());
    if (!reader.Warning().empty())
        std::cout << "Warning while reading 3D mode:\n " << reader.Warning() << "\n";

    auto vertices = std::vector<float>{};
    for (auto const& shape : reader.GetShapes()) {
        for (auto const& idx : shape.mesh.indices) {
            // Position
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 1]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 2]);

            // UV
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 0]);
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 1]);

            // Normale
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 0]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 1]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 2]);
        }
    }

    auto mesh = gl::Mesh {
        {
            .vertex_buffers = {
                {
                    .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}, gl::VertexAttribute::Normal3D{2}},
                    .data = vertices,
                }},
        }
    };

    return mesh;
}