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

typedef struct Bala{
    Rectangle pos;
    Color color;
    int ativa;
    int tempo;
    int velocidade;
    Sound tiro;
}Bala;

typedef struct Nave{
    Rectangle pos;
    Color color;
    Bala bala;
    int velocidade;
    int direcao;
    int ativa; //franca: verificar se a nave esta ativa
    int jaAtirou; //franca: verifica se o tiro da nave esta ativo
}Nave;

typedef struct Heroi{
    Rectangle pos;
    Color color;
    int velocidade;
    Bala bala;
    int vidas; //franca: quantas vidas tem o heroi
}Heroi;

typedef struct Bordas{
    Rectangle pos;
} Bordas;

typedef struct Assets{
    Texture2D naveVerde;
    Texture2D heroiPng; //joto: adição de variável do tipo texture 2d para a nave do herói
    Sound tiro;
}Assets;

typedef struct Jogo{
    Nave naves[LINHAS_NAVES][COLUNAS_NAVES];
    Nave nave;
    Heroi heroi;
    Bordas bordas[4];
    Assets assets;
    int alturaJanela;
    int larguraJanela;
    int tempoanimacao;
}Jogo;

void IniciaJogo(Jogo *j);
void IniciaNaves(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void AtualizaNavePos(Jogo *j);
void AtualizaHeroiPos(Jogo *j); //joto: atualiza herói
void DesenhaNaves(Jogo *j);
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
int ColisaoBalas(Jogo *j);
void DesenhaBordas(Jogo *j);
void AtiraBalas(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
int EndGame(Jogo *j); //franca: acaba jogo

int main(){
    InitAudioDevice();

    Jogo jogo;

    jogo.alturaJanela = ALTURA_JANELA;
    jogo.larguraJanela = LARGURA_JANELA;
//
    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders 2");
    SetTargetFPS(60);
    IniciaJogo(&jogo);
    CarregaImagens(&jogo);
    Music musicaJogo = LoadMusicStream("assets/musica.mp3");
    PlayMusicStream(musicaJogo);
//
    while(!WindowShouldClose()){
        UpdateMusicStream(musicaJogo);
        AtualizaFrameDesenho(&jogo);
        
    }
    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow();
    system("clear");//Franca: Limpa a tela após fechar jogo
    printf("Você desistiu!\n"); 
    exit(0);
    return 0;
}

void IniciaJogo(Jogo *j){
    j->tempoanimacao = GetTime();
    // heroi.cfg
    j->heroi.pos = (Rectangle) {LARGURA_JANELA/2 - STD_SIZE_X/2, ALTURA_JANELA - STD_SIZE_Y -10, STD_SIZE_X, STD_SIZE_Y};
    j->heroi.color = BLUE;
    j->heroi.velocidade = 10; //franca: Velocidade de movimentação do herói
    j->heroi.bala.ativa = 0; //joto: ativa a bala do herói
    j->heroi.bala.velocidade = -9; //joto: define a velocidade da bala
    j->heroi.bala.color = YELLOW; //joto: define a cor da bala
    j->heroi.vidas = 4; //franca: inicializa com 3 vidas


    
    j->nave.pos = (Rectangle) {0, 15, STD_SIZE_X, STD_SIZE_Y};
    j->nave.color = RED;
    j->nave.velocidade = 3;
    /*direcao = 1 faz nave mover para direita, direcao = 0 faz mover para esquerda*/
    j->nave.direcao = 1;
    j->nave.bala.ativa = 0;
    j->nave.bala.tempo = GetTime();
    j->nave.bala.velocidade = 10;
    j->nave.bala.tiro = LoadSound("assets/shoot.wav");

    //borda encima
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda embaixo
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA};
    IniciaNaves(j);//franca: chama a funcao pra iniciar as naves
}

void IniciaNaves(Jogo *j){//franca: funcao que inicia todas as naves
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            j->naves[i][k].pos = (Rectangle){50 + k * (STD_SIZE_X + 20), 50 + i * (STD_SIZE_Y + 20), STD_SIZE_X, STD_SIZE_Y};
            j->naves[i][k].color = RED;
            j->naves[i][k].velocidade = 3;
            j->naves[i][k].direcao = 1;
            j->naves[i][k].ativa = 1; //franca: todas naves comecam ativas = 1
            j->naves[i][k].jaAtirou = 0; //franca: nenhuma nave comeca atirando
        }
    }
}

void AtualizaJogo(Jogo *j){
    AtualizaNavePos(j);
    AtualizaHeroiPos(j);//
    AtiraBalas(j);
    EndGame(j);
}

