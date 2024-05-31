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

void write_image(const std::string filename, const std::vector<uint32_t> &image, const size_t w, const size_t h) { // image is the framebuffer (a 1-D vector)
    assert(image.size() == w*h);
    std::ofstream ofs(filename); // standard output file stream
    ofs << "P6\n" << w << " " << h << "\n255\n"; // header text (metadata?)
    for (size_t i = 0; i < w*h; i++) {
        uint8_t r, g, b, a;
        unpack_color(image[i], r, g, b, a); // the framebuffer is unpacked \
                                               the colors are appended to the file stream (how?)
        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    ofs.close();
}

// This function draws each block of our game map
void draw_rectangle(std::vector<uint32_t> &image, const size_t img_w, const size_t img_h, const size_t x,
                    const size_t y, const size_t w, const size_t h, const uint32_t color) {
    assert(image.size() == img_w*img_h);
    // this is essentially doing the same thing as the gradient loop in main()
    // for every "block", we get its top-left corner as (x,y) and that block's width and height
    // then we run a loop from x to x+w and y to y+h and fill with cyan color
    // (to start from the x and y of a particular block we pass those in as constants that vary in the loop in main())
    // future Sabeer I hope you get this as clearly as you did back then
    for (size_t i = 0; i < w; i++) {
        for (size_t j = 0; j < h; j++) {
            size_t cx = x + i;
            size_t cy = y + j;
            assert(cx < img_w && cy < img_h);
            image[cx + cy*img_w] = color;
        }
    }
}

int main() {
    const size_t win_w = 512;
    const size_t win_h = 512;
    std::vector<uint32_t> framebuffer(win_h*win_w, 255); // a 1-D vector of size 512*512 \
                                                            with all values = 255 \
                                                            (supposed initialization to a white image)
    // The following loops loop over the entire image and sweeps a red-green gradient with \
       red increasing vertically downwards and green increasing horizontally to the write

    const size_t map_w = 16;
    const size_t map_h = 16;
    const char map[] = "0000222222220000"\
                       "1              0"\
                       "1      11111   0"\
                       "1     0        0"\
                       "0     0  1110000"\
                       "0     2        0"\
                       "0   30400      0"\
                       "0   0   11100  0"\
                       "0   0   0      0"\
                       "0   0   1  00000"\
                       "0       6      0"\
                       "2       1      0"\
                       "0       0      0"\
                       "0 0000000      0"\
                       "0              0"\
                       "0002222222200000"; // our game map | are these numbers random? each char represents a "rectangle block"
    assert(sizeof(map) == map_w*map_h + 1); // +1 for \0 terminator
    float player_x = 3.456; // player x position (in units relative to map-size)
    float player_y = 2.345; // player y position (in units relative to map-size)

    for (size_t j = 0; j < win_h; j++) {
        for (size_t i = 0; i < win_w; i++) {
            uint8_t r = 255 * j/float(win_h);
            uint8_t g = 255 * i/float(win_w);
            uint8_t b = 0;
            framebuffer[i+j*win_w] = pack_color(r, g, b); // the pixel (i, j) is represented by a 32-bit color
        }
    }

    // rect is one block unit
    const size_t rect_w = win_w/map_w;
    const size_t rect_h = win_h/map_h;
    for (size_t j = 0; j < map_h; j++) {
        for (size_t i = 0; i < map_w; i++) {
            if (map[i+j*map_w] == ' ') continue;
            size_t rect_x = i*rect_w;
            size_t rect_y = j*rect_h;
            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, pack_color(0, 255, 255)); // O(n⁴) 💀
        }
    }

    // draw player on map
    draw_rectangle(framebuffer, win_w, win_h, player_x*rect_w, player_y*rect_h, 5, 5, pack_color(255, 255, 255));

    write_image("./out.ppm", framebuffer, win_w, win_h);
}