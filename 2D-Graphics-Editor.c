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
    char type;   // 'R' rectangle, 'F' filled rect, 'L' line, 'C' circle, 'T' triangle, 'D' diamond
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

// Draw line (Handles horizontal, vertical, and diagonal lines using Bresenham's algorithm)
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while(1) {
        if(x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            canvas[y1][x1] = '*';
        }
        if(x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if(e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if(e2 < dx) {
            err += dx;
            y1 += sy;
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

// Draw diamond shape centered at (cx, cy) with radius r
void drawDiamond(int cx, int cy, int r) {
    for(int y = -r; y <= r; y++) {
        for(int x = -r; x <= r; x++) {
            if(abs(x) + abs(y) <= r) {
                int px = cx + x;
                int py = cy + y;
                if(px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    canvas[py][px] = '*';
                }
            }
        }
    }
}

// Draw filled rectangle
void drawFilledRectangle(int x, int y, int w, int h) {
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
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
        else if(o.type == 'D') drawDiamond(o.x, o.y, o.w);
        else if(o.type == 'F') drawFilledRectangle(o.x, o.y, o.w, o.h);
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

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getIntInput(const char* prompt) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            clearInputBuffer();
            return value;
        } else {
            printf("Invalid input. Please enter an integer.\n");
            clearInputBuffer();
        }
    }
}

char getCharInput(const char* prompt) {
    char value;
    while (1) {
        printf("%s", prompt);
        if (scanf(" %c", &value) == 1) {
            clearInputBuffer();
            return value;
        } else {
            printf("Invalid input. Please enter a character.\n");
            clearInputBuffer();
        }
    }
}

void printObjectsList() {
    printf("\n--- Current Objects (%d/%d) ---\n", objectCount, MAX_OBJECTS);
    if (objectCount == 0) {
        printf("No objects on canvas.\n");
        return;
    }
    for (int i = 0; i < objectCount; i++) {
        Object o = objects[i];
        printf("[%d] ", i);
        if (o.type == 'R') {
            printf("Rectangle at (%d, %d), Width: %d, Height: %d\n", o.x, o.y, o.w, o.h);
        } else if (o.type == 'F') {
            printf("Filled Rectangle at (%d, %d), Width: %d, Height: %d\n", o.x, o.y, o.w, o.h);
        } else if (o.type == 'L') {
            printf("Line from (%d, %d) to (%d, %d)\n", o.x, o.y, o.w, o.h);
        } else if (o.type == 'C') {
            printf("Circle at (%d, %d), Radius: %d\n", o.x, o.y, o.w);
        } else if (o.type == 'T') {
            printf("Triangle at (%d, %d), Size: %d\n", o.x, o.y, o.w);
        } else if (o.type == 'D') {
            printf("Diamond at (%d, %d), Radius: %d\n", o.x, o.y, o.w);
        } else {
            printf("Unknown shape '%c' at (%d, %d)\n", o.type, o.x, o.y);
        }
    }
}

int main() {
    // Pipeline configuration
    initCanvas();

    // Add baseline elements for quick visualization
    addObject('R', 2, 2, 10, 5);   // Rectangle
    addObject('L', 0, 0, 15, 0);   // Horizontal line
    addObject('C', 25, 10, 4, 0);  // Circle
    addObject('T', 40, 5, 5, 0);   // Triangle

    int choice;
    while (1) {
        printf("\n====================================\n");
        printf("        2D GRAPHICS EDITOR CLI      \n");
        printf("====================================\n");
        displayCanvas();
        printObjectsList();
        
        printf("\nMenu Options:\n");
        printf("1. Add a Shape\n");
        printf("2. Modify a Shape\n");
        printf("3. Delete a Shape\n");
        printf("4. Clear Canvas (Delete All)\n");
        printf("5. Display / Refresh Canvas\n");
        printf("6. Exit\n");
        
        choice = getIntInput("Select an option (1-6): ");
        
        if (choice == 1) {
            if (objectCount >= MAX_OBJECTS) {
                printf("Error: Canvas is full. Delete some shapes first.\n");
                continue;
            }
            printf("\nSelect Shape Type:\n");
            printf("  R - Rectangle\n");
            printf("  F - Filled Rectangle\n");
            printf("  L - Line\n");
            printf("  C - Circle\n");
            printf("  T - Triangle\n");
            printf("  D - Diamond\n");
            
            char type = getCharInput("Enter shape type: ");
            // Convert to uppercase
            if (type >= 'a' && type <= 'z') type -= 32;
            
            if (type != 'R' && type != 'F' && type != 'L' && type != 'C' && type != 'T' && type != 'D') {
                printf("Error: Invalid shape type '%c'.\n", type);
                continue;
            }
            
            int x, y, w = 0, h = 0;
            if (type == 'R' || type == 'F') {
                x = getIntInput("Enter start X coordinate (0-49): ");
                y = getIntInput("Enter start Y coordinate (0-19): ");
                w = getIntInput("Enter width: ");
                h = getIntInput("Enter height: ");
            } else if (type == 'L') {
                x = getIntInput("Enter start X1 coordinate (0-49): ");
                y = getIntInput("Enter start Y1 coordinate (0-19): ");
                w = getIntInput("Enter end X2 coordinate (0-49): ");
                h = getIntInput("Enter end Y2 coordinate (0-19): ");
            } else if (type == 'C') {
                x = getIntInput("Enter center X coordinate (0-49): ");
                y = getIntInput("Enter center Y coordinate (0-19): ");
                w = getIntInput("Enter radius: ");
            } else if (type == 'T') {
                x = getIntInput("Enter top-left X coordinate (0-49): ");
                y = getIntInput("Enter top-left Y coordinate (0-19): ");
                w = getIntInput("Enter size: ");
            } else if (type == 'D') {
                x = getIntInput("Enter center X coordinate (0-49): ");
                y = getIntInput("Enter center Y coordinate (0-19): ");
                w = getIntInput("Enter radius: ");
            }
            
            addObject(type, x, y, w, h);
            printf("Shape added successfully!\n");
            
        } else if (choice == 2) {
            if (objectCount == 0) {
                printf("No shapes to modify.\n");
                continue;
            }
            int index = getIntInput("Enter index of the shape to modify: ");
            if (index < 0 || index >= objectCount) {
                printf("Error: Invalid index.\n");
                continue;
            }
            Object o = objects[index];
            int x, y, w = 0, h = 0;
            printf("Modifying shape '%c' at index %d:\n", o.type, index);
            if (o.type == 'R' || o.type == 'F') {
                x = getIntInput("Enter new start X: ");
                y = getIntInput("Enter new start Y: ");
                w = getIntInput("Enter new width: ");
                h = getIntInput("Enter new height: ");
            } else if (o.type == 'L') {
                x = getIntInput("Enter new X1: ");
                y = getIntInput("Enter new Y1: ");
                w = getIntInput("Enter new X2: ");
                h = getIntInput("Enter new Y2: ");
            } else if (o.type == 'C') {
                x = getIntInput("Enter new center X: ");
                y = getIntInput("Enter new center Y: ");
                w = getIntInput("Enter new radius: ");
            } else if (o.type == 'T') {
                x = getIntInput("Enter new top-left X: ");
                y = getIntInput("Enter new top-left Y: ");
                w = getIntInput("Enter new size: ");
            } else if (o.type == 'D') {
                x = getIntInput("Enter new center X: ");
                y = getIntInput("Enter new center Y: ");
                w = getIntInput("Enter new radius: ");
            }
            
            modifyObject(index, x, y, w, h);
            printf("Shape modified successfully!\n");
            
        } else if (choice == 3) {
            if (objectCount == 0) {
                printf("No shapes to delete.\n");
                continue;
            }
            int index = getIntInput("Enter index of the shape to delete: ");
            if (index < 0 || index >= objectCount) {
                printf("Error: Invalid index.\n");
                continue;
            }
            deleteObject(index);
            printf("Shape deleted successfully!\n");
            
        } else if (choice == 4) {
            while (objectCount > 0) {
                deleteObject(0);
            }
            printf("Canvas cleared!\n");
            
        } else if (choice == 5) {
            // Canvas is automatically refreshed at the beginning of the loop
            printf("Canvas refreshed!\n");
            
        } else if (choice == 6) {
            printf("Exiting 2D Graphics Editor. Goodbye!\n");
            break;
        } else {
            printf("Invalid choice. Please select a number from 1 to 6.\n");
        }
    }
    return 0;
}
