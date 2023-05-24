#pragma once
#include "../geometric_primitives/Vertex.h"
#include "../rendering/bitmap/bitmap.h"
#include "../constants/constants.h"
#include <array>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <string_view>

namespace model {
enum ObjectProperty {
  Static,
  IsAutoRotated,
  IsInvertedZComp,
  IsNotInversedZComp
};

class Object3d {
  public:
    using Vertex = geometry::Vertex;
    using Vector4d = geometry::Vector4d;

    Object3d(std::string_view filename, std::string_view texture_filename, ObjectProperty behaviour = Static,
             ObjectProperty is_inversed_z = IsNotInversedZComp, double x_delta = constants::default_model_x, double y_delta = constants::default_model_y,
             double z_delta = constants::default_model_z);

    const Vertex& GetVertexByInd(int i) const;

    size_t SizeOfPolygons() const;

    bool IsAutoTransformed() const;

    const rendering::Bitmap& GetTexture() const;

  private:
    void ParseFile(std::string_view filename, ObjectProperty behaviour, ObjectProperty is_inversed_z, double x_delta, double y_delta, double z_delta);

    void ParseVertices(std::string_view curr_line, ObjectProperty is_inversed_z, double x_delta, double y_delta, double z_delta);
    void ParseTexture(std::string_view curr_line, std::vector<Vector4d>& texture_coords);
    void ParseNormal(std::string_view curr_line, std::vector<Vector4d>& normal_coords);
    void ParseTriangles(std::string_view curr_line, std::vector<Vector4d>& texture_coords, std::vector<Vector4d>& normal_coords);

    struct Texture {
        Texture(std::string_view texture_path);
        rendering::Bitmap texture_map_;
    };

    std::vector<Vertex> vertices_;
    std::vector<size_t> indexes_;
    Texture texture_;
    ObjectProperty behaviour_;
};
}  // namespace model
