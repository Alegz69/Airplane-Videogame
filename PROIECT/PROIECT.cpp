#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <ctime>

// Variabile globale pentru controlul jocului
float translateX = 0.0f; // Coordonata X a jucătorului
float translateY = -0.5f; // Coordonata Y a jucătorului
float translationSpeed = 0.0175f; // Viteza de deplasare a jucătorului
bool keyStates[256] = { false }; // Starea tastelor
std::vector<float> bulletYs; // Lista cu coordonatele Y ale glonțurilor
std::vector<float> fallingObjectsY; // Lista cu coordonatele Y ale obiectelor care cad
std::vector<float> fallingObjectsX; // Lista cu coordonatele X ale obiectelor care cad
std::vector<float> fallingStarsY; // Lista cu coordonatele Y ale stelelor care cad
std::vector<float> fallingStarsX; // Lista cu coordonatele X ale stelelor care cad
float bulletSpeed = 0.35f; // Viteza glonțurilor
bool isShooting = false; // Indicator pentru trageri
clock_t lastShotTime = 0; // Timpul ultimei trageri
float spawnInterval = 3.0f; //Intervalul dintre a aparea inamicii
const float cooldownDuration = 0.125f; // Durata de răcire între trageri
float harderOverTime = 30.0f; // Variabilă pentru creșterea dificultății în timp

// Viteza de cădere a obiectelor și a stelelor
float fallingObjectSpeed = -0.01f;
float fallingStarSpeed = -0.0005f;

// Variabile pentru gestionarea timpului și a scorului
time_t gameStartTime = 0;
int score = 0;
bool gameOver = false;
bool overlapping;
const float destructionRadius = 0.25f; // Raza de distrugere a obiectelor

// Funcție pentru calcularea distanței între două puncte
float distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Funcție pentru inițializarea OpenGL
void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    srand(static_cast<unsigned int>(time(0)));
}

// Funcție pentru redimensionarea ferestrei
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Funcție pentru afișarea textului pe ecran
void renderBitmapString(float x, float y, void* font, const char* string) {
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Funcție pentru afișarea ecranului de Game Over
void drawGameOverScreen() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    renderBitmapString(-0.3f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Game Over");
    renderBitmapString(-0.4f, -0.1f, GLUT_BITMAP_HELVETICA_18, "Press 'R' to restart");
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Funcție pentru resetarea jocului
void resetGame()
{
    gameOver = false;
    fallingObjectsY.clear();
    fallingObjectsX.clear();
    bulletYs.clear();
    score = 0;
    gameStartTime = 0;
    for (int i = 0; i < 256; ++i) {
        keyStates[i] = false;
    }
    translateX = 0.0f;
    translateY = -0.5f;
}

// Funcție pentru desenarea scenei
void draw() {
    // Șterge bufferul de culoare și resetează matricea de modelare-vizualizare
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Desenează fundalul jocului
    glColor4f(0.0f, 0.0f, 0.035f, 0.5f); // Albastru închis cu 50% opacitate
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Desenează stelele care cad pe fundal
    for (size_t i = 0; i < fallingStarsY.size(); ++i) {
        glPushMatrix();
        glTranslatef(fallingStarsX[i], fallingStarsY[i], 0.0f);
        glScalef(0.05f, 0.05f, 1.0f);
        glBegin(GL_TRIANGLES);


        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-0.60, 0.77, 0);
        glVertex3f(-0.42, 0.77, 0);
        glVertex3f(-0.58, 0.68, 0);


        glVertex3f(-0.64, 1, 0);
        glVertex3f(-0.68, 0.77, 0);
        glVertex3f(-0.60, 0.77, 0);


        glVertex3f(-0.68, 0.77, 0);
        glVertex3f(-0.7, 0.68, 0);
        glVertex3f(-0.86, 0.77, 0);


        glVertex3f(-0.64, 0.63, 0);
        glVertex3f(-0.7, 0.68, 0);
        glVertex3f(-0.82, 0.43, 0);


        glVertex3f(-0.64, 0.63, 0);
        glVertex3f(-0.58, 0.68, 0);
        glVertex3f(-0.51, 0.43, 0);

        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POLYGON);

        glVertex3f(-0.60, 0.77, 0);
        glVertex3f(-0.68, 0.77, 0);
        glVertex3f(-0.7, 0.68, 0);
        glVertex3f(-0.64, 0.63, 0);
        glVertex3f(-0.58, 0.68, 0);

        glEnd();
        glPopMatrix();
    }

    // Desenează obiectele care cad
    glColor3f(1.0, 0.0, 0.0); // Roșu
    for (size_t i = 0; i < fallingObjectsY.size(); ++i) {
        glPushMatrix();
        glTranslatef(fallingObjectsX[i], fallingObjectsY[i], 0.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        glScalef(0.1f, 0.1f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, -0.3f);
        glVertex2f(0.1f, -0.3f);
        glVertex2f(0.1f, 0.3f);
        glVertex2f(-0.1f, 0.3f);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.1f, 0.0f);
        glVertex2f(-0.45f, 0.1f);
        glVertex2f(-0.1f, 0.25f);
        glVertex2f(0.1f, 0.0f);
        glVertex2f(0.45f, 0.1f);
        glVertex2f(0.1f, 0.25f);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.1f, 0.3f);
        glVertex2f(0.1f, 0.3f);
        glVertex2f(0.0f, 0.5f);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.1f, -0.1f);
        glVertex2f(-0.3f, -0.4f);
        glVertex2f(-0.1f, -0.3f);
        glVertex2f(0.1f, -0.1f);
        glVertex2f(0.3f, -0.4f);
        glVertex2f(0.1f, -0.3f);
        glEnd();

        glPopMatrix();
    }

    // Desenează jucătorul
    glTranslatef(translateX, translateY, 0.0f);
    glScalef(0.175f, 0.175f, 1.0f);

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.1f, -0.3f);
    glVertex2f(0.1f, -0.3f);
    glVertex2f(0.1f, 0.3f);
    glVertex2f(-0.1f, 0.3f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.1f, 0.0f);
    glVertex2f(-0.45f, 0.1f);
    glVertex2f(-0.1f, 0.25f);

    glVertex2f(0.1f, 0.0f);
    glVertex2f(0.45f, 0.1f);
    glVertex2f(0.1f, 0.25f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.1f, 0.3f);
    glVertex2f(0.1f, 0.3f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.1f, -0.1f);
    glVertex2f(-0.3f, -0.4f);
    glVertex2f(-0.1f, -0.3f);

    glVertex2f(0.1f, -0.1f);
    glVertex2f(0.3f, -0.4f);
    glVertex2f(0.1f, -0.3f);
    glEnd();

    // Desenează gloanțele trase de jucător
    for (size_t i = 0; i < bulletYs.size(); ++i) {
        glColor3f(1.0, 1.0, 1.0);
        glPointSize(5.5f);
        glBegin(GL_POINTS);
        glVertex2f(0, bulletYs[i]);
        glEnd();
    }

    // Afisează scorul pe ecran
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    char scoreText[20];
    sprintf_s(scoreText, sizeof(scoreText), "Scor: %d", score);
    glScalef(2.0f, 2.0f, 1.0f);
    renderBitmapString(0.35f, 0.45f, GLUT_BITMAP_HELVETICA_18, scoreText);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    // Afiseaza ecranul de Game Over, daca este cazul
    if (gameOver)
    {
        drawGameOverScreen();
    }

    // Schimba bufferele pentru a afisa rezultatul
    glutSwapBuffers();
}

