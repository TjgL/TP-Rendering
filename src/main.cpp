#include <utils.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>

#include "Settings.h"
#include "opengl-framework/opengl-framework.hpp"
#include "glm/ext/matrix_clip_space.hpp"

Settings settings;

gl::Camera camera;
glm::mat4 projection_matrix;

gl::Texture* object_texture;
gl::Shader* object_shader;

gl::RenderTarget* render_target;
gl::Shader* screen_shader;

gl::Mesh* mesh;

void init();
void mainLoop();
void drawRenderTarget();
void render();
void frameBufferResizedEvent(gl::FramebufferResizedEvent const& e);
void cleanup();


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
            .path = "res\\fourareen2K_albedo.jpg",
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

    LoadMesh("res\\fourareen.obj", mesh);
}


void mainLoop() {
    while (gl::window_is_open()) {
        render_target->render(drawRenderTarget);
        render();
    }
}

void drawRenderTarget() {
    glClearColor(0.4f, 0.57f, 0.62f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 const view_matrix = camera.view_matrix();
    glm::mat4 const view_projection_matrix = projection_matrix * view_matrix;

    glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, glm::radians(90.f), glm::vec3{1.f, 0.f, 0.f});
    glm::mat4 const model_view_projection_matrix = view_projection_matrix * rotation;

    object_shader->bind();
    object_shader->set_uniform("view_projection_matrix", model_view_projection_matrix);
    object_shader->set_uniform("my_texture", *object_texture);

    object_shader->set_uniform("directional_light.direction", glm::vec3{0.2, 0.3, -1});
    object_shader->set_uniform("directional_light.color", glm::vec3{1.f, 1.f, 1.f});
    object_shader->set_uniform("directional_light.ambient", glm::vec3{0.3f, 0.3f, 0.3f});


    mesh->draw();
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
