#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define STD_SIZE_X 32
#define STD_SIZE_Y 32
#define LARGURA_BALA 10
#define ALTURA_BALA 15
#define LINHAS_NAVES 4
#define COLUNAS_NAVES 7

//franca: adicionar enunciado para as telas do jogo
typedef enum {
    TELA_INICIAL,
    JOGANDO,
    TELA_VITORIA,   //franca: tela de vitoria
    TELA_DERROTA    //franca: tela de derrota
} GameState;
//franca: leaderboard status

typedef struct {
    char nomeJogador[20];
    int pontuacao;
} Registro;

Registro leaderboard[5]; //franca: declarando variavel global

//joao: Adição da estrutura Barreira
typedef struct Barreira {
    Rectangle pos;
    int vida;
    Color color;
    int hit;
} Barreira;

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
    float velocidadeBala; //joao: Velocidade da bala do herói
} Heroi;

typedef struct Bordas {
    Rectangle pos;
} Bordas;

typedef struct Assets {
    Texture2D naveInimiga;
    Texture2D heroiPng; //joao: sprite para a nave do herói
    Texture2D background; //franca: tela de fundo
    Texture2D backgroundInicial; //joao: tela de fundo da tela inicial
    Texture2D backgroundFinal; //joao: tela de fundo da tela final
    Texture2D tiroHeroi; //joao: sprite do tiro do herói
    Texture2D tiroInimigo; //joao: sprite do tiro do inimigo
    Texture2D barreira; //joao: sprite barreira
    Sound tiro;
} Assets;

typedef struct Jogo {
    Nave naves[LINHAS_NAVES][COLUNAS_NAVES];
    Nave nave;
    Heroi heroi;
    Bordas bordas[4];
    Barreira barreiras[4]; //joao: Adição das barreiras
    Assets assets;
    int alturaJanela;
    int larguraJanela;
    int tempoanimacao;
    GameState estado; //franca: adicionado por conta do enunciado
    char nomeJogador[20]; //franca: adicionado por conta do enunciado
    int navesDestruidas; //franca: pontuacao
    float roundTime;      //joao: Duração total da rodada
    float roundTimer;     //joao: Tempo restante da rodada
    float VelocidadeInimiga;     
    float probabilidadeTiro; 
    int rodadaAtual;      
    float balainimiga; //joao: Velocidade dos tiros das naves inimigas
    
} Jogo;

