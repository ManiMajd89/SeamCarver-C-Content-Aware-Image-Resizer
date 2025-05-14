#include <math.h>
#include <float.h>
#include <stdio.h>
#include "seamcarving.h"

int mod(const int a, const int b) {
    return ((a % b) + b) % b;
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    int h = im->height;
    int w = im->width;

    //Allocate memory for grad image
    create_img(grad, h, w);

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            // Wrap-around indices
            int left = (x - 1 + w) % w;
            int right = (x + 1) % w;
            int up = (y - 1 + h) % h;
            int down = (y + 1) % h;

            // Differences in x direction
            int rx = get_pixel(im, y, right, 0) - get_pixel(im, y, left, 0);
            int gx = get_pixel(im, y, right, 1) - get_pixel(im, y, left, 1);
            int bx = get_pixel(im, y, right, 2) - get_pixel(im, y, left, 2);

            // Differences in y direction
            int ry = get_pixel(im, down, x, 0) - get_pixel(im, up, x, 0);
            int gy = get_pixel(im, down, x, 1) - get_pixel(im, up, x, 1);
            int by = get_pixel(im, down, x, 2) - get_pixel(im, up, x, 2);

            // Compute gradient energies
            int dx2 = rx * rx + gx * gx + bx * bx;
            int dy2 = ry * ry + gy * gy + by * by;

            double energy = sqrt(dx2 + dy2);
            uint8_t scaled = (uint8_t)(energy / 10.0);

            // Set grayscale pixel in grad image
            set_pixel(*grad, y, x, scaled, scaled, scaled);
        }
    }
}


void dynamic_seam(struct rgb_img* grad, double** best_arr) {
    const size_t width = grad->width;
    const size_t height = grad->height;
    *best_arr = malloc(height * width * sizeof(double));
    for (int x = 0; x < width; x++) {
        (*best_arr)[x] = get_pixel(grad, 0, x, 0);
    }

    for (int y = 1; y < height; y++) {
        const size_t idx = width * (y - 1);
        for (int x = 0; x < width; x++) {
            double min = DBL_MAX;
            for (int i = x - 1; i <= x + 1; i++) {
                if (i >= 0 && i < width && (*best_arr)[idx + i] < min) {
                    min = (*best_arr)[idx + i];
                }
            }
            (*best_arr)[width * y + x] = get_pixel(grad, y, x, 0) + min;
        }
    }
}

void recover_path(double* best, int height, int width, int** path) {
    *path = malloc(height * sizeof(int));
    double min = DBL_MAX;
    int bottom_idx = (height - 1) * width;
    for (int x = 0; x < width; x++) {
        if (best[bottom_idx+x] < min) {
            min = best[bottom_idx+x];
            (*path)[height-1] = x;
        }
    }
    for (int y = height - 2; y >= 0 ; y--) {
        const int idx = width * y;
        min = DBL_MAX;
        int x = (*path)[y + 1]; // x-val of the min in the previous row
        for (int i = x - 1; i <= x + 1; i++) {
            if (i >= 0 && i < width && best[idx + i] < min) {
                min = best[idx + i];
                (*path)[y] = i;
            }
        }
    }
}

void remove_seam(struct rgb_img* src, struct rgb_img** dest, int* path) {
    create_img(dest, src->height, src->width - 1);
    for (int y = 0; y < src->height; y++) {
        int remove_x = path[y];
        for (int x = 0; x < src->width - 1; x++) {
            int src_x = x;
            if (x >= remove_x) {
                src_x = x + 1;
            }
            set_pixel(*dest, y, x, get_pixel(src, y, src_x, 0),
                get_pixel(src, y, src_x, 1),get_pixel(src, y, src_x, 2));
        }
    }
}

