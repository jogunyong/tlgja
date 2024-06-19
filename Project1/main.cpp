#pragma comment(lib, "Opengl32.lib")

#define _CRTDBG_ALLOC
#include <crtdbg.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

// ���� ����
float squareSize = 0.1f;     // �׸��� ũ�� (���簢��)
float squareX = -0.8f;       // �׸��� �ʱ� X ��ġ
float squareY = -0.75f;      // �׸��� �ʱ� Y ��ġ�� �ٴڿ� ��ġ�ϵ��� ����
float velocityY = 0.0f;      // Y ���� �ӵ�
const float gravity = -9.8f; // �߷� ���ӵ� (m/s^2)
const float jumpSpeed = 5.0f; // ���� �ӵ� (m/s)
bool isJumping = false;      // ���� ������ ����
bool isGrounded = true;      // ���� �پ� �ִ��� ����
float moveSpeed = 0.8f;      // �׸��� �̵� �ӵ�
const float maxRotationAngle = 360.0f; // �ִ� ȸ�� ���� (45��)

// ��ֹ� ����ü
struct Obstacle {
    float x, y, width, height;
};

std::vector<Obstacle> obstacles; // ��ֹ� ����

void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << description << std::endl;
}

// �׸� �׸��� �Լ�
void renderSquare()
{
    glPushMatrix();
    glTranslatef(squareX, squareY + 0.05f, 0.0f);  // �̵� (�׸��� �߽��� �ٴ� �ٷ� ���� ������)

    // �÷��̾� ĳ������ ȸ�� ���� ����
    float rotationAngle = std::min(velocityY / jumpSpeed * maxRotationAngle, maxRotationAngle);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

    // ���� ������ ���� �׸���
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); // ������ �׸�

    glVertex2f(-squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, squareSize / 2.0f);
    glVertex2f(-squareSize / 2.0f, squareSize / 2.0f);

    glEnd();

    // �ƿ����� �׸���
    glLineWidth(2.0f); // �ƿ����� �� �β� ����
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f); // ������ �ƿ�����

    glVertex2f(-squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, squareSize / 2.0f);
    glVertex2f(-squareSize / 2.0f, squareSize / 2.0f);

    glEnd();

    glPopMatrix();
}

// �ٴ��� �׸��� �Լ�
void renderFloor()
{
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f); // ȸ�� �ٴ�

    // �ٴ��� �� ���� ����
    glVertex2f(-1.0f, -0.75f);
    glVertex2f(1.0f, -0.75f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);

    glEnd();
}

// ��ֹ��� �׸��� �Լ�
void renderObstacles()
{
    for (const auto& obstacle : obstacles)
    {
        glPushMatrix();
        glTranslatef(obstacle.x, obstacle.y, 0.0f);

        glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 0.0f); // �ʷϻ� ��ֹ�

        glVertex2f(-obstacle.width / 2, 0.0f);
        glVertex2f(obstacle.width / 2, 0.0f);
        glVertex2f(obstacle.width / 2, obstacle.height);
        glVertex2f(-obstacle.width / 2, obstacle.height);

        glEnd();
        glPopMatrix();
    }
}

// �浹 ���� �Լ�
bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && isGrounded)
    {
        // ���� ����
        isJumping = true;
        velocityY = jumpSpeed;
        isGrounded = false;
    }
}

void updateSquarePosition(float deltaTime)
{
    if (isJumping)
    {
        // ���� ���� ��� �߷� ����
        velocityY += gravity * deltaTime;
        squareY += velocityY * deltaTime;

        // ���� �� �ٴڿ��� �������� ���
        if (squareY <= -0.75f)
        {
            squareY = -0.75f;
            velocityY = 0.0f;
            isJumping = false;
            isGrounded = true;
        }
    }

    // �׸� ���������� �̵�
    squareX += moveSpeed * deltaTime;

    // ȭ�� ���� �����ϸ� ��ġ �缳��
    if (squareX > 1.0f) {
        squareX = -1.0f;
    }


}

void updateObstacles(float deltaTime)
{
    // ���ο� ��ֹ��� �浹���� �ʵ��� Ȯ���ϸ� �߰�
    while (!obstacles.empty() && obstacles.front().x < -1.0f)
    {
        obstacles.erase(obstacles.begin());
    }

    if (obstacles.empty() || obstacles.back().x < 0.5f)
    {
        float newX = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
        float newHeight = 0.2f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.5f));
        Obstacle newObstacle = { newX, -0.75f, 0.1f, newHeight };

        bool overlap = false;
        for (const auto& obstacle : obstacles)
        {
            if (checkCollision(newObstacle.x - newObstacle.width / 2, newObstacle.y,
                newObstacle.width, newObstacle.height,
                obstacle.x - obstacle.width / 2, obstacle.y,
                obstacle.width, obstacle.height))
            {
                overlap = true;
                break;
            }
        }

        if (!overlap)
        {
            obstacles.push_back(newObstacle);
        }
    }

    // ��ֹ� �̵� �� �浹 �˻�
    for (auto& obstacle : obstacles)
    {
        obstacle.x -= moveSpeed * deltaTime;

        if (checkCollision(squareX - 0.05f, squareY - 0.05f, 0.1f, 0.1f,
            obstacle.x - obstacle.width / 2, obstacle.y,
            obstacle.width, obstacle.height))
        {
            
        }
    }
}


void render()
{
    glClearColor(0.0f, 30.0f / 255.0f, 100.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderFloor();
    renderSquare();
    renderObstacles();
}

void Physics()
{
    // ���� ��� (���⼭�� �߷¸� ����)
    // ���� ���� �ùķ��̼��� ���⿡ �߰��� �� ����
}

void Update(float deltaTime)
{
    // ������Ʈ ����
    updateSquarePosition(deltaTime);
    updateObstacles(deltaTime);
}

int main(void)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Jumping Square Game", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window, keyCallback);

    // ���� �õ� ����
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // �ʱ� ��ֹ� ����
    obstacles.push_back({ 0.5f, -0.75f, 0.1f, 0.3f });
    obstacles.push_back({ 0.8f, -0.75f, 0.1f, 0.4f });
    obstacles.push_back({ 1.1f, -0.75f, 0.1f, 0.5f });

    double lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // �ð� ����
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        // �̺�Ʈ ó��
        glfwPollEvents();


        // ��ü ��ġ ������Ʈ
        Update(deltaTime);

        // ȭ�� ����� �� �׸���
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        // ȭ�� ���� ��ü
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