// Protótipos de funções (comentários preservados)
void DrawWinScreen(Jogo *j); //franca: func criada
void DrawLoseScreen(Jogo *j); //franca: func criada
void CarregaLeaderboard(); //franca: func criada
void SalvaLeaderboard(); //franca: func criada
void AtualizaLeaderboard(const char *nomeJogador, int pontuacao); //franca: func criada
void FinalizaJogo(int pontuacao, const char *nomeJogador); //franca: func criada
void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void AtualizaNavePos(Jogo *j);
void AtualizaHeroiPos(Jogo *j); //joao: atualiza herói
void DesenhaNaves(Jogo *j);
void DesenhaBalas(Jogo *j); //franca: func criada
void DrawStartScreen(Jogo *j); //franca: func criada
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
int ColisaoBalas(Jogo *j);
void AtiraBalas(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
int EndGame(Jogo *j); //franca: acaba jogo


//==============================================
// FUNÇÃO PRINCIPAL (comentários preservados)
//==============================================


int main(){

    Jogo jogo;
    InitAudioDevice();
    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders 2");
    SetTargetFPS(60);
    IniciaJogo(&jogo);
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("../assets/musica.mp3");
    // Sound tiroSound = LoadSound("assets/shoot.wav");
    // jogo.assets.tiro = tiroSound;
    // jogo.nave.bala.tiro = tiroSound;
    
    PlayMusicStream(musicaJogo);
    CarregaLeaderboard();//franca: carregando a funcao da leaderboard

while (!WindowShouldClose()) {
    UpdateMusicStream(musicaJogo);

    switch (jogo.estado) {
        case TELA_INICIAL:
            DrawStartScreen(&jogo);
            break;
        case JOGANDO:
            AtualizaFrameDesenho(&jogo);
            break;
        case TELA_VITORIA:    //franca: novo caso
            DrawWinScreen(&jogo);
            break;
        case TELA_DERROTA:    //franca: novo caso
            DrawLoseScreen(&jogo);
            break;
    }
}
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    // UnloadSound(jogo.assets.tiro);//franca: descarrega o som de tiro
    CloseAudioDevice();
    CloseWindow();
    system("clear");//franca: limpa a tela após fechar jogo
    return 0;
}


//==============================================
// FUNÇÕES (comentários originais preservados)
//==============================================


//joao: Função para inicializar as barreiras
void IniciaBarreiras(Jogo *j) {
    srand(time(NULL)); 
    int larguraBarreira = 50;
    int alturaBarreira = 20;
    int espacamento = 180;
    int yPos = ALTURA_JANELA - 200; 
    //joao: vida aleatoria e espaçamento
    for (int i = 0; i < 4; i++) {
        j->barreiras[i].pos = (Rectangle){100 + i * espacamento, yPos, larguraBarreira, alturaBarreira};
        j->barreiras[i].vida = rand() % 11 + 5; 
        j->barreiras[i].color = GREEN;
        j->barreiras[i].hit = 0;
    }
}

//franca: Inicializa o estado do jogo
void IniciaJogo(Jogo *j) {
    j->tempoanimacao = GetTime();
    // heroi.cfg
    j->heroi.pos = (Rectangle){LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 10; //franca: Velocidade de movimentação do herói
    j->heroi.bala.ativa = 0; //joao: ativa a bala do herói
    j->heroi.bala.velocidade = -9; //joao: define a velocidade da bala
    j->heroi.bala.color = YELLOW; //joao: define a cor da bala
    j->heroi.vidas = 3; //franca: inicializa com 3 vidas
    j->heroi.velocidadeBala = -9.0f; //joao: Velocidade inicial da bala do herói
    j->balainimiga = 2.0f; //joao: Velocidade inicial dos tiros das naves inimigas
    j->nave.pos = (Rectangle){0, 15, STD_SIZE_X, STD_SIZE_Y};
    j->nave.color = RED;
    j->nave.velocidade = 3;
    /*direcao = 1 faz nave mover para direita, direcao = 0 faz mover para esquerda*/
    j->nave.direcao = 1;
    j->nave.bala.ativa = 0;
    j->nave.bala.tempo = GetTime();
    j->nave.bala.velocidade = 10;
    // j->nave.bala.tiro = LoadSound("assets/shoot.wav");
    // j->assets.tiro = LoadSound("assets/shoot.wav");
    j->roundTime = 50.0f;      //joao: Duração da rodada em segundos
    j->roundTimer = j->roundTime; 
    j->VelocidadeInimiga = 1.0f;      
    j->probabilidadeTiro = 0.01f; 
    j->rodadaAtual = 1;        

    //borda encima
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda embaixo
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA - 10, 0, 10, ALTURA_JANELA};

    IniciaNaves(j);//franca: chama a funcao pra iniciar as naves
    IniciaBarreiras(j); //joao: Inicializa as barreiras
}

//franca: Inicializa as naves inimigas
void IniciaNaves(Jogo *j) {
    srand(time(NULL)); 
    int numLinhas = rand() % LINHAS_NAVES + 1; // Gera um número aleatório de linhas (1 a 4)

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            j->naves[i][k].pos = (Rectangle){50 + k * (STD_SIZE_X + 20), 50 + i * (STD_SIZE_Y + 20), STD_SIZE_X, STD_SIZE_Y};
            j->naves[i][k].color = RED;
            j->naves[i][k].velocidade = 3;
            j->naves[i][k].direcao = 1;
            j->naves[i][k].ativa = 1; //franca: todas naves comecam ativas = 1
            j->naves[i][k].jaAtirou = 0; //franca: nenhuma nave comeca atirando
            j->naves[i][k].ultimoTiro = 0.0; //franca: inicia o temporizador

            //franca: inicializa o tiro das naves
            j->naves[i][k].bala.ativa = 0;
            j->naves[i][k].bala.velocidade = 2; 
            j->naves[i][k].bala.color = RED; 
        }
    }
     // Desativa as naves nas linhas que não foram sorteadas
     for (int i = numLinhas; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            j->naves[i][k].ativa = 0;
        }
    }
}

