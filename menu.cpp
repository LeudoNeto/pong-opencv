#include <raylib.h>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>

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

bool game_over = false;

Sound collisionSound;
Sound goalSound;

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
            PlaySound(goalSound);
            ResetBall();
        }

        if (x - radius <= 0) {
            player_score++;
            PlaySound(goalSound);
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
void ShowGameHistory(int game_mode) {
    while (!WindowShouldClose()) {
        ClearBackground(Dark_Green);
        DrawText("Histórico de Jogos", screenWidth / 2 - MeasureText("Histórico de Jogos", 30) / 2, 50, 30, WHITE);

        std::string historyFileName = (game_mode == 0) ? "historico_singleplayer.txt" : "historico_multiplayer.txt";
        std::string history = LoadGameHistory(historyFileName);

        DrawText(history.c_str(), 100, 100, 20, WHITE);
        
        DrawText("Pressione ESC para voltar ao menu", screenWidth / 2 - MeasureText("Pressione ESC para voltar ao menu", 20) / 2, screenHeight - 100, 20, WHITE);
    
        EndDrawing();
    }
}


// Função para a tela inicial
int ShowMainMenu() {
    int selectedOption = 0;  // Opção selecionada (0 para single player, 1 para multiplayer, 2 para histórico, 3 para sair)

    while (!WindowShouldClose()) {
        ClearBackground(Dark_Green);

        DrawText("Pong CV", screenWidth / 2 - MeasureText("Pong CV", 40) / 2, 100, 40, WHITE);

        // Botões
        DrawText("Single Player", screenWidth / 2 - MeasureText("Single Player", 20) / 2, 300, 20, WHITE);
        DrawText("Multiplayer", screenWidth / 2 - MeasureText("Multiplayer", 20) / 2, 350, 20, WHITE);
        DrawText("Game History (Singleplayer)", screenWidth / 2 - MeasureText("Game History (Singleplayer)", 20) / 2, 400, 20, WHITE);
        DrawText("Game History (Multiplayer)", screenWidth / 2 - MeasureText("Game History (Multiplayer)", 20) / 2, 450, 20, WHITE);
        DrawText("Quit", screenWidth / 2 - MeasureText("Quit", 20) / 2, 500, 20, WHITE);

        // Verifique os cliques do mouse
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, {screenWidth / 2 - MeasureText("Single Player", 20) / 2, 300, MeasureText("Single Player", 20), 20})) {
                return 0;  // Retorne 0 para iniciar o jogo single player
            } else if (CheckCollisionPointRec(mousePos, {screenWidth / 2 - MeasureText("Multiplayer", 20) / 2, 350, MeasureText("Multiplayer", 20), 20})) {
                return 1;  // Retorne 1 para iniciar o jogo multiplayer
            } else if (CheckCollisionPointRec(mousePos, {screenWidth / 2 - MeasureText("Game History (Singleplayer)", 20) / 2, 400, MeasureText("Game History (Singleplayer)", 20), 20})) {
                // Carregue e exiba o histórico de singleplayer
                ShowGameHistory(0);
            } else if (CheckCollisionPointRec(mousePos, {screenWidth / 2 - MeasureText("Game History (Multiplayer)", 20) / 2, 450, MeasureText("Game History (Multiplayer)", 20), 20})) {
                // Carregue e exiba o histórico de multiplayer
                ShowGameHistory(1);
            } else if (CheckCollisionPointRec(mousePos, {screenWidth / 2 - MeasureText("Quit", 20) / 2, 500, MeasureText("Quit", 20), 20})) {
                return 3;  // Retorne 3 para sair do jogo
            }
        }

        EndDrawing();
    }

    return 0;
}


// Função para exibir a tela de resultado
void ShowResultScreen(const std::string& winner) {
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
        ClearBackground(Dark_Green);

        DrawText("Game Over", screenWidth / 2 - MeasureText("Game Over", 40) / 2, 100, 40, WHITE);
        DrawText(TextFormat("%s Venceu!", winner.c_str()), screenWidth / 2 - MeasureText(winner.c_str(), 30) / 2, 200, 30, WHITE);
        DrawText("Pressione ESC para retornar ao menu", screenWidth / 2 - MeasureText("Press ESC to return to the main menu", 20) / 2, 300, 20, WHITE);

        EndDrawing();
    }

    std::string historyFileName = (game_mode == 0) ? "historico_singleplayer.txt" : "historico_multiplayer.txt";
    std::string history = LoadGameHistory(historyFileName);
    history += winner + " venceu!\n";
    SaveGameHistory(historyFileName, history);

    game_over = false;
}

// Função principal do jogo Pong (Single Player)
int PlaySinglePlayerPong() {
    game_mode = 0;
    player_score = cpu_score = 0;
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

        // Verifique se um dos scores passou de 10
        if (player_score >= 10) {
            game_over = true;
            ShowResultScreen("Player");
            break;
        } else if (cpu_score >= 10) {
            game_over = true;
            ShowResultScreen("CPU");
            break;
        }

        // Checking for collisions
        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player1.x, player1.y, player1.width, player1.height})) {
            ball.speed_x *= -1;
            PlaySound(collisionSound);
        }

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {cpu.x, cpu.y, cpu.width, cpu.height})) {
            ball.speed_x *= -1;
            PlaySound(collisionSound);
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

    EndDrawing();
    CloseWindow();
    return 0;
}

// Função principal do jogo Pong (Multiplayer)
int PlayMultiplayerPong() {
    game_mode = 1;
    player_score = cpu_score = 0;
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

    player2.height = 120;
    player2.width = 25;
    player2.x = 10;
    player2.y = screen_height / 2 - player2.height / 2;
    player2.speed = 6;

    while (WindowShouldClose() == false) {
        BeginDrawing();

        // Updating

        ball.Update();
        player1.Update();
        player2.Update();

        // Verifique se um dos scores passou de 10
        if (player_score >= 10) {
            game_over = true;
            ShowResultScreen("Player 1");
            break;
        } else if (cpu_score >= 10) {
            game_over = true;
            ShowResultScreen("Player 2");
            break;
        }

        // Checking for collisions
        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player1.x, player1.y, player1.width, player1.height})) {
            ball.speed_x *= -1;
            PlaySound(collisionSound);
        }

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player2.x, player2.y, player2.width, player2.height})) {
            ball.speed_x *= -1;
            PlaySound(collisionSound);
        }

        // Drawing
        ClearBackground(Dark_Green);
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.Draw();
        player2.Draw();
        player1.Draw();
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }

    EndDrawing();
    CloseWindow();
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

        cv::flip(frame, frame, 1);

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
    while (true) {
        InitWindow(screenWidth, screenHeight, "Pong CV - Main Menu");
    
        int selectedOption = ShowMainMenu();

        if (selectedOption == 0) {
            CloseWindow();
            PlaySinglePlayerPong();
        } else if (selectedOption == 1) {
            CloseWindow();
            PlayMultiplayerPong();
        } else if (selectedOption == 3) {
            CloseWindow();
            return 0;
        }
    }
}


int main() {
    InitAudioDevice(); // Inicializa o subsistema de áudio do raylib

    collisionSound = LoadSound("audios/cong.mp3"); // Carrega o som de colisão
    goalSound = LoadSound("audios/goal.mp3");

    // Crie duas threads para executar as funções simultaneamente
    std::thread thread1(opencv);
    std::thread thread2(game);

    // Aguardar que as threads terminem
    thread1.join();
    thread2.join();

    std::cout << "Thread principal terminou." << std::endl;


return 0;
}