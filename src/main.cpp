

#include "Settings.h"
#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Settings settings;

gl::Camera camera;
glm::mat4 projection_matrix;

void initialization();
void loop();
void mousePressedCallback(gl::MousePressedEvent const& event);

int main()
{
    initialization();
    loop();
}

void initialization() {
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    camera = gl::Camera{};

    switch (settings.projectionType) {
        case ProjectionType::Orthographic:
            projection_matrix = glm::ortho(-2.0f, 2.0f, -1.0f, 1.0f, 0.1f, 100.0f);
            break;

        case ProjectionType::Perspective:
            projection_matrix = glm::infinitePerspective(settings.fieldOfView, gl::framebuffer_aspect_ratio(), 0.001f);
            break;
    }

    gl::set_events_callbacks({camera.events_callbacks()});
}

void loop() {
    while (gl::window_is_open())
    {
        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds(), glm::vec3{0.f, 0.f, 1.f});

        glm::mat4 const view_projection_matrix = projection_matrix * view_matrix;
        glm::mat4 const model_view_projection_matrix = view_projection_matrix * rotation;

        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto const triangle_mesh = gl::Mesh{
                {
                    .vertex_buffers = {
                        {
                            .layout = {gl::VertexAttribute::Position3D{0}},
                            .data = {
                                -.25f, -.25f, -.25f,
                                .25f, -.25f, -.25f,
                                .25f, .25f, -.25f,
                                -.25f, .25f, -.25f,

                                -.25f, -.25f, .25f,
                                .25f, -.25f, .25f,
                                .25f, .25f, .25f,
                                -.25f, .25f, .25f,
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
        shader.set_uniform("color", glm::vec4{1.f, 1.f, 0.f, 1.f});
        shader.set_uniform("view_projection_matrix", model_view_projection_matrix);

        triangle_mesh.draw();
    }
}