void ReiniciaNaves(Jogo *j) {
    srand(time(NULL)); //joao: Inicializa a semente do gerador de números aleatórios
    int numLinhas = rand() % LINHAS_NAVES + 1; //joao: Gera um número aleatório de linhas (1 a 4)

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            j->naves[i][k].pos = (Rectangle){50 + k * (STD_SIZE_X + 20), 50 + i * (STD_SIZE_Y + 20), STD_SIZE_X, STD_SIZE_Y};
            j->naves[i][k].ativa = 1; //joao: Todas as naves começam ativas
            j->naves[i][k].direcao = 1; 
            j->naves[i][k].jaAtirou = 0;
            j->naves[i][k].ultimoTiro = 0.0; 
            j->naves[i][k].bala.ativa = 0; 
        }
    }

    //joao: Desativa as naves nas linhas que não foram sorteadas
    for (int i = numLinhas; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            j->naves[i][k].ativa = 0;
        }
    }
}


//franca: Lógica principal de atualização
void AtualizaJogo(Jogo *j) {
    float deltaTime = GetFrameTime();
    j->roundTimer -= deltaTime;

    //joao: Aumenta a velocidade dos tiros das naves inimigas ao longo do tempo
    j->VelocidadeInimiga += 0.1f * deltaTime; // Aumenta 0.1 por segundo

    //joao: Limita a velocidade máxima das naves inimigas 
    if (j->VelocidadeInimiga > 8.0f) {
        j->VelocidadeInimiga = 8.0f;
    }

    //joao: Aumenta a velocidade da bala do herói ao longo do tempo
    j->heroi.velocidadeBala -= 0.05f * deltaTime; //joao: Aumenta a velocidade 

    //joao: Limita a velocidade máxima da bala do herói 
    if (j->heroi.velocidadeBala < -15.0f) {
        j->heroi.velocidadeBala = -15.0f;
    }

    //joao: Verifica se o tempo acabou
    if (j->roundTimer <= 0) {
        //joao: Verifica se ainda há naves ativas
        int navesAtivas = 0;
        for (int i = 0; i < LINHAS_NAVES; i++) {
            for (int k = 0; k < COLUNAS_NAVES; k++) {
                if (j->naves[i][k].ativa) {
                    navesAtivas = 1;
                    break;
                }
            }
            if (navesAtivas) break;
        }

        if (navesAtivas) {
            //joao: Se ainda houver naves ativas, reinicia o round atual
            ReiniciaNaves(j); 
            j->roundTimer = j->roundTime; 
           
        } else {
            //joao: Se não houver naves ativas, avança para o próximo round
            j->rodadaAtual++;
            ReiniciaNaves(j); 
            j->roundTimer = j->roundTime; 
        }
    }

    //joao: Atualiza as posições e lógicas do jogo
    AtualizaNavePos(j);
    AtualizaHeroiPos(j);
    AtiraBalas(j);
    ColisaoBalas(j);
    EndGame(j);
}

