#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define STD_SIZE_X 32
#define STD_SIZE_Y 32
#define LARGURA_BALA 10
#define ALTURA_BALA 15
#define LINHAS_NAVES 4
#define COLUNAS_NAVES 7

//franca: adicionar enunciado para as telas do jogo
typedef enum {
    TELA_INICIAL,
    JOGANDO
} GameState;

typedef struct Bala {
    Rectangle pos;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
} Bala;

typedef struct Nave {
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int direcao;
    int ativa; //franca: verificar se a nave esta ativa
    int jaAtirou; //franca: verifica se o tiro da nave esta ativo
    double ultimoTiro; //franca: ativar o cooldown de disparo
} Nave;

typedef struct Heroi {
    Rectangle pos;
    Color color;
    int velocidade;
    Bala bala;
    int vidas; //franca: quantas vidas tem o heroi
} Heroi;

typedef struct Bordas {
    Rectangle pos;
} Bordas;

typedef struct Assets {
    Texture2D naveVerde;
    Texture2D heroiPng; //joto: adição de variável do tipo texture 2d para a nave do herói
    Sound tiro;
} Assets;

typedef struct Jogo {
    Nave naves[LINHAS_NAVES][COLUNAS_NAVES];
    Nave nave;
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    int alturaJanela;
    int larguraJanela;
    int tempoanimacao;
    GameState estado; //franca: adicionado por conta do enunciado
    char nomeJogador[20]; //franca: adicionado por conta do enunciado
} Jogo;

// Protótipos de funções
void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void AtualizaNavePos(Jogo *j);
void AtualizaHeroiPos(Jogo *j);
void DesenhaNaves(Jogo *j);
void DesenhaBalas(Jogo *j);
void DrawStartScreen(Jogo *j);
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
int ColisaoBalas(Jogo *j);
void AtiraBalas(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
int EndGame(Jogo *j);

//==============================================
// FUNÇÃO PRINCIPAL
//==============================================
int main() {
    InitAudioDevice();

    Jogo jogo;
    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;
    jogo.estado = TELA_INICIAL;
    strcpy(jogo.nomeJogador, "");

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders 2");
    SetTargetFPS(60);
    IniciaJogo(&jogo);
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("assets/musica.mp3");
    PlayMusicStream(musicaJogo);

    while (!WindowShouldClose()) {
        UpdateMusicStream(musicaJogo);

        if (jogo.estado == TELA_INICIAL) {
            DrawStartScreen(&jogo);
        } else if (jogo.estado == JOGANDO) {
            AtualizaFrameDesenho(&jogo);
        }
    }

    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow();
    system("clear");
    printf("Você desistiu!\n");
    exit(0);
    return 0;
}

//==============================================
// FUNÇÕES DO JOGO (TODAS IMPLEMENTADAS)
//==============================================

//franca: Inicializa o estado do jogo
void IniciaJogo(Jogo *j) {
    j->tempoanimacao = GetTime();
    j->heroi.pos = (Rectangle){LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 10;
    j->heroi.bala.ativa = 0;
    j->heroi.bala.velocidade = -9;
    j->heroi.bala.color = YELLOW;
    j->heroi.vidas = 3;

    j->nave.pos = (Rectangle){0, 15, STD_SIZE_X, STD_SIZE_Y};
    j->nave.color = RED;
    j->nave.velocidade = 3;
    j->nave.direcao = 1;
    j->nave.bala.ativa = 0;
    j->nave.bala.tempo = GetTime();
    j->nave.bala.velocidade = 10;
    j->nave.bala.tiro = LoadSound("assets/shoot.wav");

    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA - 10, 0, 10, ALTURA_JANELA};
    IniciaNaves(j);
}

//joto: Atualiza posição do herói
void AtualizaHeroiPos(Jogo *j) {
    ColisaoBordas(j);
    if (IsKeyDown(KEY_LEFT)) j->heroi.pos.x -= j->heroi.velocidade;
    if (IsKeyDown(KEY_RIGHT)) j->heroi.pos.x += j->heroi.velocidade;
}

//franca: Verifica colisão com bordas
void ColisaoBordas(Jogo *j) {
    if (CheckCollisionRecs(j->nave.pos, j->bordas[2].pos)) j->nave.direcao = 1;
    else if (CheckCollisionRecs(j->nave.pos, j->bordas[3].pos)) j->nave.direcao = 0;
    
    if (j->heroi.pos.x < j->bordas[2].pos.width) {
        j->heroi.pos.x = j->bordas[2].pos.width;
    } else if (j->heroi.pos.x + j->heroi.pos.width > j->larguraJanela - j->bordas[3].pos.width) { 
        j->heroi.pos.x = j->larguraJanela - j->heroi.pos.width - j->bordas[3].pos.width;
    }
}

//franca: Desenha naves inimigas
void DesenhaNaves(Jogo *j) {
    Vector2 tamanhoFrame = {32, 32};
    static Vector2 frame = {0, 0};
    static float tempoUltimaTroca = 0;

    if (GetTime() - tempoUltimaTroca >= 1) {
        frame.x = (frame.x == 0) ? 1 : 0;
        tempoUltimaTroca = GetTime();
    }

    Rectangle frameRecNave = {frame.x * tamanhoFrame.x, frame.y * tamanhoFrame.y, tamanhoFrame.x, tamanhoFrame.y};

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) {
                DrawTexturePro(j->assets.naveVerde, frameRecNave, j->naves[i][k].pos, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }
}

//franca: Desenha balas
void DesenhaBalas(Jogo *j) {
    if (j->heroi.bala.ativa) DrawRectangleRec(j->heroi.bala.pos, j->heroi.bala.color);
    
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].bala.ativa) {
                DrawRectangleRec(j->naves[i][k].bala.pos, j->naves[i][k].bala.color);
            }
        }
    }
}

