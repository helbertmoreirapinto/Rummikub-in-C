#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ConioC/conio.c"

#define TAM_MAX_NOME 20
#define TAM_MAX_CARTAS_BARALHO 106
#define TIME_ENTRE_JOG 0

typedef struct{
    int nipe;
    char valor;
} Carta;

typedef struct{
    Carta carta[TAM_MAX_CARTAS_BARALHO];
    int numCartas;
} Baralho;

typedef struct{
    Carta *carta;
    int numCartas;
} Sequencia;

typedef struct{
    Sequencia *sequencia;
    int numSequencias;
    Sequencia *temp;
    int numTemp;
} Mesa;

typedef struct{
    char nome[TAM_MAX_NOME+1];
    Carta *maoJogador;
    int numCartasMaoJogador;
    int primeiraJogada;
} Jogador;
typedef Jogador* Jogador_PTR;

void getCorPadrao(){
    textcolor(LIGHTGRAY);
}

void getNipe(int cor){
    switch(cor){
        case 0: textcolor(LIGHTBLUE); break;
        case 1: textcolor(LIGHTGREEN); break;
        case 2: textcolor(YELLOW); break;
        case 3: textcolor(LIGHTRED); break;
        default: textcolor(WHITE);
    }
}

/* Prototipo de funcoes */
int validar_inidice(char*);
int validar_jogada(Sequencia*);
Sequencia* sequencia_mao(Carta*, char*);
char dec_to_hexa(int);
void error_mem();

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
    for(j = tam - 1; j >= 1; j--)
        for(i = 0; i < j; i++)
            if(vet[i].valor > vet[i+1].valor){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
    for(j = tam - 1; j >= 1; j--)
        for(i = 0; i < j; i++)
            if(vet[i].nipe > vet[i+1].nipe){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
}

void inicializar_jogadores(Jogador_PTR jog, int qtdJog, Baralho *b){
    int i, j;
    for(i = 0; i < qtdJog; i++){
        printf("Nome do jogador %d [MAX %d carac]: ", (i + 1), TAM_MAX_NOME);
        setbuf(stdin,NULL);
        fgets(jog[i].nome, TAM_MAX_NOME+1, stdin);
        for(j = 0; j < TAM_MAX_NOME; j++){
            if(jog[i].nome[j] == '\n'){
                jog[i].nome[j] = '\0';
                break;
            }
        }

        jog[i].primeiraJogada = 1;
        jog[i].numCartasMaoJogador = 0;
        jog[i].maoJogador = NULL;
        for(j = 0; j < 14; j++){
            jog[i].numCartasMaoJogador++;
            jog[i].maoJogador = (Carta *)realloc(jog[i].maoJogador, jog[i].numCartasMaoJogador*sizeof(Carta));
            if(!jog->maoJogador)error_mem();
            jog[i].maoJogador[jog[i].numCartasMaoJogador-1] = pegar_carta_baralho(b);
        }
        ordenar_cartas(jog[i].maoJogador, jog[i].numCartasMaoJogador);
    }
}

int finalizar_jogo(Jogador_PTR jog){
    if(!jog->numCartasMaoJogador)
        return 1;
    return 0;
}

void tempo_entre_jogadas(Jogador_PTR jog){
    int i;
    for(i = 0; i < TIME_ENTRE_JOG; i++){
        system("CLS");
        printf("Preparar para mostrar pecas de %s: %d seg\n",jog->nome,(TIME_ENTRE_JOG - i));
        Sleep(1000);
    }
    system("CLS");
}

void exibir_mao_jogador(Jogador_PTR jog){
    printf("MAO:\n");
    int i;
    char var = 'A';
    for(i = 0; i < jog->numCartasMaoJogador; i++){
        getNipe(jog->maoJogador[i].nipe);
        printf("%c ",jog->maoJogador[i].valor);
    }
    getCorPadrao();
    printf("\n");
    for(i = 0; i < jog->numCartasMaoJogador; i++)
        printf("%c ", var++);
    printf("\n\n");
}

void exibir_seq_mesa(Mesa *mesa){
    int i,j;
    printf("MESA:\n");
    if(!mesa->numSequencias && !mesa->numTemp)
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
        for(i = 0; i < mesa->numTemp; i++){
            printf("Temp %d -> ",(i+1));
            for(j = 0; j < mesa->temp[i].numCartas; j++){
                getNipe(mesa->temp[i].carta[j].nipe);
                printf("%c ", mesa->temp[i].carta[j].valor);
            }
            getCorPadrao();
            printf("\n");
        }
    }
    printf("\n");
}

int getIndice(char c){
    return c -'A';
}

