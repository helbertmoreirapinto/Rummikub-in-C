#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ConioC/conio.c"

#define TAM_MAX_NOME 20
#define TAM_MAX_CARTAS_BARALHO 106
#define TAM_MAX_SEQUENCIA_UNICA 13
#define TAM_MAX_SEQUENCIAS 10
#define MAX_CARTAS_MAO_JOGADOR 20
#define TIME_ENTRE_JOG 5

typedef struct{
    int nipe;
    char valor;
} Carta;

typedef struct{
    Carta carta[TAM_MAX_CARTAS_BARALHO];
    int numCartas;
} Baralho;

typedef struct{
    Carta carta[TAM_MAX_SEQUENCIA_UNICA];
    int numCartas;
} Sequencia;

typedef struct{
    Sequencia sequencia[TAM_MAX_SEQUENCIAS];
    int numSequencias;
} Mesa;

typedef struct{
    char nome[TAM_MAX_NOME];
    Carta maoJogador[MAX_CARTAS_MAO_JOGADOR];   //Modificar p/ lista ligada
    int numCartasMaoJogador;
    int primeiraJogada;
} Jogador;
typedef Jogador* Jogador_PTR;

void getCorPadrao(){
    textcolor(LIGHTGRAY);
}

void getNipe(int cor){
    switch(cor){
        case 0: textcolor(YELLOW); break;
        case 1: textcolor(LIGHTBLUE); break;
        case 2: textcolor(LIGHTGREEN); break;
        case 3: textcolor(LIGHTRED); break;
        default: textcolor(LIGHTGRAY);
    }
}

char dec_to_hexa(int dec){
    if(dec >= 0 && dec <= 9)
        return dec + '0';
    if(dec >= 10 && dec <= 15)
        return dec - 10 + 'A';
    return '\0';
}

int hexa_to_dec(char hexa){
    if(hexa >= '0' && hexa <= '9')
        return hexa - '0';
    if(hexa >= 'a'&& hexa <= 'z')
        return hexa - 'a' + 10;
    if(hexa >= 'A' && hexa <= 'Z')
        return hexa - 'A' + 10;
    return -1;
}

void exibir_baralho(Baralho b){
    int i;
    for(i = 0;i < b.numCartas; i++){
        getNipe(b.carta[i].nipe);
        printf("ind:%d carta:%c\n", i, b.carta[i].valor);
    }
    getCorPadrao();
    printf("\n");
}

void inicializar_baralho(Baralho *b){
    int i,j;
    b->numCartas = 0;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 13; j++){
            b->carta[(13 * i)+ j].nipe  = i;
            b->carta[(13 * i)+ j].valor = dec_to_hexa(j + 1);
            b->numCartas++;

            b->carta[((13 * i)+ j)+ 52].nipe = i;
            b->carta[((13 * i)+ j)+ 52].valor = dec_to_hexa(j + 1);
            b->numCartas++;
        }
    }
    b->carta[104].nipe = 4; b->carta[104].valor = 'X'; b->numCartas++;
    b->carta[105].nipe = 4; b->carta[105].valor = 'X'; b->numCartas++;
}

void remover_carta_baralho(Baralho *b, int indCartaRemover){
    int i;
    for(i = indCartaRemover; i < b->numCartas; i++){
        b->carta[i] = b->carta[i + 1];
    }
    b->numCartas--;
}

Carta pegar_carta_baralho(Baralho *b){
    int valRand = rand() % b->numCartas;
    Carta c = b->carta[valRand];
    remover_carta_baralho(b, valRand);
    return c;
}