//franca: acaba jogo
int EndGame(Jogo *j) {
    //joao: Verifica se o herói perdeu todas as vidas
    if (IsKeyPressed(KEY_F2)) { //franca: hack de f2 para vitoria
        j->estado = TELA_VITORIA;
        AtualizaLeaderboard(j->nomeJogador, j->navesDestruidas);
        j->navesDestruidas = 0;
    }

    if (j->heroi.vidas <= 0) {
        j->estado = TELA_DERROTA; //franca: muda para tela de derrota
        AtualizaLeaderboard(j->nomeJogador, j->navesDestruidas); // Atualiza a leaderboard na derrota
        j->navesDestruidas = 0;
        return 1; 
    }

    //joao: Verifica se todas as naves foram destruídas
    int todasNavesDestruidas = 1; 
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) {
                todasNavesDestruidas = 0; 
                break;
            }
        }
        if (!todasNavesDestruidas) break;
    }

    if (todasNavesDestruidas) {
        if (j->rodadaAtual == 5) {
            //joao: Se for o quinto round e todas as naves forem destruídas, o jogador vence
            j->estado = TELA_VITORIA;
            AtualizaLeaderboard(j->nomeJogador, j->navesDestruidas);
            printf(" Vitória!\n");
            return 1; 
        } else {
            //joao: Reinicia as naves para o próximo round
            ReiniciaNaves(j);

            //joao: Incrementa o número do round
            j->rodadaAtual++;

            //joao: Aumenta a dificuldade para o próximo round
            j->VelocidadeInimiga += 0.5f; 
            j->probabilidadeTiro += 0.02f; 
            j->roundTimer = j->roundTime;

            //franca: Atualiza a leaderboard com a pontuação atual
            
        } 
    }
    return 0;
}

