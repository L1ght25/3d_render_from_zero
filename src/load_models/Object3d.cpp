#include "Object3d.h"
#include <string_view>

unsigned int load::countWordsInString(std::string const& str) {
    std::stringstream stream(str);
    return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
}

const geometry::Vertex& load::Object3d::GetVertexByInd(int i) const {
    return vertices_[indexes_[i]];
}

size_t load::Object3d::SizeOfPolygons() const {
    return indexes_.size();
}

bool load::Object3d::IsAutoTransformed() const {
    return is_auto_rotation_;
}

load::Object3d::Object3d(std::string_view filename, std::string_view texture_filename, bool is_auto_rotation, bool is_inversed_z, double x_delta, double y_delta, double z_delta)
    : is_auto_rotation_(is_auto_rotation), texture_(texture_filename) {
    std::vector<geometry::Vector4d> texture_coords_;
    std::vector<geometry::Vector4d> normal_coords_;
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        throw std::runtime_error("Cannot read file");
    }
    std::string curr_line;
    std::array<int, 4> vertices;
    std::array<int, 4> texture;
    std::array<int, 4> normal;

    while (getline(in, curr_line)) {
        texture.fill(0);
        vertices.fill(0);
        normal.fill(0);
        if (curr_line.starts_with("v ") or curr_line.starts_with("vt ") or curr_line.starts_with("vn ")) {
            std::istringstream in_vert(curr_line.substr(2));
            double x, y, z;
            in_vert >> x;
            in_vert >> y;
            if (curr_line.starts_with("v ")) {
                in_vert >> z;
                x += x_delta;
                if (is_inversed_z) {
                    y += z_delta;
                    z += y_delta;
                    vertices_.emplace_back(-x, -z, -y);
                } else {
                    y += y_delta;
                    z += z_delta;
                    vertices_.emplace_back(-x, -y, -z);  // because of inversed value of z :))
                }
            } else if (curr_line.starts_with("vt")) {
                texture_coords_.emplace_back(x, 1 - y, 0, 0);
            } else {
                in_vert >> z;
                normal_coords_.emplace_back(x, y, z, 0);
            }
        } else if (curr_line.starts_with("f ")) {
            bool has_4_dim = false;
            if (countWordsInString(curr_line) == 5) {
                if (curr_line.contains("//")) {
                    continue;
                } else {
                    sscanf(curr_line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &vertices[0], &texture[0], &normal[0], &vertices[1],
                           &texture[1], &normal[1], &vertices[2], &texture[2], &normal[2], &vertices[3], &texture[3], &normal[3]);
                }
                has_4_dim = true;
            } else {
                if (curr_line.contains("//")) {
                    continue;
                } else {
                    sscanf(curr_line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &vertices[0], &texture[0], &normal[0], &vertices[1], &texture[1],
                           &normal[1], &vertices[2], &texture[2], &normal[2]);
                }
            }
            for (int i = 0; i < (has_4_dim ? 4 : 3); ++i) {
                if (i == 3) {
                    if (vertices[i] < 1) {
                        break;
                    }
                    indexes_.push_back(vertices[0]);
                    indexes_.push_back(vertices[2]);
                }
                indexes_.push_back(--vertices[i]);
                if (texture[i] > 0) {
                    vertices_[vertices[i]].SetTexturePos(texture_coords_[--texture[i]]);
                }
                if (normal[i] > 0) {
                    vertices_[vertices[i]].SetNormalPos(normal_coords_[--normal[i]]);
                }
            }
        }
    }
}

load::Object3d::Texture::Texture(std::string_view texture_path) {
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

const rendering::Bitmap& load::Object3d::GetTexture() const {
    return texture_.texture_map_;
}