// Funcție pentru generarea obiectelor care cad
void spawnFallingObjects() {
    int numObjects = rand() % 5 + 1;

    for (int i = 0; i < numObjects; ++i) {
        float randomX, randomY;

        // Generate random position until a non-overlapping position is found
        do {
            randomX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.8f - 0.9f;
            randomY = 1.0f;

            // Check if the new position overlaps with any existing falling objects
            overlapping = false;
            for (size_t i = 0; i < fallingObjectsY.size(); ++i) {
                if (fabs(randomX - fallingObjectsX[i]) < 0.2f && fabs(randomY - fallingObjectsY[i]) < 0.2f) {
                    overlapping = true;
                    break;
                }
            }

            // If overlapping, adjust the position and repeat the process
            if (overlapping) {
                // Adjust position (for example, move vertically)
                randomX -= 0.2f; // Move down by a certain amount
            }
        } while (overlapping);

        // Add the new falling object with the adjusted position
        fallingObjectsY.push_back(randomY);
        fallingObjectsX.push_back(randomX);
    }
}

// Funcție pentru generarea stelelor care cad
void spawnFallingStars() {
    if (fallingStarsY.empty()) {
        // Generate stars randomly on the screen at the start of the program
        int numStars = 30;
        for (int i = 0; i < numStars; ++i) {
            float randomX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.8f - 0.9f;
            float randomY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.8f - 0.9f; // Generate Y positions randomly across the screen
            fallingStarsX.push_back(randomX);
            fallingStarsY.push_back(randomY);
        }
    }
    else {
        // Generate new stars at the top of the screen as they begin to fall
        int numStars = rand() % 10 + 1; // Generate fewer stars as new ones are added to avoid overcrowding
        for (int i = 0; i < numStars; ++i) {
            float randomX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.8f - 0.9f;
            float randomY = 1.0f; // Generate stars at the top of the screen
            fallingStarsX.push_back(randomX);
            fallingStarsY.push_back(randomY);
        }
    }
}