//joao: Função para desenhar as barreiras
void DesenhaBarreiras(Jogo *j) {
    for (int i = 0; i < 4; i++) {
        if (j->barreiras[i].vida > 0) {
            //joao: Define o tamanho desejado para a barreira no jogo
            Rectangle destino = {
                j->barreiras[i].pos.x, 
                j->barreiras[i].pos.y, 
                50, 
                20  
            };

            Rectangle origem = {
                0, 
                0, 
                800, 
                600
            };

            //joao: Desenha a textura redimensionada
            DrawTexturePro(j->assets.barreira, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

//franca: Desenha todos elementos do jogo
void DesenhaJogo(Jogo *j) {
    BeginDrawing();
    DrawTexture(j->assets.background, 0, 0, WHITE);
    DesenhaNaves(j);
    DesenhaHeroi(j);
    DesenhaBalas(j); //franca: balas inimigas serem desenhadas
    DesenhaBarreiras(j); //joao: Desenha as barreiras
    DesenhaBordas(j);
     //joao: Exibe o tempo restante e o número da rodada
     DrawText(TextFormat("Rodada: %d", j->rodadaAtual), 10, 10, 20, WHITE);
     DrawText(TextFormat("Tempo: %.2f", j->roundTimer), 10, 40, 20, WHITE);
    EndDrawing();
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
                // Movimenta a nave na direção atual
                j->naves[i][k].pos.x += (j->naves[i][k].direcao == 1) ? j->VelocidadeInimiga : -j->VelocidadeInimiga;

                // Verifica colisão com as bordas laterais
                if (j->naves[i][k].pos.x <= j->bordas[2].pos.width || 
                    j->naves[i][k].pos.x + j->naves[i][k].pos.width >= j->larguraJanela - j->bordas[3].pos.width) {
                    mudarDirecao = 1;
                }
            }
        }
    }

    // Se alguma nave atingiu a borda, inverte a direção de todas as naves
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

//joto: atualiza herói
void AtualizaHeroiPos(Jogo *j) {
    ColisaoBordas(j);
    if (IsKeyDown(KEY_LEFT)) {
        j->heroi.pos.x -= j->heroi.velocidade;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        j->heroi.pos.x += j->heroi.velocidade;
    }
}


//franca: Carrega assets
void CarregaImagens(Jogo *j) {
    j->assets.naveInimiga = LoadTexture("../assets/naveInimiga.png");
    j->assets.heroiPng = LoadTexture("../assets/pistola.png"); //joto: carrega a textura da nave do herói
    j->assets.tiro = LoadSound("../assets/shoot.wav"); //franca: carrega o som do tiro
    j->assets.background = LoadTexture("../assets/background.gif"); // franca: carrega o background
    j->assets.backgroundInicial = LoadTexture("../assets/background1.png"); //joao: carrega o background da tela inicial
    j->assets.backgroundFinal = LoadTexture("../assets/galaxy.png"); //joao: carrega o background da tela final
    j->assets.barreira = LoadTexture("../assets/lightsaber.png"); //joao: carrega a textura da barreira
    j->assets.tiroHeroi = LoadTexture("../assets/laserverde.png"); //joao: carrega o sprite do tiro do herói
    j->assets.tiroInimigo = LoadTexture("../assets/laservermelho.png"); //joao: carrega o sprite do tiro do heroi
    
}

//franca: Descarrega recursos
void DescarregaImagens(Jogo *j) {
    UnloadTexture(j->assets.heroiPng); //joto: descarrega a textura do herói
    // UnloadSound(j->assets.tiro); //franca: descarrega o som do tiro
    UnloadTexture(j->assets.background); //franca: descarrega o background
    UnloadTexture(j->assets.backgroundInicial); //joao: descarrega sprite background
    UnloadTexture(j->assets.backgroundFinal); //joao: descarrega sprite background
}

//franca: Desenha naves inimigas
void DesenhaNaves(Jogo *j) {
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) {
                Rectangle destino = {
                    j->naves[i][k].pos.x, 
                    j->naves[i][k].pos.y, 
                    60, 
                    50  
                };
                Rectangle origem = {
                    0, 
                    0, 
                    400, 
                    300 
                };

                //joao: Desenha a textura redimensionada
                DrawTexturePro(j->assets.naveInimiga, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }
}

//joao: Textura do herói
void DesenhaHeroi(Jogo *j) {
    Rectangle destino = {
        .x = j->heroi.pos.x - j->heroi.pos.width / 2, 
        .y = j->heroi.pos.y - j->heroi.pos.height / 1, 
        .width = j->heroi.pos.width * 2, //joao: tamanho da textura de acordo com o desenho
        .height = j->heroi.pos.height * 2
    };
    Rectangle origem = { 
        .x = 0,
        .y = 0, 
        .width = j->assets.heroiPng.width,
        .height = j->assets.heroiPng.height
    };
    Vector2 pontoOrigem = { 0, 0 }; //joao: define se a textura será desenhada sem rotação
    DrawTexturePro(j->assets.heroiPng, origem, destino, pontoOrigem, 0.0f, WHITE);
}

//franca: Desenha bordas
void DesenhaBordas(Jogo *j) {
    for(int i = 0; i < 4; i++) {
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

//franca: Desenha balas
void DesenhaBalas(Jogo *j) {
    // Desenha a bala do herói
    if (j->heroi.bala.ativa) {
        Rectangle origem = {
            0, 
            0, 
            301, 
            1101 
        };
        DrawTexturePro(j->assets.tiroHeroi, origem, j->heroi.bala.pos, (Vector2){0, 0}, 0.0f, WHITE);
    }

    //joao: Desenha as balas dos inimigos
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].bala.ativa) {
                Rectangle origem = {
                    0, 
                    0, 
                    200, 
                    280  
                };
                DrawTexturePro(j->assets.tiroInimigo, origem, j->naves[i][k].bala.pos, (Vector2){0, 0}, 0.0f, WHITE);
            }
        }
    }
}

