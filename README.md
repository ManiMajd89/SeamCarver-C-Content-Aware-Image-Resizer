# 🖼️ Seam Carving in C

This repository contains an implementation of a **content-aware image resizing algorithm** known as **seam carving**, written in C. Seam carving intelligently reduces image width by removing vertical seams (paths of low-importance pixels), preserving the most important content (e.g., objects and edges) rather than simply cropping or scaling the image.

---

## Overview

Seam carving is a content-aware image resizing technique introduced in 2007 that reduces image dimensions while preserving important visual features. Unlike traditional methods like cropping or scaling, seam carving works by identifying and removing **low-energy vertical seams**—paths of least visual importance that run from the top to the bottom of the image.

This implementation processes raw RGB images stored in `.bin` format and supports multiple iterations of seam removal to progressively reduce image width while maintaining perceptual quality.

---

## Key Concepts

- **Dual-gradient energy function**: Measures the importance of each pixel using local color gradient magnitude.
- **Dynamic programming**: Identifies the vertical seam with the minimum total energy.
- **Seam removal**: Modifies the image by removing one pixel per row along the identified seam.
- **Image preservation**: Avoids high-energy regions (edges, objects), preserving critical content.

---

## Implementation Details

### 1. Energy Calculation

Function: `void calc_energy(struct rgb_img *im, struct rgb_img **grad);`

- Computes the dual-gradient energy for each pixel using differences in RGB values along x and y directions.
- For edge pixels, wraps around the image (modular indexing).
- Stores energy as grayscale pixel intensity (uint8_t).

---

### 2. Seam Cost Array

Function: `void dynamic_seam(struct rgb_img *grad, double **best_arr);`

- Builds a 2D array of minimal cumulative seam costs from top to bottom.
- Each cell stores the cost of reaching that pixel via the lowest energy path.

---

### 3. Seam Recovery

Function: `void recover_path(double *best, int height, int width, int **path);`

- Traverses the cost array from bottom to top to find the path of the minimum-energy vertical seam.
- Stores the x-index of the seam for each row in the array `path`.

---

### 4. Seam Removal

Function: `void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);`

- Creates a new image with width reduced by one.
- Copies all pixels except the ones along the seam.

---

### 5. Batch Seam Removal (Demo)

```c
struct rgb_img *im;
struct rgb_img *cur_im;
struct rgb_img *grad;
double *best;
int *path;

read_in_img(&im, "HJoceanSmall.bin");

for (int i = 0; i < 5; i++) {
    calc_energy(im, &grad);
    dynamic_seam(grad, &best);
    recover_path(best, grad->height, grad->width, &path);
    remove_seam(im, &cur_im, path);

    char filename[200];
    sprintf(filename, "img%d.bin", i);
    write_img(cur_im, filename);

    destroy_image(im);
    destroy_image(grad);
    free(best);
    free(path);
    im = cur_im;
}

destroy_image(im);
```


## 📽️ Seam Carving Demo

![Seam Carving Demo](images/seam_demo.gif)

---

## File Structure

```
.
├── seamcarving.c       # Main seam carving implementation
├── seamcarving.h       # Function declarations and struct interfaces
├── HJoceanSmall.bin    # Sample binary image input (raw RGB)
├── img0.bin → img4.bin # Output images after seam removals
├── README.md           # Project documentation
```

---

## How to Compile and Run

### 1. Compilation

```bash
gcc -Wall -std=c99 seamcarving.c main.c -o seamcarving -lm
```

> Make sure you have `seamcarving.h` and helper image functions available (`read_in_img`, `write_img`, `create_img`, etc.)

### 2. Execution

```bash
./seamcarving
```

> The output files `img0.bin`, `img1.bin`, ..., `imgN.bin` will be written to disk and can be viewed using an image visualizer that supports the `.bin` format or converted to PNG using a helper script.


---

## Acknowledgements

> **Assignment Design Credit**:  
> This project was originally designed by **Josh Hug** and later **ported to C by Michael Guerzhoy** for the ESC190H1F course at the University of Toronto.

> **Code Implementation**:  
> All code in this repository was written by the authors.

---

## Authors

**ESC190 Project**  
Work done by **Mani Majd** and **Aayush Mengane**  
📫 [LinkedIn – Mani Majd](https://www.linkedin.com/in/mani-majd)
