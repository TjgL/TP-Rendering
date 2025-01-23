#include <iostream>

#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    while (gl::window_is_open())
    {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);

        glClearColor(0.f, 0.f, .5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto const triangle_mesh = gl::Mesh{
            {
                .vertex_buffers = {
                    {
                        .layout = {gl::VertexAttribute::Position2D{0}},
                        .data = {
                            -0.25f, +0.25f, // L-T
                            +0.25f, +0.25f, // R-T
                            -0.25f, -0.25f, // L-B
                            +0.25f, -0.25f, // R-B
                        },
                    }},
                .index_buffer = {
                    0, 1, 2,
                    1, 2, 3
                },
            }};

        auto const shader = gl::Shader{
            {
                .vertex = gl::ShaderSource::File{"res/vertex.glsl"},
                .fragment = gl::ShaderSource::File{"res/fragment.glsl"}
            }};

        float offset = 0.f;
        for (int i = 100; i > 0; i--) {
            offset -= gl::delta_time_in_seconds();
            shader.bind();
            shader.set_uniform("color", glm::vec4{1.f, 1.f, 0.f, static_cast<float>(i) / 100.f});
            shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
            shader.set_uniform("time", gl::time_in_seconds() + offset);

            triangle_mesh.draw();
        }
    }
}