int GeraProbabilidadeTiro() {
    return rand() % 101; 
}
//franca: Gerencia tiros
void AtiraBalas(Jogo *j) {
    double tempoAtual = GetTime();

    // Lógica para o herói atirar
    if (IsKeyPressed(KEY_SPACE)) { 
        // Só permite atirar se a bala anterior já tiver desaparecido
        if (j->heroi.bala.ativa == 0) {
            j->heroi.bala.ativa = 1;
            PlaySound(j->assets.tiro);
            j->heroi.bala.pos = (Rectangle){
                j->heroi.pos.x + j->heroi.pos.width / 2 - 5, 
                j->heroi.pos.y - 50, // Posiciona acima do herói
                10, //joao: Largura do sprite redimensionado
                50  //joao: Altura do sprite redimensionado
            };
        }
    }

    // Movimentação da bala do herói
    if (j->heroi.bala.ativa) {
        j->heroi.bala.pos.y += j->heroi.velocidadeBala; // Usa a velocidade atualizada da bala
        if (j->heroi.bala.pos.y + j->heroi.bala.pos.height < 0) j->heroi.bala.ativa = 0; //franca: Desativa a bala quando sai da tela
    }

    // Lógica para as naves inimigas atirarem
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa && (tempoAtual - j->naves[i][k].ultimoTiro) >= 3.0) {
                int podeAtirar = 1;
                for (int linhaAbaixo = i + 1; linhaAbaixo < LINHAS_NAVES; linhaAbaixo++) {
                    if (j->naves[linhaAbaixo][k].ativa) {
                        podeAtirar = 0;
                        break;
                    }
                }

                if (podeAtirar && GetRandomValue(0, 100) < (j->probabilidadeTiro * 100) && !j->naves[i][k].bala.ativa) {
                    j->naves[i][k].bala.ativa = 1;
                    PlaySound(j->assets.tiro); 
                    j->naves[i][k].bala.pos = (Rectangle){
                        j->naves[i][k].pos.x + j->naves[i][k].pos.width / 2 - 5, 
                        j->naves[i][k].pos.y + j->naves[i][k].pos.height, 
                        25, //joao: Largura do sprite redimensionado
                        50  //joao: Altura do sprite redimensionado
                    };
                    j->naves[i][k].bala.velocidade = j->balainimiga; //joao: Define a velocidade da bala
                    j->naves[i][k].ultimoTiro = tempoAtual;
                }
            }

            // Movimentação da bala da nave
            if (j->naves[i][k].bala.ativa) {
                j->naves[i][k].bala.pos.y += j->naves[i][k].bala.velocidade;
                if (j->naves[i][k].bala.pos.y > ALTURA_JANELA) j->naves[i][k].bala.ativa = 0;
            }
        }
    }
}

//franca: Verifica colisão com bordas
void ColisaoBordas(Jogo *j) {
    if (CheckCollisionRecs(j->nave.pos, j->bordas[2].pos)) {
        j->nave.direcao = 1;
    } else if (CheckCollisionRecs(j->nave.pos, j->bordas[3].pos)) {
        j->nave.direcao = 0;
    }
    
    if (j->heroi.pos.x < j->bordas[2].pos.width) {
        j->heroi.pos.x = j->bordas[2].pos.width;
    } else if (j->heroi.pos.x + j->heroi.pos.width > j->larguraJanela - j->bordas[3].pos.width) { 
        j->heroi.pos.x = j->larguraJanela - j->heroi.pos.width - j->bordas[3].pos.width;
    }
}

