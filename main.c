#include <stdio.h>

typedef struct _pixel {
    unsigned short int r;
    unsigned short int g;
    unsigned short int b;
} Pixel;

typedef struct _image {
    unsigned short int pixel[512][512][3];
    unsigned int w;
    unsigned int h;
} Image;

int gray_scale_media(int red, int green, int blue) {
	int media = 0;
	media = red + green + blue;
	media /= 3;
	return media;
}

Image gray_scale(Image img) {
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
			int media = gray_scale_media(img.pixel[i][j][0], 
				                img.pixel[i][j][1],
				                img.pixel[i][j][2]);
			
			for (unsigned int k = 0; k < 3; ++k){
	            img.pixel[i][j][k] = media;
			}
        }
    }

    return img;
}

void blur(unsigned int h, unsigned short int pixel[512][512][3], int T, unsigned int w) {
    for (unsigned int i = 0; i < h; ++i) {
        for (unsigned int j = 0; j < w; ++j) {
            Pixel media = {0, 0, 0};

            int min_h = (h - 1 > i + T/2) ? i + T/2 : h - 1;
            int min_w = (w - 1 > j + T/2) ? j + T/2 : w - 1;
            for(int x = (0 > i - T/2 ? 0 : i - T/2); x <= min_h; ++x) {
                for(int y = (0 > j - T/2 ? 0 : j - T/2); y <= min_w; ++y) {
                    media.r += pixel[x][y][0];
                    media.g += pixel[x][y][1];
                    media.b += pixel[x][y][2];
                }
            }

            media.r /= T * T;
            media.g /= T * T;
            media.b /= T * T;

            pixel[i][j][0] = media.r;
            pixel[i][j][1] = media.g;
            pixel[i][j][2] = media.b;
        }
    }
}

Image rotate_90_right(Image img) {
    Image rotated;

    rotated.w = img.h;
    rotated.h = img.w;

    for (unsigned int i = 0, y = 0; i < rotated.h; ++i, ++y) {
        for (int j = rotated.w - 1, x = 0; j >= 0; --j, ++x) {
            for (unsigned int k = 0; k < 3; ++k) {
                rotated.pixel[i][j][k] = img.pixel[x][y][k];
            }
        }
    }

    return rotated;
}

void invert_colors(unsigned short int pixel[512][512][3],
                    unsigned int w, unsigned int h) {
    for (unsigned int i = 0; i < h; ++i) {
        for (unsigned int j = 0; j < w; ++j) {
            for (unsigned int k = 0; k < 3; ++k) {
                pixel[i][j][k] = 255 - pixel[i][j][k];
            }
        }
    }
}

Image cut_image(Image img, int x, int y, int w, int h) {
    Image cut;

    cut.w = w;
    cut.h = h;

    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            for (unsigned int k = 0; k < 3; ++k) {
                cut.pixel[i][j][k] = img.pixel[i + y][j + x][k];
            }
        }
    }

    return cut;
}

Image sepia(Image img) {
    for (unsigned int x = 0; x < img.h; ++x) {
        for (unsigned int j = 0; j < img.w; ++j) {
            unsigned short int pixel[3];
            for (unsigned int k = 0; k < 3; ++k) {
                pixel[k] = img.pixel[x][j][k];
            }

            int p =  pixel[0] * .393 + pixel[1] * .769 + pixel[2] * .189;
            int min_r = (255 >  p) ? p : 255;
            img.pixel[x][j][0] = min_r;

            p =  pixel[0] * .349 + pixel[1] * .686 + pixel[2] * .168;
            min_r = (255 >  p) ? p : 255;
            img.pixel[x][j][1] = min_r;

            p =  pixel[0] * .272 + pixel[1] * .534 + pixel[2] * .131;
            min_r = (255 >  p) ? p : 255;
            img.pixel[x][j][2] = min_r;
        }
    }
    
    return img;
}

Image mirror_image(Image img, int horizontal) {
    int w = img.w, h = img.h;

    if (horizontal == 1) w /= 2;
    else h /= 2;

    for (int i2 = 0; i2 < h; ++i2) {
        for (int j = 0; j < w; ++j) {
            int x = i2, y = j;

            if (horizontal == 1) y = img.w - 1 - j;
            else x = img.h - 1 - i2;

            Pixel aux1;
            aux1.r = img.pixel[i2][j][0];
            aux1.g = img.pixel[i2][j][1];
            aux1.b = img.pixel[i2][j][2];

            for (unsigned int k = 0; k < 3; ++k) {
                img.pixel[i2][j][k] = img.pixel[x][y][k];
            }

            img.pixel[x][y][0] = aux1.r;
            img.pixel[x][y][1] = aux1.g;
            img.pixel[x][y][2] = aux1.b;
        }
    }
    return img;
}

Image reader(Image img){
    // read all pixels of image
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            scanf("%hu %hu %hu", &img.pixel[i][j][0],
                                 &img.pixel[i][j][1],
                                 &img.pixel[i][j][2]);

        }
    }
    return img;
}

int main() {
    Image img;

    // read type of image
    char p3[4];
    scanf("%s", p3);

    // read width height and color of image
    int max_color;
    scanf("%u %u %d", &img.w, &img.h, &max_color);

    img = reader(img);

    int options;
    scanf("%d", &options);

    for(int i = 0; i < options; ++i) {
        int option;
        scanf("%d", &option);

        switch(option) {
            case 1: { 
                // Gray Scale
                img = gray_scale(img);
                break;
            }
            case 2: { 
                // Sepia filter
                img = sepia(img);
                break;
            }
            case 3: { 
                // Blur
                int tamanho = 0;
                scanf("%d", &tamanho);
                blur(img.h, img.pixel, tamanho, img.w);
                break;
            }
            case 4: { 
                // Rotation
                int how_many_rotations = 0;
                scanf("%d", &how_many_rotations);
                how_many_rotations %= 4;
                for (int j = 0; j < how_many_rotations; ++j) {
                    img = rotate_90_right(img);
                }
                break;
            }
            case 5: { 
                // Mirroring
                int horizontal = 0;
                scanf("%d", &horizontal);
                img = mirror_image(img,horizontal);
                break;
            }
            case 6: { 
                // Colors invertion
                invert_colors(img.pixel, img.w, img.h);
                break;
            }
            case 7: { 
                // Image cutting
                int x, y;
                scanf("%d %d", &x, &y);
                int w, h;
                scanf("%d %d", &w, &h);

                img = cut_image(img, x, y, w, h);
                break;
            }
        }

    }

    // print type of image
    printf("P3\n");
    // print width height and color of image
    printf("%u %u\n255\n", img.w, img.h);

    // print pixels of image
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            printf("%hu %hu %hu ", img.pixel[i][j][0],
                                   img.pixel[i][j][1],
                                   img.pixel[i][j][2]);

        }
        printf("\n");
    }
    return 0;
}
