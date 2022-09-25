#include "gameController.h"

// Retorna o quanto o jogador apostou
int choose_combination(message *msg, int cards) {

    // Imprime na tela as opções de combinações e quanto cada uma vale
    printf("Escolha uma combinação e o valor inicial da aposta:\n");
    printf("[1]: Par - 2 fichas\n[2]: Trio - 3 fichas\n");
    printf("[3]: Dois Pares - 4 fichas\n[4]: Full House - 5 fichas\n");
    printf("[5]: Sequência baixa - 7 fichas\n[6]: Sequência alta - 7 fichas\n");
    printf("[7]: Quadra - 10 fichas\n[8]: Quinteto - 15 fichas\n");

    int price;
    scanf("%d %d", &(msg->combination), &price);

    // Verifica se o jogador pode apostar o valor de aposta inicial
    while (cards < price || price < 1 || msg->combination < 1 || msg->combination > 8) {
        printf("Aposte uma quantidade de fichas menor ou igual ao que você tem.\n");
        printf("Escolha uma combinação e o valor da aposta:\n");
        scanf("%d %d", &(msg->combination), &price);
    }

    // Retorna o valor da aposta inicial
    return price;
}

// Verifica se o jogador quer apostar na rodada
// Retorna um valor diferente de 0 se sim e 0 se não
int bet(message *msg, int cards) {
    
    // Verifica se o jogador tem saldo suficiente para apostar
    if (msg->price > cards) {   
        printf("Você não pode apostar nessa rodada\n");
        return 0;
    }

    char *betName;
    switch (msg->combination){
    	case 1: betName = "Par";
	break;
	case 2: betName = "Trio";
	break;
	case 3: betName = "2 Pares";
	break;
	case 4: betName = "Full House";
	break;
	case 5: betName = "Sequencia Baixa";
	break;
	case 6: betName = "Sequencia Alta";
	break;
	case 7: betName = "Quadra";
	break;
	case 8: betName = "Quinteto";
	break;
    }

    printf("Jogador %d apostou %d fichas em um(a) %s, você deseja cobrir a aposta por %d fichas?\n", msg->playerID, msg->price, betName, msg->price+1);
    printf("[0] Não\n[1] Sim\n");

    int play;
    scanf("%d", &play);

    // Retorna a decisão do jogador
    return play;
}

// Atualiza qual o jogador apostou mais e o valor da aposta atual
void update_choosen_player(message *msg, int idPlayer) {

    msg->playerID = idPlayer;
    msg->price++;

}

// Verifica se tem n dados iguais
// Retorna n se sim e 0 se não
int verify_n_cards (int *dices, int n, int *face) {

    int count;

    // Percorre o vetor de dados procurando n números iguais
    for (int i = 0; i < NUM_DICES; i++) {
        count = 1;
        int current_dice = dices[i];
        for (int j = i + 1; j < NUM_DICES; j++) {
            if ((dices[j] == current_dice) && (dices[j] != 0)) {
                count++;
                *face = current_dice;
            }
        }
        if (count >= n)
            return n;
    }

    return 0; 
}

// Verifica se há dois pares de faces
// Retorna 4 se sim e 0 se não
int verify_two_pairs (int *dices) {

    int count = 0;

    for (int i = 0; i < NUM_DICES; i++) {
        for (int j = i + 1; j < NUM_DICES; j++) {
            if ((dices[j] == dices[i]) && (dices[j] != 0)) {
                count++;
                dices[i] = 0;
                dices[j] = 0;
            }
        }
        if (count > 1)
            return 4;
    }

    return 0; 
}

// Verifica se há 1 par e 1 trio
// Retorna 5 se sim e 0 se não
int verify_full_house (int *dices) {

    int face = 0;
    if (verify_n_cards(dices, 3, &face)){
    	for (int i = 0, j = 0; i < NUM_DICES && j < 3; i++){
		if(dices[i] == face){
			dices[i] = 0;
			j++;
		}
	}
	if(verify_n_cards(dices, 2, &face))
		return 5;
    }
    
    return 0;
}

// Verifica se há sequência baixa ou alta nos dados escolhidos
// Retorna 7 se sim e 0 se não
int verify_sequence (int *dices, int fst_seq) {
    
    for(int i = 0; i < NUM_DICES; i++){
	    if((fst_seq == 1 && dices[i] == 6) || (fst_seq == 2 && dices[i] == 1))
		    return 0;
    }
    int face;
    if(verify_n_cards(dices, 2, &face))
	    return 0;
    return 7;
}