//franca: verifica colisões
//joao: Modificação da função ColisaoBalas para incluir colisões com as barreiras
int ColisaoBalas(Jogo *j) {
    // Colisão da bala do herói com naves inimigas
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa && CheckCollisionRecs(j->heroi.bala.pos, j->naves[i][k].pos)) {
                j->naves[i][k].ativa = 0; 
                j->heroi.bala.ativa = 0; 
                j->heroi.bala.pos = (Rectangle){-100, -100, 0, 0}; 
                j->navesDestruidas++;
                return 2; 
            }
        }
    }

    // Colisão das balas inimigas com o herói
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].bala.ativa && CheckCollisionRecs(j->heroi.pos, j->naves[i][k].bala.pos)) {
                j->heroi.vidas--; 
                j->naves[i][k].bala.ativa = 0; 

                if (j->heroi.vidas <= 0) {
                    return 1; 
                } else {
                    return 0;  
                }
            }
        }
    }
    //joao: Colisão das balas do herói com as barreiras
    for (int i = 0; i < 4; i++) {
        if (j->barreiras[i].vida > 0 && CheckCollisionRecs(j->heroi.bala.pos, j->barreiras[i].pos)) {
            j->heroi.bala.ativa = 0; //joao: Desativa a bala do herói
            j->heroi.bala.pos = (Rectangle){-100, -100, 0, 0}; 
            if (j->barreiras[i].vida <= 0) {
                
                j->barreiras[i].color = DARKGRAY; 
            }
            return 0; 
        }
    }

    //joao: Colisão das balas inimigas com as barreiras
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            for (int b = 0; b < 4; b++) {
                if (j->barreiras[b].vida > 0 && j->naves[i][k].bala.ativa && CheckCollisionRecs(j->naves[i][k].bala.pos, j->barreiras[b].pos)) {
                    j->barreiras[b].vida--;
                    j->naves[i][k].bala.ativa = 0; //joao: Desativa a bala inimiga
                    if (j->barreiras[b].vida <= 0) {
                        j->barreiras[b].color = DARKGRAY; 
                    }
                    return 0; 
                }
            }
        }
    }
    return 0; 
}


//franca: Tela inicial
void DrawStartScreen(Jogo *j) {  
    BeginDrawing();  
    ClearBackground(BLACK);  

    //joao: Desenha o fundo da tela inicial
    DrawTexture(j->assets.backgroundInicial, 0, 0, WHITE);

    //franca: desenha o título do jogo centralizado  
    DrawText("SPACE INVADERS 2", LARGURA_JANELA/2 - MeasureText("SPACE INVADERS 2", 40)/2, 100, 40, YELLOW);  
    
    //franca: desenha a interface para input do nome  
    DrawText("Digite seu nome:", LARGURA_JANELA/2 - 80, 200, 20, WHITE);  
    DrawRectangle(LARGURA_JANELA/2 - 150, 250, 300, 40, DARKGRAY); //caixa de texto  
    DrawText(j->nomeJogador, LARGURA_JANELA/2 - MeasureText(j->nomeJogador, 20)/2, 260, 20, WHITE); //texto digitado  
    
    DrawText("Scoreboard", LARGURA_JANELA/2 - MeasureText("Scoreboard", 30)/2, 350, 30, YELLOW);

    // Exibe os registros do leaderboard
    for (int i = 0; i < 5; i++) {
        char texto[50];
        sprintf(texto, "%s - %d", leaderboard[i].nomeJogador, leaderboard[i].pontuacao);
        DrawText(texto, LARGURA_JANELA/2 - MeasureText(texto, 20)/2, 400 + i * 25, 20, WHITE);
    }

    //franca: captura caracteres válidos (ASCII 32-126) e limita a 19 caracteres  
    int key = GetCharPressed();  
    while (key > 0) {  
        if (key >= 32 && key <= 126 && strlen(j->nomeJogador) < 19) {  
            strncat(j->nomeJogador, (char *)&key, 1);  
        }  
        key = GetCharPressed();  
    }  

    //franca: remove último caractere se Backspace pressionado  
    if (IsKeyPressed(KEY_BACKSPACE) && strlen(j->nomeJogador) > 0) {  
        j->nomeJogador[strlen(j->nomeJogador) - 1] = '\0';  
    }  

    //franca: exibe instrução para iniciar o jogo se nome for preenchido  
    if (strlen(j->nomeJogador) > 0) {  
        DrawText("Pressione ENTER para jogar", LARGURA_JANELA/2 - MeasureText("Pressione ENTER para jogar", 20)/2, 300, 20, YELLOW);  
    }  

    //franca: troca para o estado JOGANDO ao pressionar Enter  
    if (IsKeyPressed(KEY_ENTER)) {  
        j->estado = JOGANDO;  
    }  

    EndDrawing();  
}

