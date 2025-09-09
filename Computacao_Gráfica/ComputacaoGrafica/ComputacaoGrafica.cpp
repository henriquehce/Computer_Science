#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <GL/freeglut.h>

using namespace std;

//  Estruturas globais
struct Face {
    int v[3], t[3], n[3];
};
vector<vector<float>> vertices, texcoords, normals;
vector<Face> faces;
unsigned int modelo;

// Transformações
float tx = 0, ty = 0, tz = -50;
float sx = 1, sy = 1, sz = 1;
float rx = 0, ry = 0, rz = 0;

// Controle de mouse
int lastMouseX, lastMouseY;
bool leftButtonDown = false, rightButtonDown = false;

// Cor do objeto
GLfloat cor_objeto[4] = {0.7f, 0.7f, 0.9f, 1.0f};

//  Funções auxiliares
void setMaterial(GLfloat r, GLfloat g, GLfloat b) {
    GLfloat ambient[]  = { r, g, b, 1.0f };
    GLfloat diffuse[]  = { r, g, b, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void loadObj(const string &fname) {
    ifstream arquivo(fname);
    if (!arquivo.is_open()) {
        cerr << "Arquivo não encontrado: " << fname << endl;
        exit(1);
    }

    string tipo;
    while (arquivo >> tipo) {
        if (tipo == "v") {
            float x, y, z; arquivo >> x >> y >> z;
            vertices.push_back({x, y, z});
        } else if (tipo == "vt") {
            float u, v; arquivo >> u >> v;
            texcoords.push_back({u, v});
        } else if (tipo == "vn") {
            float nx, ny, nz; arquivo >> nx >> ny >> nz;
            normals.push_back({nx, ny, nz});
        } else if (tipo == "f") {
            Face face;
            for (int i = 0; i < 3; i++) {
                string vert; arquivo >> vert;
                int vIndex = 0, tIndex = 0, nIndex = 0;
                sscanf(vert.c_str(), "%d/%d/%d", &vIndex, &tIndex, &nIndex);
                face.v[i] = vIndex - 1;
                face.t[i] = tIndex - 1;
                face.n[i] = nIndex - 1;
            }
            faces.push_back(face);
        }
    }

    modelo = glGenLists(1);
    glNewList(modelo, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (auto &face : faces) {
        for (int i = 0; i < 3; i++) {
            if (face.n[i] >= 0 && face.n[i] < (int)normals.size()) {
                auto &n = normals[face.n[i]];
                glNormal3f(n[0], n[1], n[2]);
            }
            if (face.t[i] >= 0 && face.t[i] < (int)texcoords.size()) {
                auto &t = texcoords[face.t[i]];
                glTexCoord2f(t[0], t[1]);
            }
            auto &v = vertices[face.v[i]];
            glVertex3f(v[0], v[1], v[2]);
        }
    }
    glEnd();
    glEndList();
}

//  Configuração de câmera
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w/h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void drawFloor() {
    glPushMatrix();
    glTranslatef(0, -20, 0);
    glScalef(100, 1, 100);

    setMaterial(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-1, 0, -1);
        glVertex3f( 1, 0, -1);
        glVertex3f( 1, 0,  1);
        glVertex3f(-1, 0,  1);
    glEnd();

    glPopMatrix();
}

void drawModel() {
    glPushMatrix();
    glTranslatef(tx, ty, tz);
    glScalef(sx, sy, sz);
    glRotatef(rx, 1, 0, 0);
    glRotatef(ry, 0, 1, 0);
    glRotatef(rz, 0, 0, 1);

    setMaterial(cor_objeto[0], cor_objeto[1], cor_objeto[2]);
    glCallList(modelo);

    glPopMatrix();
}

void display(void) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // fundo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float luz_pos[] = {0.0f, 50.0f, 100.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, luz_pos);

    drawFloor();
    drawModel();
    glutSwapBuffers();
}

//  Teclado
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        // Translação
        case 'w': ty += 1; break;
        case 's': ty -= 1; break;
        case 'a': tx -= 1; break;
        case 'd': tx += 1; break;
        case 'q': tz += 1; break;
        case 'e': tz -= 1; break;

        // Rotação
        case 'j': ry -= 5; break;
        case 'l': ry += 5; break;
        case 'i': rx -= 5; break;
        case 'k': rx += 5; break;
        case 'u': rz -= 5; break;
        case 'o': rz += 5; break;

        // Escala
        case '+': sx *= 1.1f; sy *= 1.1f; sz *= 1.1f; break;
        case '-': sx *= 0.9f; sy *= 0.9f; sz *= 0.9f; break;

        // Cores do objeto
        case '1': cor_objeto[0] = 1.0f; cor_objeto[1] = 0.0f; cor_objeto[2] = 0.0f; break; // vermelho
        case '2': cor_objeto[0] = 0.0f; cor_objeto[1] = 1.0f; cor_objeto[2] = 0.0f; break; // verde
        case '3': cor_objeto[0] = 0.0f; cor_objeto[1] = 0.0f; cor_objeto[2] = 1.0f; break; // azul
        case '4': cor_objeto[0] = 1.0f; cor_objeto[1] = 1.0f; cor_objeto[2] = 0.0f; break; // amarelo
        case '5': cor_objeto[0] = 1.0f; cor_objeto[1] = 1.0f; cor_objeto[2] = 1.0f; break; // branco

        case 27: exit(0); // ESC
    }
    glutPostRedisplay();
}

//  Funções do Mouse
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            leftButtonDown = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else {
            leftButtonDown = false;
        }
    }

    // Scroll para zoom
    if (button == 3) { tz += 2; }
    if (button == 4) { tz -= 2; }

    glutPostRedisplay();
}

//  Movimento do Mouse (drag)
void motion(int x, int y) {
    if (leftButtonDown) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;

        ry += dx * 0.5f;
        rx += dy * 0.5f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

void loadFile() {
    string filename;
    cout << "Digite o nome do arquivo .obj (ex: elepham.obj): ";
    cin >> filename;
    loadObj(filename);
}

//  Main
int main(int argc, char** argv) {
    // Carregar arquivo escolhido pelo usuário
    loadFile();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 450);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Visualizador OBJ");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}