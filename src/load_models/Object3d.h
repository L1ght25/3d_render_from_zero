#pragma once
#include "../geometric_primitives/Vertex.h"
#include <array>
#include <fstream>
#include <sstream>

namespace load {
unsigned int countWordsInString(std::string const& str);

class Object3d {
  public:
    using Vertex = geometry::Vertex;
    Object3d(std::string_view filename, bool is_auto_rotation = false, bool is_inversed_z = false, double x_delta = 0, double y_delta = 0,
             double z_delta = 0);

    const Vertex& GetVertexByInd(int i) const;

    size_t SizeOfPolygons() const;

    bool IsAutoTransformed() const;

  private:
    std::vector<Vertex> vertices_;
    std::vector<size_t> indexes_;
    bool is_auto_rotation_;
};
}  // namespace load
