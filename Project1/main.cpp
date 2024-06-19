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

// 전역 변수
float squareSize = 0.1f;     // 네모의 크기 (정사각형)
float squareX = -0.8f;       // 네모의 초기 X 위치
float squareY = -0.75f;      // 네모의 초기 Y 위치는 바닥에 위치하도록 설정
float velocityY = 0.0f;      // Y 방향 속도
const float gravity = -9.8f; // 중력 가속도 (m/s^2)
const float jumpSpeed = 5.0f; // 점프 속도 (m/s)
bool isJumping = false;      // 점프 중인지 여부
bool isGrounded = true;      // 땅에 붙어 있는지 여부
float moveSpeed = 0.8f;      // 네모의 이동 속도
const float maxRotationAngle = 360.0f; // 최대 회전 각도 (45도)

// 장애물 구조체
struct Obstacle {
    float x, y, width, height;
};

std::vector<Obstacle> obstacles; // 장애물 벡터

void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << description << std::endl;
}

// 네모를 그리는 함수
void renderSquare()
{
    glPushMatrix();
    glTranslatef(squareX, squareY + 0.05f, 0.0f);  // 이동 (네모의 중심이 바닥 바로 위로 오도록)

    // 플레이어 캐릭터의 회전 각도 설정
    float rotationAngle = std::min(velocityY / jumpSpeed * maxRotationAngle, maxRotationAngle);
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);

    // 내부 빨간색 색상 그리기
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); // 빨간색 네모

    glVertex2f(-squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, squareSize / 2.0f);
    glVertex2f(-squareSize / 2.0f, squareSize / 2.0f);

    glEnd();

    // 아웃라인 그리기
    glLineWidth(2.0f); // 아웃라인 선 두께 설정
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0f, 0.0f, 0.0f); // 검정색 아웃라인

    glVertex2f(-squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, -squareSize / 2.0f);
    glVertex2f(squareSize / 2.0f, squareSize / 2.0f);
    glVertex2f(-squareSize / 2.0f, squareSize / 2.0f);

    glEnd();

    glPopMatrix();
}

// 바닥을 그리는 함수
void renderFloor()
{
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f); // 회색 바닥

    // 바닥의 네 점을 정의
    glVertex2f(-1.0f, -0.75f);
    glVertex2f(1.0f, -0.75f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);

    glEnd();
}

// 장애물을 그리는 함수
void renderObstacles()
{
    for (const auto& obstacle : obstacles)
    {
        glPushMatrix();
        glTranslatef(obstacle.x, obstacle.y, 0.0f);

        glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 0.0f); // 초록색 장애물

        glVertex2f(-obstacle.width / 2, 0.0f);
        glVertex2f(obstacle.width / 2, 0.0f);
        glVertex2f(obstacle.width / 2, obstacle.height);
        glVertex2f(-obstacle.width / 2, obstacle.height);

        glEnd();
        glPopMatrix();
    }
}

// 충돌 감지 함수
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
        // 점프 시작
        isJumping = true;
        velocityY = jumpSpeed;
        isGrounded = false;
    }
}

void updateSquarePosition(float deltaTime)
{
    if (isJumping)
    {
        // 점프 중인 경우 중력 적용
        velocityY += gravity * deltaTime;
        squareY += velocityY * deltaTime;

        // 점프 중 바닥에서 떨어지는 경우
        if (squareY <= -0.75f)
        {
            squareY = -0.75f;
            velocityY = 0.0f;
            isJumping = false;
            isGrounded = true;
        }
    }

    // 네모가 오른쪽으로 이동
    squareX += moveSpeed * deltaTime;

    // 화면 끝에 도달하면 위치 재설정
    if (squareX > 1.0f) {
        squareX = -1.0f;
    }


}

void updateObstacles(float deltaTime)
{
    // 새로운 장애물이 충돌하지 않도록 확인하며 추가
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

    // 장애물 이동 및 충돌 검사
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
    // 물리 계산 (여기서는 중력만 적용)
    // 실제 물리 시뮬레이션은 여기에 추가될 수 있음
}

void Update(float deltaTime)
{
    // 업데이트 로직
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

    // 랜덤 시드 설정
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 초기 장애물 설정
    obstacles.push_back({ 0.5f, -0.75f, 0.1f, 0.3f });
    obstacles.push_back({ 0.8f, -0.75f, 0.1f, 0.4f });
    obstacles.push_back({ 1.1f, -0.75f, 0.1f, 0.5f });

    double lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // 시간 측정
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        // 이벤트 처리
        glfwPollEvents();


        // 객체 위치 업데이트
        Update(deltaTime);

        // 화면 지우기 및 그리기
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        // 화면 버퍼 교체
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