void ordenar_cartas(Carta vet[], int tam){
    int i, j;
    Carta aux;
    for(j = tam - 1; j >= 1; j--){
        for(i = 0; i < j; i++){
            if(vet[i].valor > vet[i+1].valor){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
        }
    }
    for(j = tam - 1; j >= 1; j--){
        for(i = 0; i < j; i++){
            if(vet[i].nipe > vet[i+1].nipe){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
        }
    }
}

void inicializar_jogadores(Jogador_PTR jog, int qtdJog, Baralho *b){
    int i, j;
    for(i = 0; i < qtdJog; i++){
        printf("Nome do jogador %d [MAX %d carac]: ", (i + 1), TAM_MAX_NOME);
        setbuf(stdin,NULL);
        fgets(jog[i].nome, TAM_MAX_NOME, stdin);
        for(j = 0; j < TAM_MAX_NOME; j++){
            if(jog[i].nome[j] == '\n'){
                jog[i].nome[j] = '\0';
                break;
            }
        }
        jog[i].primeiraJogada = 1;
        jog[i].numCartasMaoJogador = 0;
        for(j = 0; j < 14; j++){
            jog[i].maoJogador[j] = pegar_carta_baralho(b);
            jog[i].numCartasMaoJogador++;
        }
        ordenar_cartas(jog[i].maoJogador, jog[i].numCartasMaoJogador);
    }
}

int finalizar_jogo(Jogador jog){
    if(jog.numCartasMaoJogador == 0)
        return 1;
    return 0;
}

void tempo_entre_jogadas(Jogador jog){
    int i;
    for(i = 0; i < TIME_ENTRE_JOG; i++){
        system("CLS");
        printf("Preparar para mostrar pecas de %s: %d seg\n",jog.nome,(TIME_ENTRE_JOG - i));
        sleep(1);
    }
    system("CLS");
}

void exibir_mao_jogador(Jogador jog){
    printf("MAO:\n");
    int i;
    for(i = 0; i < jog.numCartasMaoJogador; i++){
        getNipe(jog.maoJogador[i].nipe);
        printf("%c ",jog.maoJogador[i].valor);
    }
    getCorPadrao();
    printf("\n\n");
}

void exibir_seq_mesa(Mesa *mesa){
    int i,j;
    printf("MESA:\n");
    if(mesa->numSequencias == 0)
        printf("(Vazio)\n");
    else{
        for(i = 0; i < mesa->numSequencias; i++){
            printf("Seq %d -> ",(i+1));
            for(j = 0; j < mesa->sequencia[i].numCartas; j++){
                getNipe(mesa->sequencia[i].carta[j].nipe);
                printf("%c ", mesa->sequencia[i].carta[j].valor);
            }
            getCorPadrao();
            printf("\n");
        }
    }
    printf("\n");
}

void jogada_mesa(){
}

int jogada_mao(Jogador jog, Mesa *mesa){
    char entradaUsu[14];
    Sequencia seq;
    seq.numCartas = 0;
    int i;
    int aux;
    if(jog.primeiraJogada){
        printf("Digite o(s) indice(s) da(s) peca(s) que deseja jogar em Hexa [NNN] [SOMA MIN 30]\n");
        jog.primeiraJogada = !jog.primeiraJogada;
    } else
        printf("Digite o(s) indice(s) da(s) peca(s) que deseja jogar em Hexa [NNN]\n");
    setbuf(stdin,NULL);
    fgets(entradaUsu,16,stdin);
    for(i = 0; i < 14; i+=2){
        aux = hexa_to_dec(entradaUsu[i]);
        seq.carta[i] = jog.maoJogador[aux];
        seq.numCartas++;
        getNipe(jog.maoJogador[aux].nipe);
        printf("%c ",jog.maoJogador[aux].valor);
    }
    getCorPadrao();
    printf("\n");
    ordenar_cartas(seq.carta, seq.numCartas);
    mesa->sequencia[mesa->numSequencias++] = seq;
    system("PAUSE");
    return 0;
}

/*TO VENDO ESSE METODO*/
char realizar_jogada(Mesa *mesa, Jogador *jog, Baralho *b){
    int desceuCarta = 0;
    char entradaUsuario;
    char retorno = ' ';

    exibir_seq_mesa(mesa);
    exibir_mao_jogador(*jog);

    printf("Selecionar pecas:\n");
    printf("(1) Mesa\n");
    printf("(2) Mao\n");
    printf("(3) Encerar jogada\n");
    setbuf(stdin,NULL);
    scanf("%c", &entradaUsuario);
    switch(entradaUsuario){
        case '1': jogada_mesa(); break;
        case '2': desceuCarta = jogada_mao(*jog, mesa); break;
        case '3': retorno = 's'; break;
    }
    if(!desceuCarta){
        jog->maoJogador[jog->numCartasMaoJogador++] = pegar_carta_baralho(b);
        ordenar_cartas(jog->maoJogador, jog->numCartasMaoJogador);
        retorno = 's';
    }
    return retorno;
}

void inicializar_jogo(Mesa *mesa, Jogador_PTR jog, int qtdJog, Baralho *b){
    int i = 0, aux;
    char retornoJogada;
    do{
        tempo_entre_jogadas(jog[i]);
        do{
            system("CLS");
            printf("Vez de %s\n", jog[i].nome);
            retornoJogada = realizar_jogada(mesa, &jog[i], b);
        }while(retornoJogada != 's' && retornoJogada != 'S');
        system("PAUSE");
        aux = i; i = (i + 1 == qtdJog) ? 0 : (i + 1);
    } while(!finalizar_jogo(jog[aux]));
}

int main(){
    char entradaUsu;
    int qtdJog = 0;
    srand((unsigned) time(NULL));
    Baralho baralho;
    inicializar_baralho(&baralho);
    Mesa mesa;
    mesa.numSequencias = 0;
    do{
        printf("Quantidade de jogadores [2-4]: ");
        setbuf(stdin, NULL);
        scanf("%c", &entradaUsu);
        if(entradaUsu < '2' || entradaUsu > '4'){
            system("CLS");
            printf("Valor invalido\n");
        } else
            qtdJog = entradaUsu - '0';
    }while(entradaUsu < '2' || entradaUsu > '4');
    Jogador_PTR jog = (Jogador_PTR) malloc(qtdJog * sizeof(Jogador));
    inicializar_jogadores(jog, qtdJog, &baralho);
    inicializar_jogo(&mesa, jog, qtdJog, &baralho);
    return 0;
}