// Retorna a quantidade de fichas ganhadas pelo jogador na rodada
int cards_won(message *msg, int *dices) {
    
    int face = 0;

    switch (msg->combination) {
    case 1:
        // Par
        return verify_n_cards(dices, 2, &face);
    break;
    case 2:
        // Trio
        return verify_n_cards(dices, 3, &face);
    break;
    case 3:
        // Dois pares
        return verify_two_pairs(dices);
    break;
    case 4:
        // Full house
        return verify_full_house(dices);
    break;
    case 5:
        // Sequência baixa
        return verify_sequence(dices, 1);
    break;
    case 6:
        // Sequência alta
        return verify_sequence(dices, 2);
    break;
    case 7:
        // Quadra
        if (verify_n_cards(dices, 4, &face))
            return 10;
    break;
    case 8:
        // Quinteto
        return verify_n_cards(dices, 5, &face);
    break;
    default:
        return 0;
        break;
    }

    return 0;
}

// Joga os dados para o jogador atual
// e verifica a quantidade de fichas ganhas
void play_dices(message *msg, int *cards) {
	int *dices = malloc(sizeof(int *) * NUM_DICES);
	int *hold = calloc(NUM_DICES, sizeof(int *));

    // Atualiza a quantidade de fichas do jogador
    *cards -= msg->price;

    printf("\n+------- SUA VEZ DE JOGAR -------+\n");
    printf("Começando com %d fichas.\n", *cards);
    
     // Joga os dados
	for(int i = 0; i < 2; i++){

        printf("\nResponda 1 para manter e 0 para rerrolar embaixo de cada dado:\n");
		for(int j = 0; j < 5; j++){
            // Os dados não segurados são jogados novamente
			if(!hold[j])
				dices[j] = rand() % 6 + 1;
			printf("%d ", dices[j]);
		}

        printf("\n");
        scanf("%d %d %d %d %d", &hold[0], &hold[1], &hold[2], &hold[3], &hold[4]);
	}
    
    for (int i = 0; i < NUM_DICES; i++) {
        if(!hold[i])
            dices[i] = rand() % 6 + 1;
        printf("%d ", dices[i]);
    }
    printf("\n\n");
    
    printf("\n\n");
	
    // Verifica quantas fichas ganhou na rodada
    *cards += cards_won(msg, dices);
    msg->playerCards = *cards;

    // Se não há mais fichas, atualiza o estado do jogo
    // para avisar a origem
    if (msg->playerCards <= 0)
        msg->status = ENDGAME;
    
    free(dices);
    free(hold);
}

// Imprime o score da rodada na tela
void print_score (message *msg) {
    
    printf("+---------------- RESULTADO DA RODADA -----------------+\n");
    printf("+                                                      +\n");
    printf("+    Jogador %d está com saldo de %d fichas na rodada    +\n", msg->playerID, msg->playerCards);
    printf("+                                                      +\n");
    printf("+------------------------------------------------------+\n\n");
}

// Atualiza a origem de acordo com o id do jogador
// Retorna 1 se a origem é o jogador da máquina e 0 se não
int update_origin(int *current_origin, int idPlayer) {

    if (*current_origin == NUM_PLAYERS)
        *current_origin = 1;
    else
        *current_origin += 1;
    
    return (*current_origin == idPlayer);
}

// Verifica a quantidade de bits iguais a 1
// e retorna quantos encontrou
int parity(int n) {
    
    int count = 0, copy_n = n;
 
    // Percorre todos os bits
    while (copy_n > 0) {
 
        // Se o bit atual é igual a 1, incrementa
        if (copy_n & 1) {
            count++;
        }
 
        copy_n = copy_n >> 1;
    }

    return (count);

}

// Verifica se o número de bits na mensagem é igual a 1 é par
// Retorna 1 se não é e 0 se sim
int message_parity(message *msg) {

    int count = 0;

    count += parity(msg->status);
    count += parity(msg->playerID);
    count += parity(msg->playerCards);
    count += parity(msg->price);
    count += parity(msg->combination);

    return (count % 2 == 1);
}

// Verifica se há erro de paridade
// Retorna 1 se sim e 0 se não
int parity_error(message *msg) {

    int recv_parity;

    recv_parity = message_parity(msg);

    return (recv_parity != msg->parity);
    
}

// Verifica se o enquandramento não está correto
// Retorna 1 se houve erro e 0 se não
int init_mark_error(message *msg) {

    return (msg->init != INIT_MARK);
}

