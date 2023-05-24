#include "Object3d.h"
#include <string_view>

namespace {
    unsigned int countWordsInString(std::string_view str) {
        std::stringstream stream(str.data());
        return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
    }
}

model::Object3d::Object3d(std::string_view filename, std::string_view texture_filename, ObjectProperty behaviour, ObjectProperty is_inversed_z, double x_delta, double y_delta, double z_delta)
    : behaviour_(behaviour), texture_(texture_filename) {
    ParseFile(filename, behaviour, is_inversed_z, x_delta, y_delta, z_delta);
}

const geometry::Vertex& model::Object3d::GetVertexByInd(int i) const {
    return vertices_[indexes_[i]];
}

size_t model::Object3d::SizeOfPolygons() const {
    return indexes_.size();
}

bool model::Object3d::IsAutoTransformed() const {
    return behaviour_ == IsAutoRotated;
}

const rendering::Bitmap& model::Object3d::GetTexture() const {
    return texture_.texture_map_;
}

void model::Object3d::ParseFile(std::string_view filename, ObjectProperty behaviour, ObjectProperty is_inversed_z, double x_delta, double y_delta, double z_delta) {

    std::ifstream in(filename, std::ios::in);
    if (!in) {
        throw std::runtime_error("Cannot read file");
    }

    std::vector<geometry::Vector4d> texture_coords;
    std::vector<geometry::Vector4d> normal_coords;
    std::string curr_line;

    while (getline(in, curr_line)) {
        if (curr_line.starts_with("v ")) {
            ParseVertices(curr_line, is_inversed_z, x_delta, y_delta, z_delta);
        } else if (curr_line.starts_with("vt ")) {
            ParseTexture(curr_line, texture_coords);
        } else if (curr_line.starts_with("vn ")) {
            ParseNormal(curr_line, normal_coords);
        } else if (curr_line.starts_with("f ")) {
            ParseTriangles(curr_line, texture_coords, normal_coords);
        }
    }
}

void model::Object3d::ParseVertices(std::string_view curr_line, ObjectProperty is_inversed_z, double x_delta, double y_delta, double z_delta) {
    std::istringstream in_vert(curr_line.substr(2).data());
    double x, y, z;
    in_vert >> x;
    in_vert >> y;
    in_vert >> z;
    x += x_delta;
    if (is_inversed_z == model::IsInvertedZComp) {
        y += z_delta;
        z += y_delta;
        vertices_.emplace_back(-x, -z, -y);
    } else {
        y += y_delta;
        z += z_delta;
        vertices_.emplace_back(-x, -y, -z);  // because of inversed value of z :))
    }
}

void model::Object3d::ParseTexture(std::string_view curr_line, std::vector<Vector4d>& texture_coords) {
    std::istringstream in_vert(curr_line.substr(2).data());
    double x, y;
    in_vert >> x;
    in_vert >> y;
    texture_coords.emplace_back(x, 1 - y, 0, 0);
}

void model::Object3d::ParseNormal(std::string_view curr_line, std::vector<Vector4d>& normal_coords) {
    std::istringstream in_vert(curr_line.substr(2).data());
    double x, y, z;
    in_vert >> x;
    in_vert >> y;
    in_vert >> z;
    normal_coords.emplace_back(x, y, z, 0);
}

void model::Object3d::ParseTriangles(std::string_view curr_line, std::vector<Vector4d>& texture_coords, std::vector<Vector4d>& normal_coords) {
    if (curr_line.contains("//")) {  // not supported yet
        return;
    }

    std::array<int, 4> curr_vertices_indexes;
    std::array<int, 4> curr_texture_indexes;
    std::array<int, 4> curr_normal_indexes;
    bool has_4_dim = false;
    if (countWordsInString(curr_line) == 5) {
        sscanf(curr_line.data(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &curr_vertices_indexes[0], &curr_texture_indexes[0], &curr_normal_indexes[0],
                &curr_vertices_indexes[1], &curr_texture_indexes[1], &curr_normal_indexes[1],
                &curr_vertices_indexes[2], &curr_texture_indexes[2], &curr_normal_indexes[2],
                &curr_vertices_indexes[3], &curr_texture_indexes[3], &curr_normal_indexes[3]);
        has_4_dim = true;
    } else {
        sscanf(curr_line.data(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &curr_vertices_indexes[0], &curr_texture_indexes[0], &curr_normal_indexes[0],
                &curr_vertices_indexes[1], &curr_texture_indexes[1], &curr_normal_indexes[1],
                &curr_vertices_indexes[2], &curr_texture_indexes[2], &curr_normal_indexes[2]);
    }
    for (int i = 0; i < (has_4_dim ? 4 : 3); ++i) {
        if (i == 3) {
            if (curr_vertices_indexes[i] < 1) {
                break;
            }
            indexes_.push_back(curr_vertices_indexes[0]);
            indexes_.push_back(curr_vertices_indexes[2]);
        }
        indexes_.push_back(--curr_vertices_indexes[i]);
        if (curr_texture_indexes[i] > 0) {
            vertices_[curr_vertices_indexes[i]].SetTexturePos(texture_coords[--curr_texture_indexes[i]]);
        }
        if (curr_normal_indexes[i] > 0) {
            vertices_[curr_vertices_indexes[i]].SetNormalPos(normal_coords[--curr_normal_indexes[i]]);
        }
    }
}

model::Object3d::Texture::Texture(std::string_view texture_path) {
    sf::Image text;
    text.loadFromFile(texture_path.data());
    texture_map_ = rendering::Bitmap(text.getSize().x, text.getSize().y);
    for (int x = 0; x < text.getSize().x; ++x) {
        for (int y = 0; y < text.getSize().y; ++y) {
            sf::Color col(text.getPixel(x, y));
            texture_map_.SetPixel(x, y, col.r, col.g, col.b, constants::max_light);
        }
    }
}
