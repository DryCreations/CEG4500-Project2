// Driver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <GL/glut.h>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

struct Point {
    uint8_t intensity;
    int x, y, z;
};

bool comparePoints(Point p1, Point p2) {
    GLdouble x1, y1, z1, x2, y2, z2;

    GLdouble current_modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, current_modelview);
    GLdouble current_projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, current_projection);
    GLint current_viewport[4];
    glGetIntegerv(GL_VIEWPORT, current_viewport);

    gluProject(p1.x, p1.y, p1.z, current_modelview, current_projection, current_viewport, &x1, &y1, &z1);
    gluProject(p2.x, p2.y, p2.z, current_modelview, current_projection, current_viewport, &x2, &y2, &z2);

    return z1 > z2;
}

class Slider {
private:
    bool active = false;
    float handleWidth;
    float min_x, max_x, min_y, max_y;
    float min_val, max_val, val;
    bool vertical;

public:
    Slider(float min_x, float max_x, float min_y, float max_y, float min_val, float max_val, float default_val, bool vertical) {
        this->min_x = min_x;
        this->max_x = max_x;
        this->min_y = min_y;
        this->max_y = max_y;
        this->min_val = min_val;
        this->max_val = max_val;
        this->val = default_val;
        if (vertical) {
            this->handleWidth = max_x - min_x;
        }
        else {
            this->handleWidth = max_y - min_y;
        }
       
        this->vertical = vertical;
    }

    float getVal() {
        return this->val;
    }

    void draw() {
        glColor3f(.5, .5, .5);

        glBegin(GL_POLYGON);
        glVertex2f(min_x, max_y);
        glVertex2f(max_x, max_y);
        glVertex2f(max_x, min_y);
        glVertex2f(min_x, min_y);
        glEnd();

        if (active) {
            glColor3f(1, .1, .1);
        }
        else {
            glColor3f(.5, 0, 0);

        }

        if (vertical) {
            float valy = (val - min_val) * (max_y - min_y - handleWidth) / (max_val - min_val) + min_y + handleWidth / 2;

            glBegin(GL_POLYGON);
            glVertex2f(max_x, valy - handleWidth / 2);
            glVertex2f(max_x, valy + handleWidth / 2);
            glVertex2f(min_x, valy + handleWidth / 2);
            glVertex2f(min_x, valy - handleWidth / 2);
            glEnd();
        }
        else {
            float valx = (val - min_val) * (max_x - min_x - handleWidth) / (max_val - min_val) + min_x + handleWidth / 2;

            glBegin(GL_POLYGON);
            glVertex2f(valx - handleWidth / 2, max_y);
            glVertex2f(valx + handleWidth / 2, max_y);
            glVertex2f(valx + handleWidth / 2, min_y);
            glVertex2f(valx - handleWidth / 2, min_y);
            glEnd();
        }
        
    }

    void updateVal(int mouse_x, int mouse_y) {
        if (vertical) {
            if (mouse_y < min_y + handleWidth / 2) {
                val = min_val;
            }
            else if (mouse_y > max_y - handleWidth / 2) {
                val = max_val;
            }
            else {
                val = (mouse_y - min_y - handleWidth / 2) * (max_val - min_val) / (max_y - min_y - handleWidth) + min_val;
            }
        }
        else {
            if (mouse_x < min_x + handleWidth / 2) {
                val = min_val;
            }
            else if (mouse_x > max_x - handleWidth / 2) {
                val = max_val;
            }
            else {
                val = (mouse_x - min_x - handleWidth / 2) * (max_val - min_val) / (max_x - min_x - handleWidth) + min_val;
            }
        }
        
    }

    bool inBounds(int x, int y) {
        return x >= this->min_x && x <= this->max_x && y >= this->min_y && y <= this->max_y;
    }

    void mouseClicked(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON) {
            if (active) {
                if (state == GLUT_UP) {
                    updateVal(x, y);
                    this->active = false;
                }
                else {
                    // shouldn't happen, but just to be safe?
                    if (this->inBounds(x, y)) {
                        updateVal(x, y);
                        this->active = true;
                    }
                }
            }
            else if (state == GLUT_DOWN) {
                if (this->inBounds(x, y)) {
                    updateVal(x, y);
                    this->active = true;
                }
            }
        }
    }

    void mouseDragged(int x, int y) {
        if (active) {
            updateVal(x, y);
        }
    }
};