int EndGame(Jogo *j) {
    int resultadoColisao = ColisaoBalas(j);

    // Verifica se o herói perdeu todas as vidas
    if (resultadoColisao == 1) {
        CloseWindow();
        system("clear"); // Limpa a tela
        printf("Você perdeu!\n");
        exit(0);
    }

    // Verifica se todas as naves foram destruídas
    int todasNavesDestruidas = 1; // Assume que todas as naves foram destruídas
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) {
                todasNavesDestruidas = 0; // Ainda há naves ativas
                break;
            }
        }
        if (!todasNavesDestruidas) {
            break;
        }
    }

    // Se todas as naves foram destruídas, o jogador venceu
    if (todasNavesDestruidas) {
        CloseWindow();
        system("clear"); // Limpa a tela
        printf("Você ganhou!\n");
        exit(0);
    }

    return 0; // O jogo continua
}

void DesenhaJogo(Jogo *j){
    BeginDrawing();
    ClearBackground(BLACK);
    DesenhaNaves(j);
    DesenhaHeroi(j);
    DesenhaBordas(j);
    EndDrawing();
}

void AtualizaFrameDesenho(Jogo *j){
    AtualizaJogo(j);
    DesenhaJogo(j);
}

void AtualizaNavePos(Jogo *j) {//deep
    int mudarDirecao = 0; // 0 = não mudar direção, 1 = mudar direção

    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa) { // Verifica se a nave está ativa
                // Move a nave
                if (j->naves[i][k].direcao == 1) {
                    j->naves[i][k].pos.x += j->naves[i][k].velocidade;
                } else {
                    j->naves[i][k].pos.x -= j->naves[i][k].velocidade;
                }

                // Verifica se a nave atingiu a borda esquerda ou direita
                if (j->naves[i][k].pos.x <= j->bordas[2].pos.width || 
                    j->naves[i][k].pos.x + j->naves[i][k].pos.width >= j->larguraJanela - j->bordas[3].pos.width) {
                    mudarDirecao = 1; // Indica que a direção deve ser mudada
                }
            }
        }
    }

    // Se alguma nave atingiu a borda, muda a direção de todas as naves
    if (mudarDirecao == 1) {
        for (int i = 0; i < LINHAS_NAVES; i++) {
            for (int k = 0; k < COLUNAS_NAVES; k++) {
                if (j->naves[i][k].ativa) {
                    j->naves[i][k].direcao = !j->naves[i][k].direcao; // Inverte a direção
                }
            }
        }
    }
}

void AtualizaHeroiPos(Jogo *j){
    ColisaoBordas(j);
    if (IsKeyDown(KEY_LEFT)) {
        j->heroi.pos.x -= j->heroi.velocidade;
    }

    if (IsKeyDown(KEY_RIGHT)) {
        j->heroi.pos.x += j->heroi.velocidade;
    }


}

void CarregaImagens(Jogo *j){
    j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");
    j->assets.heroiPng = LoadTexture("assets/pistola.png"); //joto: carrega a textura da nave do herói
}

void DescarregaImagens(Jogo *j){
    UnloadTexture(j->assets.naveVerde);
    UnloadTexture(j->assets.heroiPng); //joto: descarrega a textura do herói
}