// Funcție pentru actualizarea jocului
void update(int value) {
    if (!gameOver) {
        if (keyStates['w'] && keyStates['a']) {
            translateY += translationSpeed * 0.7071f;
            translateX -= translationSpeed * 0.7071f;
        }
        else if (keyStates['w'] && keyStates['d']) {
            translateY += translationSpeed * 0.7071f;
            translateX += translationSpeed * 0.7071f;
        }
        else if (keyStates['s'] && keyStates['a']) {
            translateY -= translationSpeed * 0.7071f;
            translateX -= translationSpeed * 0.7071f;
        }
        else if (keyStates['s'] && keyStates['d']) {
            translateY -= translationSpeed * 0.7071f;
            translateX += translationSpeed * 0.7071f;
        }
        else {
            if (keyStates['w']) {
                translateY += translationSpeed;
            }
            if (keyStates['s']) {
                translateY -= translationSpeed;
            }
            if (keyStates['a']) {
                translateX -= translationSpeed;
            }
            if (keyStates['d']) {
                translateX += translationSpeed;
            }
        }

        if (translateX < -0.90f) translateX = -0.90f;
        if (translateX > 0.90f) translateX = 0.90f;
        if (translateY < -0.90f) translateY = -0.90f;
        if (translateY > 0.90f) translateY = 0.90f;

        if (gameStartTime == 0) {
            spawnFallingObjects();
            spawnFallingStars();
            gameStartTime = clock();
        }

        clock_t currentTime = clock();
        float elapsedTime = static_cast<float>(currentTime - gameStartTime) / CLOCKS_PER_SEC;
        float elapsetShotTime = static_cast<float>(currentTime - lastShotTime) / CLOCKS_PER_SEC;

        if (elapsedTime > spawnInterval) {
            spawnFallingObjects();
            spawnFallingStars();
            gameStartTime = currentTime;

            if (elapsedTime > harderOverTime) {
                spawnInterval *= 0.999f;
                harderOverTime += 30.0f;
            }
        }

        if (isShooting && elapsetShotTime > cooldownDuration) {
            bulletYs.push_back(translateY + 0.80f);
            lastShotTime = currentTime;
        }

        for (size_t i = 0; i < bulletYs.size(); ++i) {
            float bulletRadius = 0.075f; // Adjust as needed
            for (size_t j = 0; j < fallingObjectsY.size(); ++j) {
                float fallingObjectRadius = 0.125f; // Adjust as needed
                if (bulletYs[i] + bulletRadius > fallingObjectsY[j] - fallingObjectRadius &&
                    bulletYs[i] - bulletRadius < fallingObjectsY[j] + fallingObjectRadius &&
                    translateX + bulletRadius > fallingObjectsX[j] - fallingObjectRadius &&
                    translateX - bulletRadius < fallingObjectsX[j] + fallingObjectRadius) {
                    bulletYs.erase(bulletYs.begin() + i);
                    fallingObjectsY.erase(fallingObjectsY.begin() + j);
                    fallingObjectsX.erase(fallingObjectsX.begin() + j);
                    score += 10;
                    --i;
                    break;
                }
            }
        }


        for (size_t j = 0; j < fallingObjectsY.size(); ++j) {
            if (fabs(translateY - fallingObjectsY[j]) < 0.1f && fabs(translateX - fallingObjectsX[j]) < 0.1f) {
                gameOver = true;
                fallingObjectsY.erase(fallingObjectsY.begin() + j);
                fallingObjectsX.erase(fallingObjectsX.begin() + j);

                break;
            }
        }

        for (size_t i = 0; i < fallingStarsY.size(); ++i) {
            fallingStarsY[i] += fallingStarSpeed;
            if (fallingStarsY[i] < -1.0f) {
                fallingStarsY.erase(fallingStarsY.begin() + i);
                fallingStarsX.erase(fallingStarsX.begin() + i);
                --i;
            }
        }


        for (size_t i = 0; i < bulletYs.size(); ++i) {
            bulletYs[i] += bulletSpeed;
            if (bulletYs[i] > 50.0f) {
                bulletYs.erase(bulletYs.begin() + i);
                --i;
            }
        }
        for (size_t i = 0; i < fallingObjectsY.size(); ++i) {
            fallingObjectsY[i] += fallingObjectSpeed;

            if (fallingObjectsY[i] < -1.0f) {
                fallingObjectsY.erase(fallingObjectsY.begin() + i);
                fallingObjectsX.erase(fallingObjectsX.begin() + i);
                --i;
                score -= 10;
            }
        }

        if (isShooting) {
            for (size_t j = 0; j < fallingObjectsY.size(); ++j) {
                float dist = distance(translateX, translateY, fallingObjectsX[j], fallingObjectsY[j]);
                if (dist < destructionRadius) {
                    // Destroy the falling object
                    fallingObjectsY.erase(fallingObjectsY.begin() + j);
                    fallingObjectsX.erase(fallingObjectsX.begin() + j);
                    score += 10;
                    --j; // Adjust index since falling object is removed
                }
            }
        }


    }
    else {
        if (keyStates['r'] || keyStates['R']) {
            resetGame();
        }
    }
    // Reapelează funcția de actualizare la fiecare cadru
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}
// Funcție pentru tratarea apăsării tastei
void keyPressed(unsigned char key, int x, int y) {
    keyStates[key] = true;
    if (key == 'r' || key == 'R') {
        resetGame();
    }
}

// Funcție pentru tratarea eliberării tastei
void keyReleased(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

// Funcție pentru tratarea evenimentelor mouse-ului
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isShooting = true;
        }
        else if (state == GLUT_UP) {
            isShooting = false;
        }
    }
}

// Funcția principală
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Proiect");
    init();
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}