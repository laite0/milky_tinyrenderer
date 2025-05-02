#include <cmath>
#include <cstdlib>
#include <ctime>
#include "tgaimage.h"

const TGAColor white   = {255, 255, 255, 255}; // BGRA order
const TGAColor green   = {  0, 255,   0, 255};
const TGAColor red     = {  0,   0, 255, 255};
const TGAColor blue    = {255, 128,  64, 255};
const TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
	if (bx < ax) {
		std::swap(ax, bx);
		std::swap(ay, by);
	}
	bool steep = (by - ay) > (bx - ax);
	if (steep) {
		std::swap(ax, ay);
		std::swap(bx, by);
	}
	int y = ay;
	int scaled_y_to_int_y = 0;
	bool up = by > ay;
	int k = std::abs(by - ay);
	int dx = bx - ax;
	for (int x = ax; x < bx; x++) {
		if (steep) {
			framebuffer.set(y, x, color);
		} else {
			framebuffer.set(x, y, color);
		}
		scaled_y_to_int_y += 2 * k;
		int change_y = scaled_y_to_int_y > dx;
		y += change_y * (up ? 1 : -1);
		scaled_y_to_int_y -= 2 * dx * change_y;
	}
}

int main(int argc, char** argv) {
	constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::srand(std::time({}));
    for (int i=0; i<(1<<24); i++) {
        int ax = rand()%width, ay = rand()%height;
        int bx = rand()%width, by = rand()%height;
        line(ax, ay, bx, by, framebuffer, { rand()%255, rand()%255, rand()%255, rand()%255 });
    }
	// line(0, 20, 50, 50, framebuffer, { rand()%255, rand()%255, rand()%255, rand()%255 });

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}