int WIDTH = 1000, HEIGHT = 600;

Slider s1(17.0 * WIDTH / 32.0, 18.0 * WIDTH / 32.0, HEIGHT / 16.0, 7.0 * HEIGHT / 16.0, -180, 180, 0, true);
Slider s2(23.5 * WIDTH / 32.0, 24.5 * WIDTH / 32.0, HEIGHT / 16.0, 7.0 * HEIGHT / 16.0, -180, 180, 0, true);
Slider s3(30.0 * WIDTH / 32.0, 31.0 * WIDTH / 32.0, HEIGHT / 16.0, 7.0 * HEIGHT / 16.0, -180, 180, 0, true);
Slider s4(17.0 * WIDTH / 32.0, 31.0 * WIDTH / 32.0, 12 * HEIGHT / 16.0, 13 * HEIGHT / 16.0, .01, 5, 1, false);


typedef struct {
    const char* filename;
    int x_dimension;
    int y_dimension;
    int z_dimension;
} raw;

const raw BLOCK{ "block.raw", 34, 9, 54 };
const raw COLON{ "colon.raw", 204, 132, 260 };
const raw MONSTER{ "monster.raw", 87, 39, 62 };
const raw HEART{ "heart.raw", 289, 240, 267 };
const raw SKULL{ "skull.raw", 256, 256, 109 };
const raw HEAD{ "head.raw", 256, 256, 113 }; // Doesn't work, I think it uses 2 bytes for intensity


raw selected_raw = HEART;

uint8_t threshold = 0;
GLuint index;
float internal_diagonal;
vector<Point> points;

void createList(GLuint index, float angle_x, float angle_y, float angle_z) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float x_bound = 1, y_bound = 1;
    internal_diagonal = sqrtf(powf(selected_raw.x_dimension, 2) + powf(selected_raw.y_dimension, 2) + powf(selected_raw.z_dimension, 2));

    float ratio = (float)WIDTH / HEIGHT;

    if (ratio > 1) {
        x_bound *= ratio;
    }
    else {
        y_bound /= ratio;
    }
    glOrtho(-1, 2.0 * x_bound - 1.0, -y_bound, y_bound, 0, 3.0 );

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WIDTH, HEIGHT);
    glLoadIdentity();

    gluLookAt(0, 0, 2.0, 0, 0, 0, 0, 1, 0);

    glScalef(2 / internal_diagonal, 2 / internal_diagonal, 2 / internal_diagonal);

    glRotatef(angle_z, 0, 0, 1);
    glRotatef(angle_y, 0, 1, 0);
    glRotatef(angle_x, 1, 0, 0);

    glTranslatef(-selected_raw.x_dimension / 2, -selected_raw.y_dimension / 2, -selected_raw.z_dimension / 2);

    sort(points.begin(), points.end(), comparePoints);

    glNewList(index, GL_COMPILE);
    glPointSize(1);
    glBegin(GL_POINTS);
    for (auto const& p : points) {
        glColor4f(p.intensity / 255.0, 0, 1 - p.intensity / 255.0 , sqrtf(p.intensity / 255.0));
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    glEndList();
}

struct Corner {
    int x, y, z, index;
};

vector<Corner> corners;

bool compareCorners(Corner p1, Corner p2) {
    GLdouble x1, y1, z1, x2, y2, z2;

    GLdouble current_modelview[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, current_modelview);
    GLdouble current_projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, current_projection);
    GLint current_viewport[4];
    glGetIntegerv(GL_VIEWPORT, current_viewport);

    gluProject(p1.x, p1.y, p1.z, current_modelview, current_projection, current_viewport, &x1, &y1, &z1);
    gluProject(p2.x, p2.y, p2.z, current_modelview, current_projection, current_viewport, &x2, &y2, &z2);

    return z1 < z2;
}

