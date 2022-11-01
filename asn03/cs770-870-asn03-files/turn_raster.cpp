#include <iostream>

typedef unsigned char pixel;
using namespace std;

void turn_image(pixel **image, int n) {
    // image: a 2D array of bytes.
    // To access the pixel at row y, column x, use image[y][x].
    pixel now;
    int x = n/2;
    int y = n-1;

    for( int i = 0; i < 3; i++){
    for (int xx = 0; xx < n / 2; xx++) {
        for (int yy = xx; yy < n - xx - 1; yy++) {
            
            pixel temp = image[xx][yy];
 
            image[xx][yy] = image[yy][n - 1 - xx];
            image[yy][n - 1 - xx] = image[n - 1 - xx][n - 1 - yy];
            image[n - 1 - xx][n - 1 - yy] = image[n - 1 - yy][xx];
 
            image[n - 1 - yy][xx] = temp;
        }
    }
    }

}

void print_image(pixel **image, int n) {
    for (int y=n-1; y >=0; y--) {
        for (int x=0; x < n; x++)
            cout << image[y][x] << " ";
        cout << "\n";
    }
}

int main(int argc, char *argv[]) {
    // MAIN PROGRAM: Do not change.
    pixel **image;
    int n = 7;
    image = new pixel*[n];
    for (int y=0; y < n; y++) {
        image[y] = new pixel[n];
        for (int x=0; x < n; x++)
            image[y][x] = '.';
    }
    for (int y=0; y<n; y++)
        image[y][0] = '*';
    for (int x=0; x<n; x++) {
        image[0][x] = '*';
        image[n-1][x] = '*';
    }
    for (int x=0; x<n/2; x++)
        image[n/2][x] = '*';

    cout << "BEFORE ROTATION:\n";
    print_image(image, n);

    turn_image(image, n);

    cout << "AFTER ROTATION:\n";
    print_image(image, n);
}
