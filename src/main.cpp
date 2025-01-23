#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto const triangle_mesh = gl::Mesh{
                {
                    .vertex_buffers = {
                        {
                            .layout = {gl::VertexAttribute::Position2D{0}},
                            .data = {
                                -0.5f, +0.5f, // L-T
                                +0.5f, +0.5f, // R-T
                                -0.5f, -0.5f, // L-B
                                +0.5f, -0.5f, // R-B
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

        shader.bind();
        triangle_mesh.draw();
    }
}