void read_file(const char* filename, int x, int y, int z) {
    ifstream file (filename, ifstream::binary);
    if (file.is_open()) {

        int size = x * y * z;

        char * input = new char[size];
        file.seekg(0, ios::beg);
        file.read(input, size);

        index = glGenLists(8);
        
        for (int k = 0, p = 0; k < z; k++) {
            for (int j = 0; j < y; j++) {
                for (int i = 0; i < x; i++, p++) {
                    if ((uint8_t)input[p] > threshold) {
                        points.push_back({ (uint8_t)input[p], i, j, k });
                    }
                }
            }
        }

        createList(index, 45, 135, 0); // 0, 0, 0
       // cout << "loaded list 1/8" << endl;
        createList(index + 1, 135, 135, 45); // 0, 0, 1
        //cout << "loaded list 2/8" << endl;
        createList(index + 2, -45, 135, 0); // 0, 1, 0
       // cout << "loaded list 3/8" << endl;
        createList(index + 3, 45, 45, 0); // 0, 1, 1
       // cout << "loaded list 4/8" << endl;
        createList(index + 4, -135, -45, 0); // 1, 0, 0
       // cout << "loaded list 5/8" << endl;
        createList(index + 5, -45, -45, 0); // 1, 0, 1
       // cout << "loaded list 6/8" << endl;
        createList(index + 6, -45, -135, 180); // 1, 1, 0
       // cout << "loaded list 7/8" << endl;
        createList(index + 7, 45, -45, 0); // 1, 1, 1
        //cout << "loaded list 8/8" << endl;

        file.close();

        points.clear();
        delete[] input;
    }
    else {
        cout << "could not open file";
    }
    
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    
    int max_dimension = selected_raw.x_dimension;
    if (selected_raw.y_dimension > max_dimension) {
        max_dimension = selected_raw.y_dimension;
    }
    if (selected_raw.z_dimension > max_dimension) {
        max_dimension = selected_raw.z_dimension;
    }

    corners.push_back({ 0,0,0,0 });
    corners.push_back({ 0, 0, max_dimension,1 });
    corners.push_back({ 0, max_dimension, 0,2 });
    corners.push_back({ 0, max_dimension, max_dimension,3 });
    corners.push_back({ max_dimension, 0, 0,4 });
    corners.push_back({ max_dimension, 0, max_dimension,5 });
    corners.push_back({ max_dimension, max_dimension,0,6 });
    corners.push_back({ max_dimension, max_dimension, max_dimension,7 });

    read_file(selected_raw.filename, selected_raw.x_dimension, selected_raw.y_dimension, selected_raw.z_dimension);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float x_bound = 1, y_bound = 1;
    internal_diagonal = sqrtf(powf(selected_raw.x_dimension, 2) + powf(selected_raw.y_dimension, 2) + powf(selected_raw.z_dimension, 2));

    float ratio = (float)WIDTH / HEIGHT;

    if (ratio > 1) {
        x_bound *= ratio;
    }
    else {
        y_bound /= ratio;
    }
    glOrtho(-1, 2.0 * x_bound - 1.0, -y_bound, y_bound, 0, 3 * s4.getVal());

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WIDTH, HEIGHT);
    glLoadIdentity();

    gluLookAt(0, 0, 2 * s4.getVal(), 0, 0, 0, 0, 1, 0);

    glScalef(2 / internal_diagonal, 2 / internal_diagonal, 2 / internal_diagonal);
    
    glRotatef(s1.getVal(), 0, 0, 1);
    glRotatef(s2.getVal(), 0, 1, 0);
    glRotatef(s3.getVal(), 1, 0, 0);
    glScalef(s4.getVal(), s4.getVal(), s4.getVal());

    glTranslatef(-selected_raw.x_dimension / 2, -selected_raw.y_dimension / 2, -selected_raw.z_dimension / 2);

    sort(corners.begin(), corners.end(), compareCorners);

    glCallList(index + corners[0].index);

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WIDTH, HEIGHT);
    glLoadIdentity();
    s1.draw();
    s2.draw();
    s3.draw(); 
    s4.draw();

	glFlush();
}

void mouseClicked(int button, int state, int x, int y) {
    s1.mouseClicked(button, state, x, HEIGHT - y);
    s2.mouseClicked(button, state, x, HEIGHT - y);
    s3.mouseClicked(button, state, x, HEIGHT - y);
    s4.mouseClicked(button, state, x, HEIGHT - y);
    glutPostRedisplay();
}

void mouseDragged(int x, int y) {
    s1.mouseDragged(x, HEIGHT - y);
    s2.mouseDragged(x, HEIGHT - y);
    s3.mouseDragged(x, HEIGHT - y);
    s4.mouseDragged(x, HEIGHT - y);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Assignment 3");
	init();
	glutDisplayFunc(display);
    glutMouseFunc(mouseClicked);
    glutMotionFunc(mouseDragged);
	glutMainLoop();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
