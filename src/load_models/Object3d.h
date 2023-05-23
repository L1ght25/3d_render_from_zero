#pragma once
#include "../geometric_primitives/Vertex.h"
#include "../rendering/bitmap/bitmap.h"
#include "../constants/constants.h"
#include <array>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>

namespace load {
unsigned int countWordsInString(std::string const& str);

class Object3d {
  public:
    using Vertex = geometry::Vertex;
    Object3d(std::string_view filename, std::string_view texture_filename, bool is_auto_rotation = false,
             bool is_inversed_z = false, double x_delta = 0, double y_delta = 0, double z_delta = 0);

    const Vertex& GetVertexByInd(int i) const;

    size_t SizeOfPolygons() const;

    bool IsAutoTransformed() const;

    const rendering::Bitmap& GetTexture() const;

  private:
    struct Texture {
        Texture(std::string_view texture_path);
        rendering::Bitmap texture_map_;
    };

    std::vector<Vertex> vertices_;
    std::vector<size_t> indexes_;
    Texture texture_;
    bool is_auto_rotation_;
};
}  // namespace load
