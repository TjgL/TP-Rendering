#include "utils.h"
#include "opengl-framework/opengl-framework.hpp"

#include <iostream>

bool LoadMesh(std::filesystem::path const &path, gl::Mesh*& loadedMesh) {
    auto reader = tinyobj::ObjReader();
    reader.ParseFromFile(gl::make_absolute_path(path).string(), {});

    if (!reader.Error().empty())
        throw std::runtime_error("Failed to read 3D model:\n" + reader.Error());
    if (!reader.Warning().empty())
        std::cout << "Warning while reading 3D mode:\n " << reader.Warning() << "\n";

    auto vertices = std::vector<float>{};
    for (auto const& shape : reader.GetShapes()) {
        for (auto const& idx : shape.mesh.indices) {
            // Positions
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 1]); // Can be inverted with the next line to fix incorrect model orientation
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 2]); // But it also must be done for the normals

            // UVs
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 0]);
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 1]);

            // Normals
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 0]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 1]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 2]);
        }
    }

    loadedMesh = new gl::Mesh {
        {
            .vertex_buffers = {
                {
                    .layout = {gl::VertexAttribute::Position3D{0},
                        gl::VertexAttribute::UV{1},
                        gl::VertexAttribute::Normal3D{2}},
                    .data = vertices,
                }
            },
        }
    };

    return true;
}

