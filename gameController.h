#ifndef __GAME_CONTROLLER__
#define __GAME_CONTROLLER__

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT_DEFAULT 4256 
#define MAXLINE 1024 
#define NUM_PLAYERS 4
#define NUM_DICES 5

#define BET 0
#define PLAY 1
#define GETSCORE 2
#define SCORE 3
#define TOKEN 4
#define ENDGAME 5

#define INIT_MARK 126

typedef struct message {
    int init;                       // marca início da mensagem
    int status;                     // se é aposta ou jogo
    int playerID; 		            // ID do jogador de aposta maior ou do destinatario
    int playerCards;                // saldo de fichas do jogador da rodada
    int price;                      // valor da aposta
    int combination;                // combinação escolhida para a rodada
    int parity;                     // paridade para detecção de erros
} message;

int choose_combination(message *msg, int cards);

int bet(message *msg, int cards);

void update_choosen_player(message *msg, int idPlayer);

int verify_n_cards (int *dices, int n, int *face);

int verify_two_pairs (int *dices);

int verify_full_house (int *dices);

int verify_sequence (int *dices, int fst_seq);

int cards_won(message *msg, int *dices);

void play_dices(message *msg, int *cards);

void print_score (message *msg);

int update_origin(int *current_origin, int idPlayer);

int parity(int n);

int message_parity(message *msg);

int parity_error(message *msg);

int init_mark_error(message *msg);

//void choose_player(message *msg, int sockfd);

/* char *inicia_aposta(char id);

char *faz_aposta(char id, char *aposta);

char *joga_dados(char *aposta, int fichas);

char *fichas_atuais(int fichas, int id);

char *mostra_fichas(char *pontuacao); */

#endif