//franca: Tela de vitória
void DrawWinScreen(Jogo *j) {
    BeginDrawing();
    ClearBackground(BLACK);

    //joao: Desenha o fundo da tela final
    DrawTexture(j->assets.backgroundFinal, 0, 0, WHITE);
    
    DrawText("VOCÊ GANHOU!", LARGURA_JANELA/2 - MeasureText("VOCÊ GANHOU!", 50)/2, 200, 50, YELLOW);
    DrawText("Pressione qualquer tecla para voltar ao menu", LARGURA_JANELA/2 - MeasureText("Pressione qualquer tecla para voltar ao menu", 20)/2, 400, 20, WHITE);
    DrawText("Pressione ESC para sair do jogo", LARGURA_JANELA/2 - MeasureText("Pressione ESC para sair do menu", 20)/2, 450, 20, WHITE);
    
    if (GetKeyPressed() != 0) {
        j->estado = TELA_INICIAL; //franca: volta ao menu
        IniciaJogo(j); //franca: reinicia o jogo
    }
    
    EndDrawing();
}

//franca: Tela de derrota
void DrawLoseScreen(Jogo *j) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Desenha o fundo da tela final
    DrawTexture(j->assets.backgroundFinal, 0, 0, WHITE);
    
    DrawText("VOCÊ PERDEU!", LARGURA_JANELA/2 - MeasureText("VOCÊ PERDEU!", 50)/2, 200, 50, RED);
    DrawText("Pressione qualquer tecla para voltar ao menu", LARGURA_JANELA/2 - MeasureText("Pressione qualquer tecla para voltar ao menu", 20)/2, 400, 20, YELLOW);
    DrawText("Pressione ESC para sair do jogo", LARGURA_JANELA/2 - MeasureText("Pressione ESC para sair do menu", 20)/2, 450, 20, YELLOW);
    if (GetKeyPressed() != 0) {
        j->estado = TELA_INICIAL; //franca: volta ao menu
        IniciaJogo(j); //franca: reinicia o jogo
    }
    
    EndDrawing();
}
//franca: funcao pra carregar leaderboard
void CarregaLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < 5; i++) {
            if (fscanf(file, "%s %d", leaderboard[i].nomeJogador, &leaderboard[i].pontuacao) != 2) {
                // Se o arquivo estiver corrompido ou incompleto, inicializa com valores vazios
                strcpy(leaderboard[i].nomeJogador, "");
                leaderboard[i].pontuacao = 0;
            }
        }
        fclose(file);
    } else {
        // Se o arquivo não existir, inicializa com dados vazios
        for (int i = 0; i < 5; i++) {
            strcpy(leaderboard[i].nomeJogador, "");
            leaderboard[i].pontuacao = 0;
        }
    }
}

//franca: funcao pra salvar leaderboard
void SalvaLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < 5; i++) {
            fprintf(file, "%s %d\n", leaderboard[i].nomeJogador, leaderboard[i].pontuacao);
        }
        fclose(file);
    }
}

void AtualizaLeaderboard(const char *nomeJogador, int pontuacao) {
    // Desloca os registros para baixo
    for (int i = 4; i > 0; i--) {
        strcpy(leaderboard[i].nomeJogador, leaderboard[i - 1].nomeJogador);
        leaderboard[i].pontuacao = leaderboard[i - 1].pontuacao;
    }
    // Adiciona o novo jogo na primeira posição
    strcpy(leaderboard[0].nomeJogador, nomeJogador);
    leaderboard[0].pontuacao = pontuacao;

    // Salva o novo histórico no arquivo
    SalvaLeaderboard();
}

void FinalizaJogo(int pontuacao, const char *nomeJogador) {
    AtualizaLeaderboard(nomeJogador, pontuacao);
    // Outras ações ao final do jogo, como exibir uma mensagem de derrota ou vitória
}