//franca: Desenha bordas
void DesenhaBordas(Jogo *j) {
    for(int i = 0; i < 4; i++) DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
}

//franca: Lógica principal de atualização
void AtualizaJogo(Jogo *j) {
    AtualizaNavePos(j);
    AtualizaHeroiPos(j);
    AtiraBalas(j);
    ColisaoBalas(j);
    EndGame(j);
}

//franca: Desenha todos elementos do jogo
void DesenhaJogo(Jogo *j) {
    BeginDrawing();
    ClearBackground(BLACK);
    DesenhaNaves(j);
    DesenhaHeroi(j);
    DesenhaBalas(j);
    DesenhaBordas(j);
    EndDrawing();
}

//franca: Gerencia tiros
void AtiraBalas(Jogo *j) {
    double tempoAtual = GetTime();

    if (IsKeyPressed(KEY_SPACE) && !j->heroi.bala.ativa) {
        j->heroi.bala.ativa = 1;
        j->heroi.bala.pos = (Rectangle){
            j->heroi.pos.x + j->heroi.pos.width/2 - LARGURA_BALA/2,
            j->heroi.pos.y - ALTURA_BALA,
            LARGURA_BALA,
            ALTURA_BALA
        };
    }

    if (j->heroi.bala.ativa) {
        j->heroi.bala.pos.y += j->heroi.bala.velocidade;
        if (j->heroi.bala.pos.y + ALTURA_BALA < 0) j->heroi.bala.ativa = 0;
    }

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa && (tempoAtual - j->naves[i][k].ultimoTiro) >= 2.0) {
                if (GetRandomValue(0, 100) < 0.001 && !j->naves[i][k].bala.ativa) {
                    j->naves[i][k].bala.ativa = 1;
                    j->naves[i][k].bala.pos = (Rectangle){
                        j->naves[i][k].pos.x + j->naves[i][k].pos.width/2 - LARGURA_BALA/2,
                        j->naves[i][k].pos.y + j->naves[i][k].pos.height,
                        LARGURA_BALA,
                        ALTURA_BALA
                    };
                    j->naves[i][k].ultimoTiro = tempoAtual;
                }
            }

            if (j->naves[i][k].bala.ativa) {
                j->naves[i][k].bala.pos.y += j->naves[i][k].bala.velocidade;
                if (j->naves[i][k].bala.pos.y > ALTURA_JANELA) j->naves[i][k].bala.ativa = 0;
            }
        }
    }
}

//franca: Tela inicial
void DrawStartScreen(Jogo *j) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawText("SPACE INVADERS", LARGURA_JANELA/2 - MeasureText("SPACE INVADERS", 40)/2, 100, 40, GREEN);
    DrawText("Digite seu nome:", LARGURA_JANELA/2 - 120, 300, 20, WHITE);
    DrawRectangle(LARGURA_JANELA/2 - 150, 340, 300, 40, DARKGRAY);
    DrawText(j->nomeJogador, LARGURA_JANELA/2 - MeasureText(j->nomeJogador, 20)/2, 350, 20, WHITE);
    
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126 && strlen(j->nomeJogador) < 19) {
            strncat(j->nomeJogador, (char *)&key, 1);
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) j->nomeJogador[strlen(j->nomeJogador)-1] = '\0';
    
    if (strlen(j->nomeJogador)) {
        DrawText("Pressione ENTER para jogar", LARGURA_JANELA/2 - MeasureText("Pressione ENTER para jogar", 20)/2, 450, 20, GREEN);
    }

    if (IsKeyPressed(KEY_ENTER)) j->estado = JOGANDO;
    
    EndDrawing();
}

