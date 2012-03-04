#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <GL/glfw.h>

struct bullet {
    float position_x;
    float position_y;
};

struct color {
    float r;
    float g;
    float b;
};

struct opponent {
    float position_x;
    float position_y;
    int life;
    color bgColor;
};



/*----------------------*/
void initGlfwWindow(void);
void shutdown(int returnCode);
void mainLoop(void);
void drawScene(void);
color getColorByLifes(int lifes);

int running = GL_TRUE;
/*----------------------*/

using namespace std;

float machinePosition = 0.0f;
int score = 0;
float opponentAcceleration = 1.0f;



namespace gameOptions {
    color bulletColor = {0.5f, 1.0f, 0.0f};
    float opponentSize = 0.2f;
}

vector<bullet> bullets;
vector<opponent> opponents;

double t0;

int main(int argc, char** argv) {
    initGlfwWindow();
    mainLoop();
    shutdown(0);
}

void resize(int x, int y) {
    glfwSetWindowSize(x, y);
}

void keyPressed(int key, int action) {
    if ((key == GLFW_KEY_UP || key == ' ') && action == GLFW_PRESS) {
        bullet b;
        b.position_x = machinePosition;
        b.position_y = -0.95f;
        bullets.push_back(b);
    }
}


void initGlfwWindow(void) {
    const int width = 500, height = 500;
    if (glfwInit() != GL_TRUE)
        shutdown(1);
    if (glfwOpenWindow(width, height, 5, 6, 5, 0, 0, 0, GLFW_WINDOW) != GL_TRUE)
        shutdown(1);
    glfwSetWindowTitle("Game");
    glfwSetWindowSizeCallback(&resize);
    glfwSetKeyCallback(&keyPressed);
    glfwSwapInterval(1);
    
    t0 = glfwGetTime();
    
    srand(time(NULL));

    while(running) mainLoop();
}

void mainLoop(void) {
    if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
        if ((machinePosition - 0.1f) >= -1.0f)
            machinePosition -= 0.02f;
    }
    
    if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if ((machinePosition + 0.1f) <= 1.0f)
            machinePosition += 0.02f;
    }
    
    double time = glfwGetTime();
    
    // Пришло время запустить нового врага
    if((time - t0) > 2.0f) {
        opponent newOpponent;
        float x_pos = (rand() % 100 - 50) / (float)50;
        newOpponent.position_x = x_pos;
        newOpponent.position_y = 1.0f;
        newOpponent.life = rand() % 3 + 1;
        newOpponent.bgColor = getColorByLifes(newOpponent.life);
        opponents.push_back(newOpponent);
        t0 = time;
    }
    
    // Ускорение врагов
    opponentAcceleration += 0.001f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    
    glfwSwapBuffers();
    running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam(GLFW_OPENED);
}

color getColorByLifes(int lifes) {
    color c;
    switch(lifes) {
    case 1:
        c = {1.0f, 0.0f, 0.0f};
        break;
    case 2:
        c = {1.0f, 0.4f, 0.4f};
        break;
    case 3:
        c = {1.0f, 0.8f, 0.8f};
        break;
    }
    return c;
}

void drawKillMachine(float position) {
    glBegin(GL_POLYGON);
        glColor3f(0.2f, 0.2f, 0.2f);
        glVertex2f(position - 0.1f, -1.0f);
        glColor3f(0.4f, 0.4f, 0.4f);
        glVertex2f(position + 0.1f, -1.0f);
        glColor3f(0.6f, 0.6f, 0.6f);
        glVertex2f(position + 0.1f, -0.95f);
        glColor3f(0.8f, 0.8f, 0.8f);
        glVertex2f(position - 0.1f, -0.95f);
    glEnd();
}

void drawBullet(float pos_x, float pos_y) {
    color bc = gameOptions::bulletColor;
    glColor3f(bc.r, bc.g, bc.b);
    // Рисуем точку, котрая какбэ пуля
    glBegin(GL_POINTS);
        glVertex2f(pos_x, pos_y);
    glEnd();
}

void drawOpponent(opponent data) {
    glColor3f(data.bgColor.r, data.bgColor.g, data.bgColor.b);
    float half = gameOptions::opponentSize / 2;
    glBegin(GL_QUADS);
        glVertex2f(data.position_x - half, data.position_y - half);
        glVertex2f(data.position_x - half, data.position_y + half);
        glVertex2f(data.position_x + half, data.position_y + half);
        glVertex2f(data.position_x + half, data.position_y - half);
    glEnd();
}

void drawScene(void) {
    glLoadIdentity();
    // Рисуем машину-убийцу
    drawKillMachine(machinePosition);
    std::vector<bullet>::iterator bi;
    for (bi = bullets.begin(); bi < bullets.end(); ++bi) {
        
        std::vector<opponent>::iterator oi;
        float bx = (*bi).position_x;
        float by = (*bi).position_y;
        
        // Проверяем попадание в мишень, если попал - удаляем элемент
        for (oi = opponents.begin(); oi < opponents.end(); ++oi) {
            float ox = (*oi).position_x;
            float oy = (*oi).position_y;
            float halfO = gameOptions::opponentSize / 2;
            if (   bx > (ox - halfO) && bx < (ox + halfO)
                && by > (oy - halfO) && by < (oy + halfO)) {
                score++;
                bullets.erase(bi);
                if(--(*oi).life == 0) {
                    opponents.erase(oi);
                } else {
                    (*oi).bgColor = getColorByLifes((*oi).life);
                }
            }
        }
        
        // Перемещаем "пулю вверх"
        (*bi).position_y += 0.01f;
        // Если пуля ушла за приделы экрана, удаляем её из массива,
        // чтобы не занимать память
        if ((*bi).position_y > 1.0f) {
            bullets.erase(bi);
        }
        // Рисуем пулю
        drawBullet((*bi).position_x, (*bi).position_y);
    }
    
    std::vector<opponent>::iterator oi;
    for (oi = opponents.begin(); oi < opponents.end(); ++oi) {
        (*oi).position_y -= 0.002f * opponentAcceleration;
        // Если враг таки пробрался через твою бляддскую защиту, то можно констатировать
        // тот факт, что ты криворукий мудак и нужно отобрать у тебя баллов, чтобы ты больше
        // не был таким ебанутым тормозом.
        if ((*oi).position_y < -1.0f) {
            opponents.erase(oi);
            score -= 5;
        }
        drawOpponent(*oi);
    }
}

void shutdown(int returnCode) {
    cout << "Your score: " << score << endl;
    glfwTerminate();
    exit(returnCode);
}