/*TO VENDO ESSE METODO*/
void realizar_jogada(Mesa *mesa, Jogador_PTR jog, Baralho *b){
    char entradaUsuario;
    int seqUsu;
    int auxNumMaoJogador = jog->numCartasMaoJogador;
    int entradaValida;
    char seqJogada[auxNumMaoJogador];
    int indice;
    int i;
    Sequencia *seqTemp;
    mesa->temp = NULL;
    mesa->numTemp = 0;
    do{
        entradaValida = 0;
        entradaUsuario = '\0';
        seqUsu = -1;
        system("CLS");
        printf("Vez de %s\n", jog->nome);
        exibir_seq_mesa(mesa);
        exibir_mao_jogador(jog);

        printf("|**********************|\n");
        printf("|Selecionar pecas:     |\n");
        printf("|**********************|\n");        if(mesa->numSequencias || mesa->numTemp)
            printf("|Mesa              | 1 |\n");
        printf("|Mao               | 2 |\n");
        printf("|Encerar jogada    | 3 |\n");        printf("|**********************|\n");

        do{
            printf("|Selecione ");
            setbuf(stdin,NULL);
            scanf("%c", &entradaUsuario);
            entradaValida = ((entradaUsuario == '1' && (mesa->numSequencias || mesa->numTemp) ) || entradaUsuario == '2' || entradaUsuario == '3');
            if(!entradaValida)
                printf("Opcao invalida\n");
        }while(!entradaValida);

        if(entradaUsuario == '1' && (mesa->numSequencias || mesa->numTemp)){
            printf("Selecione a sequencia: ");
            setbuf(stdin,NULL);
            scanf("%d", &seqUsu);
        }else if(entradaUsuario == '2'){
            printf("Selecione os indices das pecas: ");
            setbuf(stdin,NULL);
            fscanf(stdin, "%s", seqJogada);
            seqTemp = sequencia_mao(jog->maoJogador, seqJogada);
            if(validar_inidice(seqJogada) && validar_jogada(seqTemp)){
                mesa->numTemp++;
                mesa->temp = (Sequencia*)realloc(mesa->temp, mesa->numTemp*sizeof(Sequencia));
                if(!mesa->temp)error_mem();
                mesa->temp[mesa->numTemp-1] = *seqTemp;
            }
            for(i = 0; i < strlen(seqJogada); i++){
                indice = getIndice(seqJogada[i]);
                getNipe(jog->maoJogador[indice].nipe);
                printf("%c ",jog->maoJogador[indice].valor);
            }
            printf("\n");
            getCorPadrao();
            system("PAUSE");
        }
    }while(entradaUsuario!='3');

    if(jog->numCartasMaoJogador == auxNumMaoJogador){   //Jogador nao desceu carta, entao deve comprar uma no monte
        jog->numCartasMaoJogador++;
        jog->maoJogador = (Carta*)realloc(jog->maoJogador, jog->numCartasMaoJogador*sizeof(Carta));
        if(!jog->maoJogador)error_mem();
        jog->maoJogador[jog->numCartasMaoJogador-1] = pegar_carta_baralho(b);
        ordenar_cartas(jog->maoJogador, jog->numCartasMaoJogador);
    }
}

void error_mem(){
    printf("Erro ao alocar memoria. O programa será encerrado!\n");
    exit(0);
}

Sequencia* sequencia_mao(Carta *mao, char *indSel){
    Sequencia *seq = NULL;
    int tam = strlen(indSel);
    if(tam){
        seq = (Sequencia*)malloc(sizeof(Sequencia));
        if(seq){
            seq->numCartas = tam;
            seq->carta = (Carta*)malloc(tam*sizeof(Carta));
            if(seq->carta){
                int i= 0;
                ordenar_cartas(mao, tam);
                for(i = 0; i < tam; i++){
                    seq->carta[i] = mao[getIndice(indSel[i])];
                }
            }
        }else
            error_mem();
    }

    return seq;
}

int validar_inidice(char *ind){
    for(int i = 0; i< strlen(ind); i++)
        if(!(ind[i]>='a' && ind[i]<='z') && !(ind[i]>='A' && ind[i]<='Z'))
            return 0;
    return 1;
}

int validar_jogada(Sequencia *seq){
    int i = 0;
    if(!seq || seq->numCartas < 3)
        return 0;
    int ord;
    int aux;
    ordenar_cartas(seq->carta, seq->numCartas);
    if(seq->carta[i].nipe == seq->carta[i+1].nipe){
        ord = seq->carta[i].nipe;
        aux = hexa_to_dec(seq->carta[i].valor);
        aux--;
        printf("VALID SEQ DE MESMO NIPE (1,2,3,4,5,6)\n");
        for(i = 0; i < seq->numCartas; i++){
            aux++;
            if(seq->carta[i].nipe != ord || ((i+1) < seq->numCartas && aux != (hexa_to_dec(seq->carta[i+1].valor)-1))){
                return 0;
            }
        }
    }else{
        ord = hexa_to_dec(seq->carta[i].valor);
        printf("VALID SEQ DE MESMO VALOR (7,7,7,7)\n");
        for(i = 0; i < seq->numCartas; i++){
            if(seq->carta[i].valor != 'X'){
                if(hexa_to_dec(seq->carta[i].valor) != ord){
                    return 0;
                }
                for(int j = 0; j < seq->numCartas; j++){
                    if(i != j && (seq->carta[i].nipe == seq->carta[j].nipe)){
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

void iniciar_jogo(Mesa *mesa, Jogador_PTR jog, int qtdJog, Baralho *b){
    int i = 0, aux;
    do{
        tempo_entre_jogadas(&jog[i]);
        system("CLS");
        realizar_jogada(mesa, &jog[i], b);
        aux = i; i = (i + 1 == qtdJog) ? 0 : (i + 1);
    } while(!finalizar_jogo(&jog[aux]));
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
    iniciar_jogo(&mesa, jog, qtdJog, &baralho);
    return 0;
}
