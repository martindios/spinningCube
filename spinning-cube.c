#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define WIDTH 160 // Width of the screen
#define HEIGHT 44 // Height of the screen

float rotationX, rotationY, rotationZ; // Rotation angles for the cube

float cubeSize = 20; // Half of the cube's width
float zBuffer[WIDTH * HEIGHT]; // Z-screenBuffer to handle depth for each pixel
char screenBuffer[WIDTH * HEIGHT]; // Character screenBuffer for the screen
int backgroundChar = ' '; // Background ASCII character
int viewerDistance = 100; // Distance of the viewer from the cube
float horizontalOffSet; // Horizontal offset for centering the cube
float perspectiveScale = 40; // Perspective scaling factor

float rotationSpeed = 0.85; // Increment speed for cube rendering

float xProjected, yProjected, zProjected;
float inverseDepth;
int pixelX, pixelY; // Screen pixel cordinates
int bufferIndex; // Index in the screen buffer

const char* colors[] = {
    "\033[1;31m",  // Red
    "\033[1;32m",  // Green
    "\033[1;33m",  // Yellow
    "\033[1;34m",  // Blue
    "\033[1;35m",  // Magenta
    "\033[1;36m"   // Cyan
};

/* Function prototypes */
float calculateX(int i, int j, int k);
float calculateY(int i, int j, int k);
float calculateZ(int i, int j, int k);
void renderFace(float cubeX, float cubeY, float cubeZ, int ch);

int main(int argc, char** argv) {
    printf("\x1b[2J");  // Clear terminal screen

    while(1) {
        // Reset the buffers for each frame
        memset(screenBuffer, backgroundChar, WIDTH * HEIGHT);
        memset(zBuffer, 0, WIDTH * HEIGHT * 4);

        // Set the horizontal offset to center the cube
        horizontalOffSet = -2 * cubeSize;

        // Loop through the cube's vertices and render each face
        for(float cubeX = -cubeSize; cubeX < cubeSize; cubeX += rotationSpeed) {
            for(float cubeY = -cubeSize; cubeY < cubeSize; cubeY += rotationSpeed) {
                for(float cubeZ = -cubeSize; cubeZ < cubeSize; cubeZ++) {
                    // Render each face of the cube with different ASCII characters
                    renderFace(cubeX, cubeY, -cubeSize, '@'); // Front face
                    renderFace(cubeSize, cubeY, cubeZ, '$'); // Right face
                    renderFace(-cubeSize, cubeY, -cubeX, '~'); // Left face
                    renderFace(-cubeX, cubeY, cubeSize, '#'); // Back face
                    renderFace(cubeX, -cubeSize, -cubeY, ';'); // Bottom face
                    renderFace(cubeX, cubeSize, cubeY, '+'); // Top face

                }
            }
        }
        printf("\x1b[H"); // Move the cursor to the top-left corner of the terminal

        /* Print the buffer with colors for each face */
        for(int i = 0; i < WIDTH * HEIGHT; i++) {
            switch(screenBuffer[i]) {
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
            putchar(i % WIDTH ? screenBuffer[i] : '\n');
            printf("\033[0m"); // Reset the color
        }

        /* Display rotation angles */
        printf("\nRotation angles:\n");
        printf("rotationX: %.2f  |  rotationY: %.2f  |  rotationZ: %.2f\n", rotationX, rotationY, rotationZ);

        /* Increment rotation angles for the next frame */
        rotationX += 0.05;
        rotationY += 0.05;
        rotationZ += 0.01;
        usleep(16000); // Delay to control frame rate (approximately 60 FPS)
    }
    return 0;
}

/* Calculate the projected X coordinate for a vertex */
float calculateX(int i, int j, int k) {
    return j * sin(rotationX) * sin(rotationY) * cos(rotationZ) - k * cos(rotationX) * sin(rotationY) * cos(rotationZ) +
         j * cos(rotationX) * sin(rotationZ) + k * sin(rotationX) * sin(rotationZ) + i * cos(rotationY) * cos(rotationZ);
}

/* Calculate the projected Y coordinate for a vertex */
float calculateY(int i, int j, int k) {
    return j * cos(rotationX) * cos(rotationZ) + k * sin(rotationX) * cos(rotationZ) -
         j * sin(rotationX) * sin(rotationY) * sin(rotationZ) + k * cos(rotationX) * sin(rotationY) * sin(rotationZ) -
         i * cos(rotationY) * sin(rotationZ);
}

/* Calculate the projected Z coordinate for a vertex */
float calculateZ(int i, int j, int k) {
    return k * cos(rotationX) * cos(rotationY) - j * sin(rotationX) * cos(rotationY) + i * sin(rotationY);
}

void renderFace(float cubeX, float cubeY, float cubeZ, int ch) {
    /* Calculate the projected coordinates */
    xProjected = calculateX(cubeX, cubeY, cubeZ);
    yProjected = calculateY(cubeX, cubeY, cubeZ);
    zProjected = calculateZ(cubeX, cubeY, cubeZ) + viewerDistance;

    inverseDepth = 1.0 / zProjected;

    /* Map 3D coordinates to 2D screen coordinates */
    pixelX = (int) (WIDTH / 2 + horizontalOffSet + perspectiveScale * inverseDepth * xProjected * 2);
    pixelY = (int) (HEIGHT / 2 + perspectiveScale * inverseDepth * yProjected);

    /* Calculate the buffer index */
    bufferIndex = pixelX + pixelY * WIDTH;

    /* Update the buffers if the current pixel is closer */
    if(bufferIndex >= 0 && bufferIndex < WIDTH * HEIGHT && inverseDepth > zBuffer[bufferIndex]) {
        zBuffer[bufferIndex] = inverseDepth;
        screenBuffer[bufferIndex] = ch;
    }
}