//joto: Textura do herói
void DesenhaHeroi(Jogo *j) {
    Rectangle destino = {
        j->heroi.pos.x - j->heroi.pos.width/2, 
        j->heroi.pos.y - j->heroi.pos.height/1, 
        j->heroi.pos.width * 2, 
        j->heroi.pos.height * 2
    };
    Rectangle origem = {0, 0, j->assets.heroiPng.width, j->assets.heroiPng.height};
    DrawTexturePro(j->assets.heroiPng, origem, destino, (Vector2){0,0}, 0.0f, WHITE);
}

//franca: Carrega assets
void CarregaImagens(Jogo *j) {
    j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");
    j->assets.heroiPng = LoadTexture("assets/pistola.png");
    j->assets.tiro = LoadSound("assets/shoot.wav");
}

//franca: Descarrega recursos
void DescarregaImagens(Jogo *j) {
    UnloadTexture(j->assets.naveVerde);
    UnloadTexture(j->assets.heroiPng);
    UnloadSound(j->assets.tiro);
}

//franca: Lógica de fim de jogo
int EndGame(Jogo *j) {
    if (j->heroi.vidas <= 0) {
        CloseWindow();
        system("clear");
        printf("Você perdeu!\n");
        exit(0);
    }

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) return 0;
        }
    }

    CloseWindow();
    system("clear");
    printf("Você ganhou!\n");
    exit(0);
    return 0;
}

//franca: Inicializa naves
void IniciaNaves(Jogo *j) {
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            j->naves[i][k].pos = (Rectangle){50 + k * (STD_SIZE_X + 20), 50 + i * (STD_SIZE_Y + 20), STD_SIZE_X, STD_SIZE_Y};
            j->naves[i][k].color = RED;
            j->naves[i][k].velocidade = 3;
            j->naves[i][k].direcao = 1;
            j->naves[i][k].ativa = 1;
            j->naves[i][k].jaAtirou = 0;
            j->naves[i][k].ultimoTiro = 0.0;
            j->naves[i][k].bala.ativa = 0;
            j->naves[i][k].bala.velocidade = 2; 
            j->naves[i][k].bala.color = RED;
        }
    }
}

//franca: Atualiza frame
void AtualizaFrameDesenho(Jogo *j) {
    AtualizaJogo(j);
    DesenhaJogo(j);
}

//franca: Movimento das naves
void AtualizaNavePos(Jogo *j) {
    int mudarDirecao = 0;

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) {
                j->naves[i][k].pos.x += (j->naves[i][k].direcao == 1) ? j->naves[i][k].velocidade : -j->naves[i][k].velocidade;

                if (j->naves[i][k].pos.x <= j->bordas[2].pos.width || 
                    j->naves[i][k].pos.x + j->naves[i][k].pos.width >= j->larguraJanela - j->bordas[3].pos.width) {
                    mudarDirecao = 1;
                }
            }
        }
    }

    if (mudarDirecao) {
        for (int i = 0; i < LINHAS_NAVES; i++) {
            for (int k = 0; k < COLUNAS_NAVES; k++) {
                if (j->naves[i][k].ativa) {
                    j->naves[i][k].direcao = !j->naves[i][k].direcao;
                }
            }
        }
    }
}

int ColisaoBalas(Jogo *j) {
    // Colisão da bala do herói com naves inimigas
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa && CheckCollisionRecs(j->heroi.bala.pos, j->naves[i][k].pos)) {
                j->naves[i][k].ativa = 0;       // Desativa a nave
                j->heroi.bala.ativa = 0;         // Desativa a bala do herói
                j->heroi.bala.pos = (Rectangle){-100, -100, 0, 0}; // Move a bala para fora da tela
                return 1; // Indica que uma nave foi destruída
            }
        }
    }

    // Colisão das balas inimigas com o herói
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].bala.ativa && CheckCollisionRecs(j->naves[i][k].bala.pos, j->heroi.pos)) {
                j->heroi.vidas--;                 // Remove uma vida
                j->naves[i][k].bala.ativa = 0;    // Desativa a bala inimiga
                if (j->heroi.vidas <= 0) return 2; // Derrota (sem vidas)
                return -1; // Indica que o herói perdeu uma vida
            }
        }
    }

    // Verifica se as balas inimigas saíram da tela
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].bala.ativa && 
                (j->naves[i][k].bala.pos.y < 0 || j->naves[i][k].bala.pos.y > ALTURA_JANELA)) {
                j->naves[i][k].bala.ativa = 0;
            }
        }
    }

    return 0; // Nenhuma colisão relevante
}