#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "ConioC/conio.c"

#define TAM_MAX_NOME 20
#define TAM_MAX_CARTAS_BARALHO 106
#define TIME_ENTRE_JOG 0
#define QTD_CARTAS_INICIAIS 14
#define TIPO_SEQ  0
#define TIPO_TEMP 1

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* Declaracao Structs */
typedef struct{
    int naipe;
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
    int pontos;
    bool primeiraJogada;
} Jogador;

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* typedef's para os ponteiros */
typedef Jogador* Jogador_PTR;
typedef Jogador_PTR* Jogador_PTR_PTR;
typedef Baralho* Baralho_PTR;
typedef Sequencia* Sequencia_PTR;
typedef Carta* Carta_PTR;
typedef Mesa* Mesa_PTR;
typedef Mesa_PTR* Mesa_PTR_PTR;

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* Prototipo de funcoes */
int main();
Baralho_PTR get_baralho_file();
void get_cor_naipe(int);
void get_cor_padrao();
int get_code_naipe(char);
char get_simb_naipe(int);
char dec_to_hexa(int);
int hexa_to_dec(char);
void embaralhar(Baralho_PTR);
void distribuir_cartas(Jogador_PTR_PTR, int, Baralho_PTR);
void exibir_baralho(Baralho_PTR);
void inicializar_baralho(Baralho_PTR);
void remover_carta_baralho(Baralho_PTR, int);
Carta pegar_carta_baralho(Baralho_PTR);
void ordenar_cartas(Carta_PTR, int);
void ordenar_jogadores(Jogador_PTR*, int);
void ordenar_indice(char*, int);
void corrigir_enter(char*);
void inicializar_jogadores(Jogador_PTR_PTR, int, Baralho_PTR);
bool finalizar_jogo(Jogador_PTR, Baralho_PTR);
void tempo_entre_jogadas(Jogador_PTR);
void exibir_mao_jogador(Jogador_PTR);
void exibir_seq_mesa(Mesa_PTR);
int get_indice_por_letra(char);
void remover_cartas_mao_jogador(Jogador_PTR, char*);
bool continuar_jogada();
void realizar_jogada(Mesa_PTR_PTR, Jogador_PTR_PTR, Baralho_PTR);
void remover_cartas_sequencia(Sequencia_PTR, char*);
void remover_sequencia_mesa(Mesa_PTR, char*, int);
void error_mem();
Sequencia_PTR get_sequencia_mao(Carta_PTR, char*, int*);
bool validar_inidice(char*, int);
bool validar_jogada(Sequencia_PTR);
void iniciar_jogo(Mesa_PTR, Jogador_PTR_PTR, int, Baralho_PTR);
void contagem_pontos(Jogador_PTR);

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int main() {
	system("MODE con cols=1024 lines=768");
	char entradaUsu;
	int qtdJog = 0;
	srand((unsigned) time(NULL));
	Baralho_PTR baralho = get_baralho_file();
	if(!baralho){
        baralho = (Baralho_PTR) malloc(sizeof(Baralho));
        inicializar_baralho(baralho);
        embaralhar(baralho);
	}
	Mesa_PTR mesa = (Mesa_PTR) malloc(sizeof(Mesa));
	mesa->numSequencias = 0;
	mesa->sequencia = NULL;
	do {
		printf("Quantidade de jogadores [1-5]: ");
		setbuf(stdin, NULL);
		scanf("%c", &entradaUsu);
		if (entradaUsu < '1' || entradaUsu > '5') {
			system("CLS");
			printf("Valor invalido\n");
		} else
			qtdJog = entradaUsu - '0';
	} while (entradaUsu < '1' || entradaUsu > '5');
	Jogador_PTR_PTR jogList = (Jogador_PTR*)malloc(qtdJog * sizeof(Jogador_PTR));
	for (int i = 0; i < qtdJog; i++)
		jogList[i] = (Jogador_PTR) malloc(sizeof(Jogador));
	inicializar_jogadores(jogList, qtdJog, baralho);
	distribuir_cartas(jogList, qtdJog, baralho);
	iniciar_jogo(mesa, jogList, qtdJog, baralho);

	ordenar_jogadores(jogList, qtdJog);
	system("CLS");

	/* exibindo pontuacao dos jogadores */
	printf("|    | JOGADOR              | PONTOS   |\n");
	for (int i = 0; i < qtdJog; i++)
		printf("| 0%d | %-20s | %04d pts |\n", (i + 1), jogList[i]->nome, jogList[i]->pontos);

	/* jogo finalizado *//* liberando espaco na memoria */
	free(baralho);
	free(mesa);
	for (int i = 0; i < qtdJog; i++)
		free(jogList[i]);
	free(jogList);
	return 0;
}

