#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 50
#define HEIGHT 20
#define MAX_OBJECTS 100

// Canvas memory (Framebuffer)
char canvas[HEIGHT][WIDTH];

// Object structure
typedef struct {
    char type;   // 'R' rectangle, 'C' circle, 'L' line, 'T' triangle
    int x, y;    // position / start coordinate
    int w, h;    // width/height, radius, or end coordinate
} Object;

Object objects[MAX_OBJECTS];
int objectCount = 0;

// Initialize canvas with background character
void initCanvas() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            canvas[i][j] = '_';
        }
    }
}

// Display canvas to the terminal
void displayCanvas() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            printf("%c ", canvas[i][j]);
        }
        printf("\n");
    }
}

// Draw rectangle with boundary clipping
void drawRectangle(int x, int y, int w, int h) {
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    // Draw horizontal borders (Top & Bottom)
    for(int i = x; i <= x2; i++) {
        if(i >= 0 && i < WIDTH) {
            if(y >= 0 && y < HEIGHT)   canvas[y][i] = '*';
            if(y2 >= 0 && y2 < HEIGHT) canvas[y2][i] = '*';
        }
    }
    // Draw vertical borders (Left & Right)
    for(int j = y; j <= y2; j++) {
        if(j >= 0 && j < HEIGHT) {
            if(x >= 0 && x < WIDTH)   canvas[j][x] = '*';
            if(x2 >= 0 && x2 < WIDTH) canvas[j][x2] = '*';
        }
    }
}

// Draw line (Handles bidirectional horizontal/vertical lines)
void drawLine(int x1, int y1, int x2, int y2) {
    if(y1 == y2) { // Horizontal line
        int startX = (x1 < x2) ? x1 : x2;
        int endX = (x1 < x2) ? x2 : x1;
        if(y1 >= 0 && y1 < HEIGHT) {
            for(int i = startX; i <= endX; i++) {
                if(i >= 0 && i < WIDTH) canvas[y1][i] = '*';
            }
        }
    } else if(x1 == x2) { // Vertical line
        int startY = (y1 < y2) ? y1 : y2;
        int endY = (y1 < y2) ? y2 : y1;
        if(x1 >= 0 && x1 < WIDTH) {
            for(int j = startY; j <= endY; j++) {
                if(j >= 0 && j < HEIGHT) canvas[j][x1] = '*';
            }
        }
    }
}

// Draw circle using algebraic distance approximation
void drawCircle(int cx, int cy, int r) {
    for(int y = -r; y <= r; y++) {
        for(int x = -r; x <= r; x++) {
            // Using a slight tolerance factor (+1) to make the text-circle look full
            if(x*x + y*y <= r*r + 1) {
                int px = cx + x;
                int py = cy + y;
                if(px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    canvas[py][px] = '*';
                }
            }
        }
    }
}

// Draw right-angled triangle
void drawTriangle(int x, int y, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j <= i; j++) {
            int px = x + j;
            int py = y + i;
            if(px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                canvas[py][px] = '*';
            }
        }
    }
}

// Clear canvas and redraw the entire scene graph hierarchy
void redrawObjects() {
    initCanvas();
    for(int i = 0; i < objectCount; i++) {
        Object o = objects[i];
        if(o.type == 'R')      drawRectangle(o.x, o.y, o.w, o.h);
        else if(o.type == 'L') drawLine(o.x, o.y, o.w, o.h);
        else if(o.type == 'C') drawCircle(o.x, o.y, o.w);
        else if(o.type == 'T') drawTriangle(o.x, o.y, o.w);
    }
}

// Add object safely to scene array
void addObject(char type, int x, int y, int w, int h) {
    if(objectCount >= MAX_OBJECTS) return;
    
    objects[objectCount].type = type;
    objects[objectCount].x = x;
    objects[objectCount].y = y;
    objects[objectCount].w = w;
    objects[objectCount].h = h;
    objectCount++;
    
    redrawObjects();
}

// Delete object by index shifts elements left
void deleteObject(int index) {
    if(index < 0 || index >= objectCount) return;
    for(int i = index; i < objectCount - 1; i++) {
        objects[i] = objects[i + 1];
    }
    objectCount--;
    redrawObjects();
}

// Modify object values at runtime
void modifyObject(int index, int x, int y, int w, int h) {
    if(index < 0 || index >= objectCount) return;
    objects[index].x = x;
    objects[index].y = y;
    objects[index].w = w;
    objects[index].h = h;
    redrawObjects();
}

int main() {
    // Pipeline configuration
    initCanvas();

    // 1. Add baseline elements
    addObject('R', 2, 2, 10, 5);   // Rectangle at (2,2) with W=10, H=5
    addObject('L', 0, 0, 15, 0);   // Horizontal line from (0,0) to (15,0)
    addObject('C', 25, 10, 4, 0);  // Circle at (25,10) with Radius=4
    addObject('T', 40, 5, 5, 0);   // Triangle at (40,5) with Size=5

    // Render current scene layout
    displayCanvas();

    return 0;
}
