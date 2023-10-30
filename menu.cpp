#include <raylib.h>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <fstream>

// Variáveis globais
Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int screenWidth = 800;
int screenHeight = 600;

int game_mode; // 0 para singleplayer, 1 para multiplayer

int player_score = 0;
int cpu_score = 0;

class Ball {
 public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, Yellow);
    }

    void Update() {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }
        // Cpu wins
        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            ResetBall();
        }

        if (x - radius <= 0) {
            player_score++;
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle {
 protected:
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

 public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
 public:
    void Update(int ball_y){
        if (y + height / 2 > ball_y) {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y) {
            y = y + speed;
        }
        LimitMovement();
    }
};

Ball ball;
Paddle player1, player2;
CpuPaddle cpu;

// Função para carregar o histórico de jogos
std::string LoadGameHistory(const std::string& fileName) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string history;
        std::string line;
        while (std::getline(file, line)) {
            history += line + "\n";
        }
        file.close();
        return history;
    }
    return "No game history available.";
}

// Função para salvar o histórico de jogos
void SaveGameHistory(const std::string& fileName, const std::string& history) {
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << history;
        file.close();
    }
}

// Função para exibir o histórico de jogos
void ShowGameHistory(const std::string& history) {
    // Implemente a lógica para exibir o histórico na tela
    // Pode ser uma janela pop-up ou uma nova tela
}

// Função para a tela inicial
int ShowMainMenu() {
    int selectedOption = 0;  // Opção selecionada (0 para single player, 1 para multiplayer, 2 para histórico, 3 para sair)

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_DOWN)) {
            selectedOption = (selectedOption + 1) % 4;
        } else if (IsKeyPressed(KEY_UP)) {
            if (selectedOption == 0) {
                selectedOption = 3;
            } else {
                selectedOption--;
            }
        }

        ClearBackground(Dark_Green);

        DrawText("Pong CV", screenWidth / 2 - MeasureText("Pong CV", 40) / 2, 100, 40, WHITE);

        // Botões
        if (selectedOption == 0) {
            DrawText("Single Player", screenWidth / 2 - MeasureText("Single Player", 20) / 2, 300, 20, LIGHTGRAY);
            DrawText("Multiplayer", screenWidth / 2 - MeasureText("Multiplayer", 20) / 2, 350, 20, WHITE);
            DrawText("Game History", screenWidth / 2 - MeasureText("Game History", 20) / 2, 400, 20, WHITE);
            DrawText("Quit", screenWidth / 2 - MeasureText("Quit", 20) / 2, 450, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {
                return 0;  // Retorne 0 para iniciar o jogo single player
            }
        } else if (selectedOption == 1) {
            DrawText("Single Player", screenWidth / 2 - MeasureText("Single Player", 20) / 2, 300, 20, WHITE);
            DrawText("Multiplayer", screenWidth / 2 - MeasureText("Multiplayer", 20) / 2, 350, 20, LIGHTGRAY);
            DrawText("Game History", screenWidth / 2 - MeasureText("Game History", 20) / 2, 400, 20, WHITE);
            DrawText("Quit", screenWidth / 2 - MeasureText("Quit", 20) / 2, 450, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {
                return 1;  // Retorne 1 para iniciar o jogo multiplayer
            }
        } else if (selectedOption == 2) {
            DrawText("Single Player", screenWidth / 2 - MeasureText("Single Player", 20) / 2, 300, 20, WHITE);
            DrawText("Multiplayer", screenWidth / 2 - MeasureText("Multiplayer", 20) / 2, 350, 20, WHITE);
            DrawText("Game History", screenWidth / 2 - MeasureText("Game History", 20) / 2, 400, 20, LIGHTGRAY);
            DrawText("Quit", screenWidth / 2 - MeasureText("Quit", 20) / 2, 450, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {
                // Carregue o histórico e exiba-o (implemente a funcionalidade)
                std::string history = LoadGameHistory("game_history.txt");
                ShowGameHistory(history);
            }
        } else if (selectedOption == 3) {
            DrawText("Single Player", screenWidth / 2 - MeasureText("Single Player", 20) / 2, 300, 20, WHITE);
            DrawText("Multiplayer", screenWidth / 2 - MeasureText("Multiplayer", 20) / 2, 350, 20, WHITE);
            DrawText("Game History", screenWidth / 2 - MeasureText("Game History", 20) / 2, 400, 20, WHITE);
            DrawText("Quit", screenWidth / 2 - MeasureText("Quit", 20) / 2, 450, 20, LIGHTGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                return 3;  // Retorne 3 para sair do jogo
            }
        }

        EndDrawing();
    }

    return 0;
}

// Função principal do jogo Pong (Single Player)
int PlaySinglePlayerPong() {
    std::cout << "Starting the game" << std::endl;
    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "My Pong Game!");
    SetTargetFPS(60);
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player1.width = 25;
    player1.height = 120;
    player1.x = screen_width - player1.width - 10;
    player1.y = screen_height / 2 - player1.height / 2;
    player1.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (WindowShouldClose() == false) {
        BeginDrawing();

        // Updating

        ball.Update();
        player1.Update();
        cpu.Update(ball.y);

        // Checking for collisions
        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player1.x, player1.y, player1.width, player1.height})) {
            ball.speed_x *= -1;
        }

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {cpu.x, cpu.y, cpu.width, cpu.height})) {
            ball.speed_x *= -1;
        }

        // Drawing
        ClearBackground(Dark_Green);
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.Draw();
        cpu.Draw();
        player1.Draw();
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Função principal do jogo Pong (Multiplayer)
int PlayMultiplayerPong() {
    // ... (código do jogo Pong multiplayer)

    return 0;
}