Baralho_PTR get_baralho_file(){
    FILE *file = fopen("baralho.txt", "r");
    Baralho_PTR bar = NULL;
    char vec[3];
    int x;
    if(file){
        bar = (Baralho_PTR)malloc(sizeof(Baralho));
        bar->numCartas = 0;
        while(!feof(file)){
            x = fscanf(file, "%s", vec);
            if(x == 1){
                bar->numCartas++;
                bar->carta[bar->numCartas-1].valor = vec[0];
                bar->carta[bar->numCartas-1].naipe = get_code_naipe(vec[1]);
            }
        }
    }
    fclose(file);
    return bar;
}

void get_cor_padrao(){
    textcolor(LIGHTGRAY);
}

int get_code_naipe(char naipe){
    switch(naipe){
        case '!': return 0;
        case '@': return 1;
        case '#': return 2;
        case '$': return 3;
        case '*': return 4;
        default: return -1;
    }
}

char get_simb_naipe(int naipe){
    switch(naipe){
        case 0: return '!';
        case 1: return '@';
        case 2: return '#';
        case 3: return '$';
        case 4: return '*';
        default: return '\0';
    }
}

void get_cor_naipe(int cor){
    switch(cor){
        case 0: textcolor(LIGHTRED); break;
        case 1: textcolor(YELLOW); break;
        case 2: textcolor(LIGHTGREEN); break;
        case 3: textcolor(LIGHTCYAN); break;
        default: textcolor(WHITE);
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
    if(hexa == '*')
        return 20;
    return -1;
}

void embaralhar(Baralho_PTR baralho){
    int valRand;
    Carta aux;
    for(int i = 0; i < 1000; i++){
        valRand = rand() % baralho->numCartas;
        aux = baralho->carta[valRand];
        for(int j = valRand; j >= 0; j--){
            if((j-1) >= 0){
                baralho->carta[j] = baralho->carta[j-1];
            }
        }
        baralho->carta[0] = aux;
    }
}

void distribuir_cartas(Jogador_PTR_PTR jogList, int qtdJog, Baralho_PTR baralho){
    for(int i = 0; i < qtdJog; i++){
        jogList[i]->maoJogador = (Carta_PTR)malloc(QTD_CARTAS_INICIAIS * sizeof(Carta));
        jogList[i]->numCartasMaoJogador = QTD_CARTAS_INICIAIS;
    }
    for(int i = 0; i < QTD_CARTAS_INICIAIS; i++)
        for(int j = 0; j < qtdJog; j++)
            jogList[j]->maoJogador[i] = pegar_carta_baralho(baralho);
    for(int i = 0; i < qtdJog; i++)
        ordenar_cartas(jogList[i]->maoJogador, jogList[i]->numCartasMaoJogador);
}

void exibir_baralho(Baralho_PTR b){
    for(int i = 0;i < b->numCartas; i++){
        get_cor_padrao();
        printf("ind:%d - ",i);
        get_cor_naipe(b->carta[i].naipe);
        printf("%c%c\n",b->carta[i].valor,get_simb_naipe(b->carta[i].naipe));
    }
    get_cor_padrao();
    printf("\n");
}

void inicializar_baralho(Baralho_PTR b){
    b->numCartas = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 13; j++){
            b->carta[(13 * i)+ j].naipe  = i;
            b->carta[(13 * i)+ j].valor = dec_to_hexa(j + 1);
            b->numCartas++;

            b->carta[((13 * i)+ j)+ 52].naipe = i;
            b->carta[((13 * i)+ j)+ 52].valor = dec_to_hexa(j + 1);
            b->numCartas++;
        }
    }
    b->carta[104].naipe = 4; b->carta[104].valor = '*'; b->numCartas++;
    b->carta[105].naipe = 4; b->carta[105].valor = '*'; b->numCartas++;
}

