#include "Settings.h"
#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Settings settings;

gl::Camera camera;
glm::mat4 projection_matrix;
gl::RenderTarget* render_target;


int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

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

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
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
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
        if(e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
            render_target.resize(e.width_in_pixels, e.height_in_pixels);
        }},
    });

    gl::Texture texture = gl::Texture {
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

    while (gl::window_is_open()) {
        render_target.render([&]() {
            glClearColor(1.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 const view_matrix = camera.view_matrix();
            glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds(), glm::vec3{0.f, 0.f, 1.f});

            glm::mat4 const view_projection_matrix = projection_matrix * view_matrix;
            glm::mat4 const model_view_projection_matrix = view_projection_matrix * rotation;


            auto const cube_mesh = gl::Mesh{
                    {
                        .vertex_buffers = {
                            {
                                .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}},
                                .data = {
                                    -1.f, -1.f, -1.f,   0.f, 0.f,
                                    1.f, -1.f, -1.f,    1.f, 0.f,
                                    1.f, 1.f, -1.f,     1.f, 1.f,
                                    -1.f, 1.f, -1.f,    0.f, 1.f,

                                    -1.f, -1.f, 1.f,    0.f, 0.f,
                                    1.f, -1.f, 1.f,     1.f, 0.f,
                                    1.f, 1.f, 1.f,      1.f, 1.f,
                                    -1.f, 1.f, 1.f,     0.f, 1.f,
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

            auto const shader = gl::Shader{
                    {
                        .vertex = gl::ShaderSource::File{"res/vertex.glsl"},
                        .fragment = gl::ShaderSource::File{"res/fragment.glsl"}
                    }};

            shader.bind();
            shader.set_uniform("view_projection_matrix", view_projection_matrix);
            shader.set_uniform("my_texture", texture);

            cube_mesh.draw();
        });

        // Draw render target texture on mesh here
    }
}
