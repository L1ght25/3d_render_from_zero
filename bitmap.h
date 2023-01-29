#pragma once
#include <vector>
#include <SFML/Graphics/Export.hpp>

class Bitmap {
public:
    Bitmap(int width, int height) : width_(width), height_(height), pixels_(width * height * 4) {

    }

    auto Fill(sf::Uint8 color) -> std::vector<sf::Uint8> {
        std::fill(pixels_.begin(), pixels_.end(), color);
        return pixels_;
    }

    void SetPixel(int x, int y, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a) {
        int ind = (x + y * width_) * 4;
        pixels_[ind] = r;
        pixels_[ind + 1] = g;
        pixels_[ind + 2] = b;
        pixels_[ind + 3] = a;
    }

    auto GetPointerToPixels() -> const sf::Uint8* {
        return pixels_.data();
    }

    int Width() const {
        return width_;
    }

    int Height() const {
        return height_;
    }

    bool IsInsideMap(int x, int y) {
        return 0 <= x and x < width_ and 0 <= y and y < height_;
    }

protected:
    int width_;
    int height_;
    std::vector<sf::Uint8> pixels_;
};