void remover_carta_baralho(Baralho_PTR b, int indCartaRemover){
    for(int i = indCartaRemover; i < b->numCartas; i++){
        b->carta[i] = b->carta[i + 1];
    }
    b->numCartas--;
}

Carta pegar_carta_baralho(Baralho_PTR b){
    int valRand = 0;
    Carta c;
    if(b->numCartas){
        c = b->carta[valRand];
        remover_carta_baralho(b, valRand);
    }
    return c;
}

/* Ordena duas vezes para ficar com valores e naipes ordenados */
void ordenar_cartas(Carta_PTR vet, int tam){
    Carta aux;
    for(int j = tam - 1; j >= 1; j--)
        for(int i = 0; i < j; i++)
            if(vet[i].valor > vet[i+1].valor){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
    for(int j = tam - 1; j >= 1; j--)
        for(int i = 0; i < j; i++)
            if(vet[i].naipe > vet[i+1].naipe){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
}

void ordenar_jogadores(Jogador_PTR *jog, int qtd){
    Jogador_PTR aux;
    for(int j = qtd - 1; j >= 1; j--)
        for(int i = 0; i < j; i++)
            if(jog[i]->pontos < jog[i+1]->pontos){
                aux = jog[i];
                jog[i] = jog[i+1];
                jog[i+1] = aux;
            }
}

void ordenar_indice(char *vet, int tam){
    char aux;
    for(int j = tam - 1; j >= 1; j--)
        for(int i = 0; i < j; i++)
            if(vet[i] > vet[i+1]){
                aux = vet[i];
                vet[i] = vet[i+1];
                vet[i+1] = aux;
            }
}

void corrigir_enter(char *vet){
    for(int i = 0; i < TAM_MAX_NOME; i++)
        if(vet[i] == '\n'){
            vet[i] = '\0';
            break;
        }
}

void inicializar_jogadores(Jogador_PTR *jog, int qtdJog, Baralho_PTR b){
    for(int i = 0; i < qtdJog; i++){
        printf("Nome do jogador %d [MAX %d carac]: ", (i + 1), TAM_MAX_NOME);
        setbuf(stdin,NULL);
        fgets(jog[i]->nome, TAM_MAX_NOME+1, stdin);
        corrigir_enter(jog[i]->nome);
        jog[i]->primeiraJogada = true;
    }
}

bool finalizar_jogo(Jogador_PTR jog, Baralho_PTR bar){
    if(!jog->numCartasMaoJogador || !bar->numCartas)
        return true;
    return false;
}

/* X segundos para trocar de jog, baseado no define TIME_ENTRE_JOG */
/* Criei essa opcao para dar a possibilidade de um jogador nao ver a carta do outro */
void tempo_entre_jogadas(Jogador_PTR jog){
    for(int i = 0; i < TIME_ENTRE_JOG; i++){
        system("CLS");
        printf("Preparar para mostrar pecas de %s: %d seg\n",jog->nome,(TIME_ENTRE_JOG - i));
        Sleep(1000);
    }
    system("CLS");
}

void exibir_mao_jogador(Jogador_PTR jog){
    printf("MAO:\n");
    char var = 'a';
    for(int i = 0; i < jog->numCartasMaoJogador; i++){
        get_cor_naipe(jog->maoJogador[i].naipe);
        printf("%c%c ",jog->maoJogador[i].valor, get_simb_naipe(jog->maoJogador[i].naipe));
    }
    get_cor_padrao();
    printf("\n");
    for(int i = 0; i < jog->numCartasMaoJogador; i++){
        printf("%c  ", var);
        if(var >= '0' && var <= '9')
            var = (var == '9') ? 'a' : var+1;
        if(var >= 'A' && var <= 'Z')
            var = (var == 'Z') ? '0' : var+1;
        if(var >= 'a' && var <= 'z')
            var = (var == 'z') ? 'A' : var+1;
    }
    printf("\n\n");
}

void exibir_seq_mesa(Mesa_PTR mesa){
    printf("MESA:\n");
    char indice = 'a';
    if(!mesa->numSequencias)
        printf("(Vazio)\n");
    else{
        for(int i = 0; i < mesa->numSequencias; i++){
            printf("M[%c] -> ",indice);
            if(indice >= '0' && indice <= '9')
                indice = (indice == '9') ? 'a' : indice+1;
            if(indice >= 'A' && indice <= 'Z')
                indice = (indice == 'Z') ? '0' : indice+1;
            if(indice >= 'a' && indice <= 'z')
                indice = (indice == 'z') ? 'A' : indice+1;
            for(int j = 0; j < mesa->sequencia[i].numCartas; j++){
                get_cor_naipe(mesa->sequencia[i].carta[j].naipe);
                printf("%c%c ", mesa->sequencia[i].carta[j].valor, get_simb_naipe(mesa->sequencia[i].carta[j].naipe));
            }
            get_cor_padrao();
            printf("\n");
        }
    }
    if(mesa->numTemp){
        indice = 'a';
        printf("----------------------------------------\n");
        printf("TEMP:\n");
        for(int i = 0; i < mesa->numTemp; i++){
            printf("T[%c] -> ", indice);
            if(indice >= '0' && indice <= '9')
                indice = (indice == '9') ? 'a' : indice+1;
            if(indice >= 'A' && indice <= 'Z')
                indice = (indice == 'Z') ? '0' : indice+1;
            if(indice >= 'a' && indice <= 'z')
                indice = (indice == 'z') ? 'A' : indice+1;
            for(int j = 0; j < mesa->temp[i].numCartas; j++){
                get_cor_naipe(mesa->temp[i].carta[j].naipe);
                printf("%c%c ", mesa->temp[i].carta[j].valor, get_simb_naipe(mesa->temp[i].carta[j].naipe));
            }
            get_cor_padrao();
            printf("\n");
        }
    }
    printf("\n");
}

int get_indice_por_letra(char c){
    if(c >= 'a' && c <= 'z')
        return c - 'a';
    if(c >= 'A' && c <= 'Z')
        return c - 'A' + 26;
    if(c >= '0' && c <= '9')
        return c - '0' + 52;
    return -1;
}

void remover_cartas_mao_jogador(Jogador_PTR jog, char *indice){
    int tam = strlen(indice);
    ordenar_indice(indice, tam);
    for(int i = (tam-1); i >= 0; i--)
        for(int j = get_indice_por_letra(indice[i]); j < jog->numCartasMaoJogador; j++)
            if((j+1) < jog->numCartasMaoJogador)
                jog->maoJogador[j] = jog->maoJogador[j+1];
    jog->numCartasMaoJogador -= tam;
    jog->maoJogador = (Carta_PTR)realloc(jog->maoJogador, jog->numCartasMaoJogador * sizeof(Carta));
    if(!jog->maoJogador)error_mem();
}

bool continuar_jogada(){
    char respUsu;
    printf("Jogada Invalida!\n");
    printf("Opcoes:\n");
    printf("1 - Continuar com jogada\n");
    printf("2 - Desfazer jogada e passar a vez\n");
    do{
        printf("Selecione: ");
        setbuf(stdin, NULL);
        fscanf(stdin, "%c",&respUsu);
    }while(respUsu < '1' || respUsu > '2');
    if(respUsu == '1')
        return true;
    return false;
}

void realizar_jogada(Mesa_PTR_PTR mesa, Jogador_PTR_PTR jog, Baralho_PTR b){
    /* BKP das situacoes iniciais (mao e mesa) */
    Jogador_PTR jogBKP = (Jogador_PTR)malloc(sizeof(Jogador));
    Carta_PTR maoBKP = (Carta_PTR)malloc((*jog)->numCartasMaoJogador * sizeof(Carta));
    for(int i = 0; i < (*jog)->numCartasMaoJogador; i++)
        maoBKP[i] = (*jog)->maoJogador[i];
    *jogBKP = **jog;
    jogBKP->maoJogador = maoBKP;
    Mesa_PTR mesaBKP = (Mesa_PTR)malloc(sizeof(Mesa));
    Sequencia_PTR seqMesaBKP = (Sequencia_PTR)malloc((*mesa)->numSequencias * sizeof(Sequencia));
    for(int i = 0; i < (*mesa)->numSequencias; i++)
        seqMesaBKP[i] = (*mesa)->sequencia[i];
    *mesaBKP = **mesa;
    mesaBKP->sequencia = seqMesaBKP;

    char entradaUsuario;
    int indice;
    bool flagJogadaValida;
    bool entradaValida;
    int auxNumMaoJogador = (*jog)->numCartasMaoJogador;
    int soma = 0;
    char letra;
    char *auxIndice;
    char *selecionarIndice = NULL;
    Sequencia_PTR seqTemp, seqAux;
    (*mesa)->temp = NULL;
    (*mesa)->numTemp = 0;

    do{
        flagJogadaValida = false;
        entradaValida = 0;
        entradaUsuario = '\0';
        system("CLS");
        printf("Vez de %s\n", (*jog)->nome);
        printf("\n");
        printf("Baralho contem %d cartas\n", b->numCartas);
        printf("\n");
        exibir_seq_mesa(*mesa);
        exibir_mao_jogador(*jog);

        printf("|***************************|\n");
        printf("|Opcoes\t\t\t    |\n");
        printf("|***************************|\n");
        if(!(*jog)->primeiraJogada && (*mesa)->numSequencias)
            printf("|Mesa-> Temp\t\t| 1 |\n");
        if((*mesa)->numTemp){
            printf("|Editar Temp\t\t| 2 |\n");
            printf("|Juntar Temp\t\t| 3 |\n");
        }
        printf("|Mao -> Temp\t\t| 4 |\n");
        if((*mesa)->numTemp)
            printf("|Desfazer jogada\t| 5 |\n");
        printf("|Encerar jogada\t\t| 6 |\n");
        printf("|***************************|\n");

        do{
            printf("|Selecione ");
            setbuf(stdin,NULL);
            scanf("%c", &entradaUsuario);
            entradaValida = ((entradaUsuario == '1' && (*mesa)->numSequencias) ||
                             ((entradaUsuario == '2' || entradaUsuario == '3' || entradaUsuario == '5') && (*mesa)->numTemp) ||
                             entradaUsuario == '4' ||
                             entradaUsuario == '6');
            if(!entradaValida)
                printf("Opcao invalida\n");
        }while(!entradaValida);

        switch(entradaUsuario){
        case '1':
            if((*mesa)->numSequencias){
                selecionarIndice = (char*)realloc(selecionarIndice, (*mesa)->numSequencias * sizeof(char));
                printf("Selecione a sequencia da MESA: ");
                setbuf(stdin,NULL);
                fgets(selecionarIndice, (*mesa)->numSequencias+1 , stdin);
                corrigir_enter(selecionarIndice);
                if(validar_inidice(selecionarIndice,(*mesa)->numSequencias)){
                    seqTemp = &(*mesa)->sequencia[get_indice_por_letra(selecionarIndice[0])];
                    (*mesa)->numTemp++;
                    (*mesa)->temp = (Sequencia_PTR)realloc((*mesa)->temp, (*mesa)->numTemp * sizeof(Sequencia));
                    if(!(*mesa)->temp)error_mem();
                    (*mesa)->temp[(*mesa)->numTemp-1] = *seqTemp;
                    remover_sequencia_mesa(*mesa, selecionarIndice, TIPO_SEQ);
                }
            }
            break;
        case '2':
            if((*mesa)->numTemp){
                selecionarIndice = (char*)realloc(selecionarIndice, (*mesa)->numTemp * sizeof(char));
                printf("Selecione a TEMP: ");
                setbuf(stdin, NULL);
                fgets(selecionarIndice, (*mesa)->numTemp+1 , stdin);
                corrigir_enter(selecionarIndice);
                if(strlen(selecionarIndice) == 1 && validar_inidice(selecionarIndice, (*mesa)->numTemp)){
                    seqTemp = &(*mesa)->temp[get_indice_por_letra(selecionarIndice[0])];
                    for(int i = 0; i < seqTemp->numCartas; i++){
                        get_cor_naipe(seqTemp->carta[i].naipe);
                        printf("%c%c ",seqTemp->carta[i].valor, get_simb_naipe(seqTemp->carta[i].naipe));
                    }
                    get_cor_padrao();
                    printf("\n");
                    letra = 'a';
                    for(int i = 0; i < seqTemp->numCartas; i++){
                        printf("%c  ", letra);
                        if(letra >= '0' && letra <= '9')
                            letra = (letra == '9') ? 'a' : letra+1;
                        if(letra >= 'A' && letra <= 'Z')
                            letra = (letra == 'Z') ? '0' : letra+1;
                        if(letra >= 'a' && letra <= 'z')
                            letra = (letra == 'z') ? 'A' : letra+1;
                    }
                    printf("\n");
                }
                auxIndice = selecionarIndice;
                selecionarIndice = NULL;
                selecionarIndice = (char*)realloc(selecionarIndice, seqTemp->numCartas * sizeof(char));
                printf("Selecione os indices: ");
                setbuf(stdin, NULL);
                fgets(selecionarIndice, seqTemp->numCartas+1 , stdin);
                corrigir_enter(selecionarIndice);

                if(validar_inidice(selecionarIndice, seqTemp->numCartas)){
                    seqAux = (Sequencia_PTR)malloc(sizeof(Sequencia));
                    seqAux->numCartas = 0;
                    seqAux->carta = NULL;
                    for(int i = 0; i < strlen(selecionarIndice); i++){
                        seqAux->numCartas++;
                        seqAux->carta = (Carta_PTR)realloc(seqAux->carta, seqAux->numCartas * sizeof(Carta));
                        seqAux->carta[i] = seqTemp->carta[get_indice_por_letra(selecionarIndice[i])];
                    }
                    remover_cartas_sequencia(seqTemp, selecionarIndice);
                    if(!seqTemp->numCartas)
                        remover_sequencia_mesa(*mesa, auxIndice, TIPO_TEMP);
                    (*mesa)->numTemp++;
                    (*mesa)->temp = (Sequencia_PTR) realloc((*mesa)->temp, (*mesa)->numTemp * sizeof(Sequencia));
                    if(!(*mesa)->temp)error_mem();
                    (*mesa)->temp[(*mesa)->numTemp-1] = *seqAux;
                }
            }
            break;
        case '3':
            if((*mesa)->numTemp){
                selecionarIndice = (char*)realloc(selecionarIndice, (*mesa)->numTemp * sizeof(char));
                printf("Selecione o(s) indicies da(s) TEMP(s): ");
                setbuf(stdin, NULL);
                fgets(selecionarIndice, (*mesa)->numTemp+1 , stdin);
                corrigir_enter(selecionarIndice);
                if(validar_inidice(selecionarIndice, (*mesa)->numTemp)){
                    seqTemp->numCartas = 0;
                    seqTemp->carta = NULL;
                    if(strlen(selecionarIndice)){
                        for(int i = 0; i < strlen(selecionarIndice); i++){
                            indice = get_indice_por_letra(selecionarIndice[i]);
                            seqAux = &(*mesa)->temp[indice];
                            seqTemp->carta = (Carta_PTR)realloc(seqTemp->carta, (seqTemp->numCartas + seqAux->numCartas) * sizeof(Carta));
                            for(int j = 0; j < seqAux->numCartas; j++)
                                seqTemp->carta[seqTemp->numCartas + j] = seqAux->carta[j];
                            seqTemp->numCartas += seqAux->numCartas;
                        }
                        remover_sequencia_mesa(*mesa, selecionarIndice, TIPO_TEMP);
                        (*mesa)->numTemp++;
                        (*mesa)->temp = (Sequencia_PTR) realloc((*mesa)->temp, (*mesa)->numTemp * sizeof(Sequencia));
                        if(!(*mesa)->temp)error_mem();
                        (*mesa)->temp[(*mesa)->numTemp-1] = *seqTemp;
                    }
                }
            }
            break;
        case '4':
            selecionarIndice = (char*)realloc(selecionarIndice, (*jog)->numCartasMaoJogador * sizeof(char));
            printf("Selecione os indices das pecas: ");
            setbuf(stdin,NULL);
            fgets(selecionarIndice, (*jog)->numCartasMaoJogador+1 , stdin);
            corrigir_enter(selecionarIndice);
            if(validar_inidice(selecionarIndice, (*jog)->numCartasMaoJogador)){
                seqTemp = get_sequencia_mao((*jog)->maoJogador, selecionarIndice, &soma);
                (*mesa)->numTemp++;
                (*mesa)->temp = (Sequencia_PTR)realloc((*mesa)->temp, (*mesa)->numTemp * sizeof(Sequencia));
                if(!(*mesa)->temp)error_mem();
                (*mesa)->temp[(*mesa)->numTemp-1] = *seqTemp;
                remover_cartas_mao_jogador(*jog, selecionarIndice);
            }
            break;
        case '5':
            printf("Todas as jogadas da rodada (TEMP) serao desfeitas. Deseja continuar?\n");
            break;
        case '6':
            flagJogadaValida = true;
            for(int i = 0; i < (*mesa)->numTemp; i++){
                if(!validar_jogada(&(*mesa)->temp[i])){
                    flagJogadaValida = false;
                    break;
                }
            }
            if(flagJogadaValida &&(!(*jog)->primeiraJogada || ((*jog)->primeiraJogada && soma >= 30))){
                if(!(*jog)->primeiraJogada || soma >= 30){
                    /* realizar jogada */
                    if((*jog)->primeiraJogada)
                        (*jog)->primeiraJogada = false;
                    if((*mesa)->numTemp){
                        (*mesa)->numSequencias += (*mesa)->numTemp;
                        (*mesa)->sequencia = (Sequencia_PTR)realloc((*mesa)->sequencia, (*mesa)->numSequencias * sizeof(Sequencia));
                        if(!(*mesa)->sequencia) error_mem();
                        for(int i = 0; i < (*mesa)->numTemp; i++)
                            (*mesa)->sequencia[((*mesa)->numSequencias - (*mesa)->numTemp) + i] = (*mesa)->temp[i];
                        (*mesa)->numTemp = 0;
                    }
                }
                free(jogBKP);
            }else{
                if((*mesa)->numTemp){
                    flagJogadaValida = false;
                    if(!continuar_jogada()){
                        flagJogadaValida = true;
                        free(*jog);
                        *jog = jogBKP;
                    }
                }
            }
            break;
        }
    }while(!flagJogadaValida);

    if((*jog)->numCartasMaoJogador == auxNumMaoJogador && b->numCartas){   //Jogador nao desceu carta, entao deve comprar uma no monte
        (*jog)->numCartasMaoJogador++;
        (*jog)->maoJogador = (Carta_PTR)realloc((*jog)->maoJogador, (*jog)->numCartasMaoJogador * sizeof(Carta));
        if(!(*jog)->maoJogador)error_mem();
        (*jog)->maoJogador[(*jog)->numCartasMaoJogador-1] = pegar_carta_baralho(b);
        ordenar_cartas((*jog)->maoJogador, (*jog)->numCartasMaoJogador);
    }
}

void remover_cartas_sequencia(Sequencia_PTR seq, char *ind){
    int tam = strlen(ind);
    ordenar_indice(ind, tam);
    for(int i = (tam-1); i >= 0; i--)
        for(int j = get_indice_por_letra(ind[i]); j < seq->numCartas; j++)
            if((j+1) < seq->numCartas)
                seq->carta[j] = seq->carta[j+1];
    seq->numCartas -= tam;
    seq->carta = (Carta_PTR)realloc(seq->carta, seq->numCartas * sizeof(Carta));
    if(seq->numCartas && !seq->carta)
        error_mem();
}

void remover_sequencia_mesa(Mesa_PTR mesa, char* indice, int tipo){
    int tam = strlen(indice);
    ordenar_indice(indice, tam);
    if(tipo == TIPO_SEQ){
        for(int i = (tam-1); i >= 0; i--)
            for(int j = get_indice_por_letra(indice[i]); j < mesa->numSequencias; j++)
                if((j+1) < mesa->numSequencias)
                    mesa->sequencia[j] = mesa->sequencia[j+1];
        mesa->numSequencias -= tam;
        mesa->sequencia = (Sequencia_PTR)realloc(mesa->sequencia, mesa->numSequencias * sizeof(Sequencia));
        if(mesa->numSequencias && !mesa->sequencia)
            error_mem();
	}else if(tipo == TIPO_TEMP){
	    for(int i = (tam-1); i >= 0; i--)
            for(int j = get_indice_por_letra(indice[i]); j < mesa->numTemp; j++)
                if((j+1) < mesa->numTemp)
                    mesa->temp[j] = mesa->temp[j+1];
        mesa->numTemp -= tam;
        mesa->temp = (Sequencia_PTR)realloc(mesa->temp, mesa->numTemp * sizeof(Sequencia));
        if(mesa->numTemp && !mesa->temp)
            error_mem();
	}
}

void error_mem(){
    printf("Erro ao alocar memoria. O programa ser� encerrado!\n");
    exit(0);
}

Sequencia_PTR get_sequencia_mao(Carta_PTR mao, char *indSel, int *soma){
    Sequencia_PTR seq = NULL;
    int tam = strlen(indSel);
    if(tam){
        seq = (Sequencia_PTR)malloc(sizeof(Sequencia));
        if(seq){
            seq->numCartas = tam;
            seq->carta = (Carta_PTR)malloc(tam*sizeof(Carta));
            if(seq->carta){
                ordenar_cartas(mao, tam);
                for(int i = 0; i < tam; i++){
                    seq->carta[i] = mao[get_indice_por_letra(indSel[i])];
                    *soma += hexa_to_dec(seq->carta[i].valor);
                }
            }
        }else
            error_mem();
    }

    return seq;
}

bool validar_inidice(char *ind, int max){
    int aux;
    int tam = strlen(ind);
    if(!tam)
        return false;
    for(int i = 0; i < tam; i++){
        aux = get_indice_por_letra(ind[i]);
        if(aux == -1 || aux >= max)
            return false;
        for(int j = i+1; j < tam; j++)
            if(ind[i] == ind[j])
                return false;
    }
    return true;
}

/* Valida sequencia recebida */
bool validar_jogada(Sequencia_PTR seq){
    if(!seq || seq->numCartas < 3)
        return false;
    int i = 0;
    int ord, aux;
    bool flagCoringa = false;
    Carta cartaAux;
    ordenar_cartas(seq->carta, seq->numCartas);
    if(seq->carta[i].naipe == seq->carta[i+1].naipe){ /* ordenar por naipe */
        ord = seq->carta[i].naipe;
        aux = hexa_to_dec(seq->carta[i].valor);
        aux--;
        for(i = 0; i < seq->numCartas; i++){
            if(seq->carta[i].naipe != ord && seq->carta[i].naipe != 4)
                return false;
            aux++;
            if((i+1) < seq->numCartas)
                if(aux != (hexa_to_dec(seq->carta[i+1].valor)-1)){
                    flagCoringa = false;
                    for(int j = i+1; j < seq->numCartas; j++)   /* verificar se tem coringa no restante do vetor */
                        if(seq->carta[j].naipe == 4){
                            cartaAux = seq->carta[seq->numCartas-1];
                            for(int k = (seq->numCartas-1); k > i ; k--)    /* mover colocar coringa la onde falta a carta (apos o i) e mover td mundo 1 pro lado */
                                seq->carta[k] = seq->carta[k-1];
                            seq->carta[i+1] = cartaAux;
                            flagCoringa = true;
                            break;
                        }
                    if(!flagCoringa)
                        return false;
                }
        }
    }else{ /* ordenar por valor */
        ord = hexa_to_dec(seq->carta[i].valor);
        for(i = 0; i < seq->numCartas; i++)
            if(seq->carta[i].naipe != 4){
                if(hexa_to_dec(seq->carta[i].valor) != ord)
                    return false;
                for(int j = 0; j < seq->numCartas; j++)
                    if(i != j && (seq->carta[i].naipe == seq->carta[j].naipe))
                        return false;
            }
    }
    return true;
}

void iniciar_jogo(Mesa_PTR mesa, Jogador_PTR *jogList, int qtdJog, Baralho_PTR b){
    int i = 0, aux;
    do{
        tempo_entre_jogadas(jogList[i]);
        system("CLS");
        realizar_jogada(&mesa, &jogList[i], b);
        aux = i; i = (i + 1 == qtdJog) ? 0 : (i + 1); /* alterna entre os jogadores */
    } while(!finalizar_jogo(jogList[aux], b));
    for(int j = 0; j < qtdJog; j++)
        contagem_pontos(jogList[j]);
}

void contagem_pontos(Jogador_PTR jog){
    int soma = 0;
    for(int i = 0; i < jog->numCartasMaoJogador; i++)
        soma += hexa_to_dec(jog->maoJogador[i].valor);
    jog->pontos = soma * (-1);
}
