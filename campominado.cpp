#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void habilitar_ansi()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return;
    }
}

typedef struct
{
    bool tem_bomba;
    bool tem_bandeira;
    bool foi_aberto;
    int bombas_redor;
} TIJOLO;

typedef struct
{
    int altura;
    int largura;
    int quantidade_bombas;
    TIJOLO **tabuleiro;
} TABULEIRO;

void mostrar_tijolo(TIJOLO tijolo)
{
    char texto_tijolo[100] = "";
    if (!tijolo.tem_bomba && tijolo.foi_aberto)
    {
        switch (tijolo.bombas_redor)
        {
            default:
            case 0:
                strcat(texto_tijolo, "\x1B[90m - \x1B[0m");
                break;
            case 1:
                strcat(texto_tijolo, "\x1B[94m 1 \x1B[0m");
                break;
            case 2:
                strcat(texto_tijolo, "\x1B[92m 2 \x1B[0m");
                break;
            case 3:
                strcat(texto_tijolo, "\x1B[31m 3 \x1B[0m");
                break;
            case 4:
                strcat(texto_tijolo, "\x1B[34m 4 \x1B[0m");
                break;
            case 5:
                strcat(texto_tijolo, "\x1B[95m 5 \x1B[0m");
                break;
            case 6:
                strcat(texto_tijolo, "\x1B[96m 6 \x1B[0m");
                break;
            case 7:
                strcat(texto_tijolo, "\x1B[93m 7 \x1B[0m");
                break;
            case 8:
                strcat(texto_tijolo, "\x1B[95m 8 \x1B[0m");
                break;
        }
    }
    else if (tijolo.tem_bandeira)
    {
        strcpy(texto_tijolo, " \x1B[90m|\x1B[91m>\x1B[0m");
    }
    else
    {
        strcpy(texto_tijolo, " ? ");
    }

    printf("%s", texto_tijolo);
}

void colocar_bombas(TABULEIRO* tabuleiro)
{
    int segundos = time(0);
    srand(segundos);

    int bombas_a_colocar = tabuleiro->quantidade_bombas;

    while (true)
    {
        int x = rand() % tabuleiro->altura;
        int y = rand() % tabuleiro->largura;

        if (tabuleiro->tabuleiro[x][y].tem_bomba)
        {
            continue;
        }

        tabuleiro->tabuleiro[x][y].tem_bomba = true;

        bombas_a_colocar--;

        if (bombas_a_colocar == 0)
        {
            break;
        }
    }
}

bool posicao_valida(int x, int y, TABULEIRO tabuleiro)
{
    return x >= 0 && y >= 0 && x < tabuleiro.largura && y < tabuleiro.altura;
}

void calcular_numero_bombas_tijolo(TABULEIRO* tabuleiro, int i, int j)
{
    int quantidade_bombas = 0;
    for (int xbomb = j - 1; xbomb <= j + 1; xbomb++)
    {
        for (int ybomb = i - 1; ybomb <= i + 1; ybomb++)
        {
            if (!posicao_valida(xbomb, ybomb, *tabuleiro))
            {
                continue;
            }
            if (tabuleiro->tabuleiro[ybomb][xbomb].tem_bomba)
            {
                quantidade_bombas++;
            }
        }
    }

    tabuleiro->tabuleiro[i][j].bombas_redor = quantidade_bombas;
}

void calcular_numeros_bombas_tabuleiro(TABULEIRO* tabuleiro)
{
    for (int i = 0; i < tabuleiro->altura; i++)
    {
        for (int j = 0; j < tabuleiro->largura; j++)
        {
            calcular_numero_bombas_tijolo(tabuleiro, i, j);
        }
    }
}

TABULEIRO iniciar_tabuleiro(int altura, int largura, int quantidade_bombas)
{
    TABULEIRO tabuleiro;
    tabuleiro.altura = altura;
    tabuleiro.largura = largura;
    tabuleiro.quantidade_bombas = quantidade_bombas;

    tabuleiro.tabuleiro = (TIJOLO **)malloc(sizeof(TIJOLO *) * altura);
    
    for(int i=0; i < altura; i++)
    {
        tabuleiro.tabuleiro[i] = (TIJOLO*)malloc(largura * sizeof(TIJOLO)) ;
    }
    
    for(int i=0; i < altura; i++)
    {
        for(int j=0; j < largura; j++)
        {
            tabuleiro.tabuleiro[i][j].tem_bomba = false;
            tabuleiro.tabuleiro[i][j].tem_bandeira = false;
            tabuleiro.tabuleiro[i][j].foi_aberto = false;
            tabuleiro.tabuleiro[i][j].bombas_redor = 0;
        }
    }

    colocar_bombas(&tabuleiro);

    calcular_numeros_bombas_tabuleiro(&tabuleiro);

    return tabuleiro;
}

