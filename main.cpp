#include <cmath>
#include <iostream>
#include "tgaimage.h"
#include "model.h"

const TGAColor white   = {255, 255, 255, 255}; // BGRA order
const TGAColor green   = {  0, 255,   0, 255};
const TGAColor red     = {  0,   0, 255, 255};
const TGAColor blue    = {255, 128,  64, 255};
const TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, const TGAColor& color) {
    bool steep = std::abs(by - ay) > std::abs(bx - ax);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (bx < ax) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

	int y = ay;
	int scaled_y_to_int_y = 0;
	bool up = by > ay;
	int k = std::abs(by - ay);
	int dx = bx - ax;
	for (int x = ax; x <= bx; x++) {
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
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

	constexpr int width  = 800;
    constexpr int height = 800;
    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);


    for (int i = 0; i < model.numberOfFaces(); i++) {
        std::vector<int> face = model.faceAt(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertexAt(face[j]);
            Vec3f v1 = model.vertexAt(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, framebuffer, white);
        }
    }

    framebuffer.flip_vertically();

	framebuffer.write_tga_file("wire_frame_attempt.tga");
	return 0;
}