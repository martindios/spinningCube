#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define WIDTH 160 
#define HEIGHT 44

/*Global variables*/
float A, B, C;

float cubeWidth = 20;
float zBuffer[WIDTH * HEIGHT];
char buffer[WIDTH * HEIGHT];
int backgroundASCII = ' ';
int distance = 100;
float horizontalOffSet;
float K1 = 40;

float incrementSpeed = 0.85;

float x, y, z;
float ooz;
int xp, yp;
int idx;

const char* colors[] = {
    "\033[31m",
    "\033[32m", 
    "\033[33m", 
    "\033[34m", 
    "\033[35m", 
    "\033[36m"};

float calculateX(int i, int j, int k);
float calculateY(int i, int j, int k);
float calculateZ(int i, int j, int k);

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch);

int main(int argc, char** argv) {
    printf("\x1b[2J");
    while(1) {
        memset(buffer, backgroundASCII, WIDTH * HEIGHT);
        memset(zBuffer, 0, WIDTH * HEIGHT * 4);
        horizontalOffSet = -2 * cubeWidth;

        for(float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for(float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                for(float cubeZ = -cubeWidth; cubeZ < cubeWidth; cubeZ++) {
                    calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
                    calculateForSurface(cubeWidth, cubeY, cubeZ, '$');
                    calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
                    calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
                    calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
                    calculateForSurface(cubeX, cubeWidth, cubeY, '+');

                }
            }
        }
        printf("\x1b[H");

        for(int i = 0; i < WIDTH * HEIGHT; i++) {
            switch(buffer[i]) {
                case '@':
                    printf("%s", colors[0]);
                    break;
                case '$':
                    printf("%s", colors[1]);
                    break;
                case '~':
                    printf("%s", colors[2]);
                    break;
                case '#':
                    printf("%s", colors[3]);
                    break;
                case ';':
                    printf("%s", colors[4]);
                    break;
                case '+':
                    printf("%s", colors[5]);
                    break;
            }
            putchar(i % WIDTH ? buffer[i] : 10);
            printf("\033[0m");
        }


        printf("\nRotation angles:\n");
        printf("A: %.2f  |  B: %.2f  |  C: %.2f\n", A, B, C);

        A += 0.05;
        B += 0.05;
        C += 0.01;
        usleep(16000);
    }
    return 0;
}

float calculateX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
    x = calculateX(cubeX, cubeY, cubeZ);
    y = calculateY(cubeX, cubeY, cubeZ);
    z = calculateZ(cubeX, cubeY, cubeZ) + distance;

    ooz = 1.0 / z;

    xp = (int) (WIDTH / 2 + horizontalOffSet + K1 * ooz * x * 2);
    yp = (int) (HEIGHT / 2 + K1 * ooz * y);

    idx = xp + yp * WIDTH;

    if(idx >= 0 && idx < WIDTH * HEIGHT && ooz > zBuffer[idx]) {
        zBuffer[idx] = ooz;
        buffer[idx] = ch;
    }
}
