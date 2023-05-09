#pragma once
#include <vector>
#include <SFML/Graphics/Export.hpp>

namespace rendering {
class Bitmap {
public:
    Bitmap(int width, int height);

    std::vector<sf::Uint8> Fill(sf::Uint8 color);

    void SetPixel(int x, int y, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a);

    void CopyPixel(const Bitmap& map, int cur_x, int cur_y, int x, int y);

    const sf::Uint8* GetPointerToPixels();

    int Width() const;

    int Height() const;

    bool IsInsideMap(int x, int y) const;

protected:
    int width_;
    int height_;
    std::vector<sf::Uint8> pixels_;
};
}