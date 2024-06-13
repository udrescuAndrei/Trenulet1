#include "glos.h"
#include "gl.h"
#include "glu.h"
#include "glaux.h"
#include "math.h"
#include "glut.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

GLuint textureId1;
GLuint textureId2;
float trainPos = 0.0;
float sphereAngle = 0.0;  

GLfloat trackCurve1[][3] = {
    {-5.0, -0.5, 0.0},
    {-4.0, -0.5, -1.0},
    {-2.0, -0.5, 1.0},
    {-1.0, -0.5, 0.0}
};

GLfloat trackCurve2[][3] = {
    {-5.0, -0.5, 1.0},
    {-4.0, -0.5, 0.0},
    {-2.0, -0.5, -1.0},
    {-1.0, -0.5, 1.0}
};

float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
float cameraDistance = 10.0f;

void bezier(GLfloat cp[][3], GLint n, GLfloat t, GLfloat result[3]) {
    GLfloat temp[4][3];
    GLint i, j;

    for (i = 0; i <= n; i++) {
        for (j = 0; j < 3; j++) {
            temp[i][j] = cp[i][j];
        }
    }

    for (i = 1; i <= n; i++) {
        for (j = 0; j < n - i; j++) {
            temp[j][0] = (1 - t) * temp[j][0] + t * temp[j + 1][0];
            temp[j][1] = (1 - t) * temp[j][1] + t * temp[j + 1][1];
            temp[j][2] = (1 - t) * temp[j][2] + t * temp[j + 1][2];
        }
    }

    for (j = 0; j < 3; j++) {
        result[j] = temp[0][j];
    }
}

void drawSingleTrack(GLfloat trackCurve[][3], GLint numControlPoints) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 100; i++) {
        GLfloat t = (GLfloat)i / 100.0;
        GLfloat p[3];
        bezier(trackCurve, numControlPoints - 1, t, p);
        glVertex3fv(p);
    }
    glEnd();
}

void drawTracks() {
    glColor3f(0.5, 0.5, 0.5);
    drawSingleTrack(trackCurve1, 4);
    drawSingleTrack(trackCurve2, 4);
}

GLuint loadTexture(const char* filename) {
    GLuint textureId = 0;
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(filename);

    if (pTextureImage != nullptr) {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY,
            0, GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);
        cout << "Loaded texture: " << filename << endl;
    }
    else {
        cerr << "Failed to load texture: " << filename << endl;
    }

    if (pTextureImage) {
        if (pTextureImage->data) {
            free(pTextureImage->data);
        }
        free(pTextureImage);
    }

    return textureId;
}

void myInit() {
    glClearColor(0.5, 0.8, 1.0, 1.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 2.0, 5.0, 5.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    
    //textureId1 = loadTexture("covor.bmp");
    //textureId2 = loadTexture("parchet.bmp");

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float size) {
    glutSolidCube(size);
}

void drawCylinder(float radius, float height) {
    GLUquadric* quadric = gluNewQuadric();
    gluCylinder(quadric, radius, radius, height, 32, 32);
    gluDeleteQuadric(quadric);
}

void drawTrain() {
    GLfloat mat_diffuse2[] = { 0.4, 0.8, 2.0, 1.0 };
    GLfloat mat_specular2[] = { 1.0, 1.0, 0.5, 1.0 };
    GLfloat mat_shininess2[] = { 50.0 };
    GLfloat mat_diffuse3[] = { 0.1, 0.8, 0.3, 1.0 };
    GLfloat mat_specular3[] = { 1.0, 1.0, 0.5, 1.0 };
    GLfloat mat_shininess3[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular3);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess3);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glScalef(3.0, 1.0, 1.0);
    drawCube(1.0);
    glPopMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);

    glPushMatrix();
    glColor3f(0.8, 0.8, 0.8);
    glTranslatef(-1.0, 0.5, 0.0);
    glScalef(1.0, 1.0, 1.0);
    drawCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(1.2, 0.50, 0.0);
    glRotatef(-90, 1, 0, 0);
    drawCylinder(0.15, 0.7);
    glPopMatrix();

    glColor3f(0.2, 0.2, 0.2);
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glPushMatrix();
            glTranslatef(1.2 * i, -0.5, 0.5 * j);
            glRotatef(90, 0, 1, 0);
            glTranslatef(0.0, 0.0, -0.3);
            drawCylinder(0.2, 0.6);
            glPopMatrix();
        }
    }

    
    glPushMatrix();
    glTranslatef(2.0 * cos(sphereAngle), 0.5, 1.0 * sin(sphereAngle));
    glColor3f(0.0, 0.0, 1.0); 
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -cameraDistance);
    glRotatef(cameraAngleY, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleX, 0.0f, 1.0f, 0.0f);

    GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };

    GLfloat mat_diffuse1[] = { 1.0, 0.0, 0.8, 1.0 };
    GLfloat mat_specular1[] = { 3.0, 1.0, 5.0, 1.0 };
    GLfloat mat_shininess1[] = { 40.0 };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glPushMatrix();
    glTranslatef(trainPos, 0.0, 0.0);
    drawTrain();
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);

    drawTracks();

    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-3.5, -3.5);
    std::string studentName = "UDRESCU ANDREI ALEXANDRU";
    for (char c : studentName) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glutSwapBuffers();

    glBindTexture(GL_TEXTURE_2D, textureId1);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0, 0.0); glVertex3f(-2.5, -1.0, 0.0);
        glTexCoord2f(0.0, 2.0); glVertex3f(-2.5, 1.0, 0.0);
        glTexCoord2f(2.0, 2.0); glVertex3f(-0.5, 1.0, 0.0);
        glTexCoord2f(2.0, 0.0); glVertex3f(-0.5, -1.0, 0.0);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureId2);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -1.0, 0.0);
        glTexCoord2f(0.0, 2.0); glVertex3f(0.5, 1.0, 0.0);
        glTexCoord2f(2.0, 2.0); glVertex3f(2.5, 1.0, 0);
        glTexCoord2f(2.0, 0.0); glVertex3f(2.5, -1.0, 0);
    }
    glEnd();

    glFlush();
}

void update(int value) {
    trainPos += 0.01;
    if (trainPos > 5.0) trainPos = -5.0;
    sphereAngle += 0.05;  
    if (sphereAngle > 360.0) sphereAngle -= 360.0;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        trainPos -= 0.1;
        break;
    case 'd':
        trainPos += 0.1;
        break;
    case 'w':
        cameraDistance -= 0.5;
        if (cameraDistance < 2.0) cameraDistance = 2.0;
        break;
    case 's':
        cameraDistance += 0.5;
        break;
    case 'q':
        cameraAngleX -= 5.0;
        break;
    case 'e':
        cameraAngleX += 5.0;
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        cameraAngleY -= 5.0;
        break;
    case GLUT_KEY_DOWN:
        cameraAngleY += 5.0;
        break;
    case GLUT_KEY_LEFT:
        cameraAngleX -= 5.0;
        break;
    case GLUT_KEY_RIGHT:
        cameraAngleX += 5.0;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Train with Lighting and Rotating Sphere in OpenGL");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    myInit();
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