void DesenhaNaves(Jogo *j) {//deep
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

void DesenhaHeroi(Jogo *j){
    //joto: hitbox do herói
    
    Rectangle destino = { //joto: define onde a textura será desenhada
        .x = j->heroi.pos.x - j->heroi.pos.width / 2, 
        .y = j->heroi.pos.y - j->heroi.pos.height / 1, 
        .width = j->heroi.pos.width * 2, //joto: tamanho da textura de acordo com o desenho(aumentada 2x)
        .height = j->heroi.pos.height * 2
    };
    Rectangle origem = { //joto: define se a textura inteira será usada
    //joto: !!Sprites animados são modificados aqui!!
        .x = 0,
        .y = 0, 
        .width = j->assets.heroiPng.width,
        .height = j->assets.heroiPng.height
    };
    Vector2 pontoOrigem = { 0, 0 }; //joto: define se a textura será desenhada sem rotação
    DrawTexturePro( //joto: Parâmetros para desenhar a textura
        j->assets.heroiPng, 
        origem, 
        destino, 
        pontoOrigem, 
        0.0f,  
        WHITE   
    );
}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void DesenhaBalas(Jogo *j){
    DrawRectangleRec(j->nave.bala.pos, YELLOW);
}

void AtiraBalas(Jogo *j){
    if(j->nave.bala.ativa == 0 && GetTime()-j->nave.bala.tempo > 3){
        j->nave.bala.pos = (Rectangle){j->nave.pos.x+j->nave.pos.width/2, j->nave.pos.y+j->nave.pos.height/2, 
        LARGURA_BALA, ALTURA_BALA};
        j->nave.bala.ativa = 1;
        j->nave.bala.tempo = GetTime();
        PlaySound(j->nave.bala.tiro);
    }
    else if(ColisaoBalas(j)){
        j->nave.bala.ativa = 0;
        //franca: bugfix: fazer com q a bala vá para a posicao da nave
        j->nave.bala.pos = (Rectangle) {
        j->nave.pos.x + j->nave.pos.width / 2 - LARGURA_BALA / 2, 
        j->nave.pos.y + j->nave.pos.height / 2, 
        LARGURA_BALA, 
        ALTURA_BALA
    };
    }
    if(j->nave.bala.ativa == 1){
        j->nave.bala.pos.y += j->nave.bala.velocidade;
        DesenhaBalas(j);
    }

    //joto: função para o herói atirar as balas

    if (IsKeyPressed(KEY_SPACE) && !j->heroi.bala.ativa) { 
        //joto: retorna true se a tecla foi pressionada e impede de atirar mais de uma bala por vez
        j->heroi.bala.pos = (Rectangle) {
            j->heroi.pos.x + j->heroi.pos.width / 2 - LARGURA_BALA / 2,
            //joto: posiciona a bala no centro horizontal do herói
            j->heroi.pos.y - ALTURA_BALA, //joto: posiciona a bala no centro vertical do herói, acima dele
            LARGURA_BALA,
            ALTURA_BALA
            //joto: define o tamanho da bala
        };
        j->heroi.bala.ativa = 1; //joto: bala ativa
    }
    if (j->heroi.bala.ativa) { //joto: se bala ativa...
        j->heroi.bala.pos.y += j->heroi.bala.velocidade; //joto: move a bala na vertical
        if (j->heroi.bala.pos.y + ALTURA_BALA < 0) {  //joto: verifica se bala saiu da tela
            j->heroi.bala.ativa = 0;
        }
        DrawRectangleRec(j->heroi.bala.pos, j->heroi.bala.color); //joto: desenho da bala
    }
}


void ColisaoBordas(Jogo *j){
    if(CheckCollisionRecs(j->nave.pos, j->bordas[2].pos)){
        j->nave.direcao = 1;
    }else if(CheckCollisionRecs(j->nave.pos, j->bordas[3].pos)){
        j->nave.direcao = 0;
    }
     if (j->heroi.pos.x < j->bordas[2].pos.width) {
        j->heroi.pos.x = j->bordas[2].pos.width;
    } else if (j->heroi.pos.x + j->heroi.pos.width > j->larguraJanela - j->bordas[3].pos.width) { 
        j->heroi.pos.x = j->larguraJanela - j->heroi.pos.width - j->bordas[3].pos.width;
    }
}

int ColisaoBalas(Jogo *j) {
    // Verifica colisão da bala do herói com as naves inimigas
    for (int i = 0; i < LINHAS_NAVES; i++) {
        for (int k = 0; k < COLUNAS_NAVES; k++) {
            if (j->naves[i][k].ativa && CheckCollisionRecs(j->heroi.bala.pos, j->naves[i][k].pos)) {
                j->naves[i][k].ativa = 0; // Desativa a nave inimiga
                j->heroi.bala.ativa = 0;  // Desativa a bala do herói
                // Reseta a posição da bala do herói
                j->heroi.bala.pos = (Rectangle){
                    j->heroi.pos.x + j->heroi.pos.width / 2 - LARGURA_BALA / 2,
                    j->heroi.pos.y - ALTURA_BALA,
                    LARGURA_BALA,
                    ALTURA_BALA
                };
                return 2; // Indica que uma nave foi atingida
            }
        }
    }

    // Verifica colisão da bala da nave inimiga com o herói
    if (CheckCollisionRecs(j->heroi.pos, j->nave.bala.pos)) {
        j->heroi.vidas--; // Retira uma vida do herói
        j->nave.bala.ativa = 0; // Desativa a bala da nave inimiga

        if (j->heroi.vidas <= 0) {
            return 1; // Derrota (vidas do herói acabaram)
        }
        return -1; // Herói perdeu uma vida, mas ainda está no jogo
    }

    // Verifica colisão da bala da nave inimiga com a borda inferior
    if (CheckCollisionRecs(j->nave.bala.pos, j->bordas[1].pos)) {
        j->nave.bala.ativa = 0; // Desativa a bala da nave inimiga
        return -1; // Bala da nave inimiga atingiu a borda inferior
    }

    return 0; // Nenhuma colisão relevante ocorreu
}