#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a=255) {
    return (a<<24) + (b<<16) + (g<<8) + r;
}

void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
    // &-ing with 255 to account for the fact that the image could be encoded in just RGB and not RGBA
    // how does that help?
    // consistency and ensuring 8-bits
    // shift & mask convention
    r = (color >>  0) & 255;
    g = (color >>  8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}

void write_image(const std::string filename, const std::vector<uint32_t> &image, const size_t w, const size_t h) {
    assert(image.size() == w*h);
    std::ofstream ofs(filename); // standard output file stream
    ofs << "P6\n" << w << " " << h << "\n255\n"; // header text (metadata?)
    for (size_t i = 0; i < w*h; i++) {
        uint8_t r, g, b, a;
        unpack_color(image[i], r, g, b, a);
        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    ofs.close();
}

int main() {
    const size_t win_w = 512;
    const size_t win_h = 512;
    std::vector<uint32_t> framebuffer(win_h*win_w, 255); // a 1-D vector of size 512*512 \
                                                            with all values = 255 \
                                                            (supposed initialization to a white image)
    // The following loops loop over the entire image and sweeps a red-green gradient with \
       red increasing vertically downwards and green increasing horizontally to the write
    for (size_t j = 0; j < win_h; j++) {
        for (size_t i = 0; i < win_w; i++) {
            uint8_t r = 255 * j/float(win_h);
            uint8_t g = 255 * i/float(win_w);
            uint8_t b = 0;
            framebuffer[i+j*win_w] = pack_color(r, g, b); // the pixel (i, j) is represented by a 32-bit color
        }
    }
    write_image("./out.ppm", framebuffer, win_w, win_h);
}