void mostrar_tabuleiro(TABULEIRO tabuleiro)
{
    for(int i = 0; i <= tabuleiro.largura; i++)
    {
        if (i == 0)
        {
            printf(" ");
        }
        else
        {
            printf("%2d ", i);
        }
    }
    printf("\n");
    for(int i = 0; i < tabuleiro.altura; i++)
    {
        printf("%c",  i + 'a');
        for(int j = 0; j < tabuleiro.largura; j++)
        {
            mostrar_tijolo(tabuleiro.tabuleiro[i][j]);
        }
        printf("\n");
    }
}

void liberar_tabuleiro(TABULEIRO* tabuleiro)
{
    for(int i = 0; i < tabuleiro->altura; i++)
    {
        free(tabuleiro->tabuleiro[i]);
    }
    free(tabuleiro->tabuleiro);
}

void pegar_posicao(int* x, int* y, TABULEIRO tabuleiro)
{
    fflush(stdin);
    printf("\n\nDigite o espaco que deseja jogar (ex: b3): ");
    int x_digitado = 0;
    char y_caracter = ' ';
    scanf(" %c%d", &y_caracter, &x_digitado);
    x_digitado--;
    int y_digitado = y_caracter - 'a';

    bool digitou_corretamente = posicao_valida(x_digitado, y_digitado, tabuleiro);
    while (!digitou_corretamente)
    {
        x_digitado = 0;
        y_caracter = ' ';
        fflush(stdin);
        printf("A posicao que voce escolheu esta fora do tabuleiro, digite novamente: ");
        scanf(" %c%d", &y_caracter, &x_digitado);
        x_digitado--;
        y_digitado = y_caracter - 'a';

        digitou_corretamente = posicao_valida(x_digitado, y_digitado, tabuleiro);
    }

    *x = x_digitado;
    *y = y_digitado;
}

int abrir(int x, int y, TABULEIRO* tabuleiro);

void abrir_proximos(int x, int y, TABULEIRO* tabuleiro)
{
    for (int x_bomba = x - 1; x_bomba <= x + 1; x_bomba++)
    {
        for (int y_bomba = y - 1; y_bomba <= y + 1; y_bomba++)
        {
            if (posicao_valida(x_bomba, y_bomba, *tabuleiro))
            {
                abrir(x_bomba, y_bomba, tabuleiro);
            }
        }
    }
}

int abrir(int x, int y, TABULEIRO* tabuleiro)
{
    TIJOLO* tijolo = &tabuleiro->tabuleiro[y][x];

    if (tijolo->tem_bandeira)
    {
        printf("\nEste tijolo tem bandeira e nao pode ser aberto\n");
        return true;
    }

    if (tijolo->tem_bomba)
    {
        tijolo->foi_aberto = true;
        printf("\nO tijolo tem uma bomba e voce perdeu\n");
        return false;
    }

    if (tijolo->bombas_redor == 0 && !tijolo->foi_aberto)
    {
        tijolo->foi_aberto = true;
        abrir_proximos(x, y, tabuleiro);
    }

    tijolo->foi_aberto = true;
    return true;
}

bool pegar_e_rodar_acao(int x, int y, TABULEIRO* tabuleiro)
{
    char acao[50] = "";
    fflush(stdin);
    printf("\n\nDigite a acao que deseja jogar (abrir, bandeira, cancelar): ");
    scanf("%s", &acao);

    bool digitou_corretamente = strcmp(acao, "abrir") == 0 || strcmp(acao, "bandeira") == 0 || strcmp(acao, "cancelar") == 0;
    while (!digitou_corretamente)
    {
        fflush(stdin);
        printf("A acao que voce escolheu nao existe, digite novamente (abrir, bandeira, cancelar): ");
        scanf("%s", &acao);

        digitou_corretamente = strcmp(acao, "abrir") == 0 || strcmp(acao, "bandeira") == 0 || strcmp(acao, "cancelar") == 0;
    }


    if (strcmp(acao, "abrir") == 0)
    {
        return abrir(x, y, tabuleiro);
    }
    else if (strcmp(acao, "bandeira") == 0)
    {
        TIJOLO* tijolo = &tabuleiro->tabuleiro[y][x];
        if (tijolo->foi_aberto)
        {
            printf("\nEste tijolo já foi aberto e nao pode ter bandeira\n");
        }

        tijolo->tem_bandeira = !tijolo->tem_bandeira;
    }

    return true;
}