int opencv() {
    // Carregue o classificador Haar Cascade para detecção de faces
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("haarcascade_frontalface_default.xml")) {
        std::cerr << "Erro ao carregar o classificador Haar Cascade." << std::endl;
        return -1;
    }

    // Inicialize a câmera ou carregue uma imagem
    cv::VideoCapture cap(0);  // Use 0 para a câmera padrão ou forneça o caminho para um arquivo de imagem

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera ou o arquivo de imagem." << std::endl;
        return -1;
    }

    cv::Mat frame;

    while (true) {
        cap >> frame;  // Captura um quadro da câmera ou do arquivo de imagem

        if (frame.empty()) {
            std::cerr << "Quadro vazio." << std::endl;
            break;
        }

        // Converta o quadro para escala de cinza
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detecte rostos no quadro
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.3, 5);

        // Desenhe retângulos ao redor dos rostos detectados
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            if (!game_mode) { // singleplayer
                player1.y = (face.y + face.height/2 - 80)*17/8;
            }
            else {
                if ((face.x + face.width/2) < 320) {
                    player1.y = (face.y + face.height/2 - 80)*17/8;
                }
                else {
                    player2.y = (face.y + face.height/2 - 80)*17/8;
                }
            }
            
        }

        // Exiba o quadro com os rostos detectados
        cv::imshow("Detecção de Rosto", frame);

        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}

int game() {
    InitWindow(screenWidth, screenHeight, "Pong CV - Main Menu");

    while (true) {
        int selectedOption = ShowMainMenu();

        if (selectedOption == 0) {
            // Inicie o jogo single player quando a opção for selecionada
            CloseWindow();
            PlaySinglePlayerPong();
        } else if (selectedOption == 1) {
            // Inicie o jogo multiplayer quando a opção for selecionada
            CloseWindow();
            PlayMultiplayerPong();
        } else if (selectedOption == 3) {
            // Saia do jogo quando a opção for selecionada
            CloseWindow();
            return 0;
        }
    }
}

int main() {
    // Crie duas threads para executar as funções simultaneamente
    std::thread thread1(opencv);
    std::thread thread2(game);

    // Aguardar que as threads terminem
    thread1.join();
    thread2.join();

    std::cout << "Thread principal terminou." << std::endl;


return 0;
}