bool validar_vitoria(TABULEIRO tabuleiro)
{
    int bombas_marcadas = 0;
    int tijolos_abertos = 0;
    for(int i = 0; i < tabuleiro.altura; i++)
    {
        for(int j = 0; j < tabuleiro.largura; j++)
        {
            if (tabuleiro.tabuleiro[i][j].tem_bomba && tabuleiro.tabuleiro[i][j].tem_bandeira)
            {
                bombas_marcadas++;
            }

            if (!tabuleiro.tabuleiro[i][j].tem_bomba && !tabuleiro.tabuleiro[i][j].tem_bandeira && tabuleiro.tabuleiro[i][j].foi_aberto)
            {
                tijolos_abertos++;
            }
        }
    }

    bool marcou_todas_bombas = bombas_marcadas == tabuleiro.quantidade_bombas;

    bool abriu_todos_espacos = (tabuleiro.altura * tabuleiro.largura - tabuleiro.quantidade_bombas) == tijolos_abertos;

    return marcou_todas_bombas || abriu_todos_espacos;
}

TABULEIRO escolher_dificuldade()
{
    char dificuldade[50] = "";
    fflush(stdin);
    printf("\n\nDigite a dificuldade que deseja jogar (facil, medio, dificil): ");
    scanf("%s", &dificuldade);

    bool digitou_corretamente = strcmp(dificuldade, "facil") == 0 || strcmp(dificuldade, "medio") == 0 || strcmp(dificuldade, "dificil") == 0;
    while (!digitou_corretamente)
    {
        fflush(stdin);
        printf("A dificuldade que voce escolheu nao existe, digite novamente (facil, medio, dificil): ");
        scanf("%s", &dificuldade);

        digitou_corretamente = strcmp(dificuldade, "facil") == 0 || strcmp(dificuldade, "medio") == 0 || strcmp(dificuldade, "dificil") == 0;
    }

    if (strcmp(dificuldade, "dificil") == 0)
    {
        return iniciar_tabuleiro(16, 30, 99);
    }

    if (strcmp(dificuldade, "medio") == 0)
    {
        return iniciar_tabuleiro(16, 16, 40);
    }

    // facil
    return iniciar_tabuleiro(9, 9, 10);
}

void mostrar_logo()
{
    printf("\x1B[0m-------------------------------------------------------------------------------------------------------------------------\n\x1B[0m");
    printf("\x1B[0m|\x1B[91m   ______                                                     __       __  __                            __            \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m  /      \\                                                   /  \\     /  |/  |                          /  |           \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m /$$$$$$  |  ______   _____  ____    ______    ______        $$  \\   /$$ |$$/  _______    ______    ____$$ |  ______   \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m $$ |  $$/  /      \\ /     \\/    \\  /      \\  /      \\       $$$  \\ /$$$ |/  |/       \\  /      \\  /    $$ | /      \\  \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m $$ |       $$$$$$  |$$$$$$ $$$$  |/$$$$$$  |/$$$$$$  |      $$$$  /$$$$ |$$ |$$$$$$$  | $$$$$$  |/$$$$$$$ |/$$$$$$  | \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m $$ |   __  /    $$ |$$ | $$ | $$ |$$ |  $$ |$$ |  $$ |      $$ $$ $$/$$ |$$ |$$ |  $$ | /    $$ |$$ |  $$ |$$ |  $$ | \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m $$ \\__/  |/$$$$$$$ |$$ | $$ | $$ |$$ |__$$ |$$ \\__$$ |      $$ |$$$/ $$ |$$ |$$ |  $$ |/$$$$$$$ |$$ \\__$$ |$$ \\__$$ | \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m $$    $$/ $$    $$ |$$ | $$ | $$ |$$    $$/ $$    $$/       $$ | $/  $$ |$$ |$$ |  $$ |$$    $$ |$$    $$ |$$    $$/  \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m  $$$$$$/   $$$$$$$/ $$/  $$/  $$/ $$$$$$$/   $$$$$$/        $$/      $$/ $$/ $$/   $$/  $$$$$$$/  $$$$$$$/  $$$$$$/   \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m                                   $$ |                                                                                \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m                                   $$ |                                                                                \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m                                   $$/                                                                                 \x1B[0m|\n");
    printf("\x1B[0m|\x1B[91m                                                                                                                       \x1B[0m|\n");
    printf("\x1B[0m|\x1B[94m                                              Projeto realizado por Fernando Galvao, Kimberly Rayanne e Carlos Eduardo \x1B[0m|\n");
    printf("\x1B[0m-------------------------------------------------------------------------------------------------------------------------\n\x1B[0m");
}

int main()
{
    habilitar_ansi();

    mostrar_logo();

    TABULEIRO tabuleiro = escolher_dificuldade();

    int acoes = 0;
    bool jogo_esta_rodando = true;
    while (jogo_esta_rodando)
    {
        mostrar_tabuleiro(tabuleiro);

        int x, y = 0;
        pegar_posicao(&x, &y, tabuleiro);

        jogo_esta_rodando = pegar_e_rodar_acao(x, y, &tabuleiro);

        acoes++;

        if (jogo_esta_rodando && validar_vitoria(tabuleiro))
        {
            printf("\nParabens, voce ganhou em %d acoes!!!!!\n", acoes);
            jogo_esta_rodando = false;
        }
    }

    liberar_tabuleiro(&tabuleiro);
}
