#include "gameController.h"
#include <time.h>

int main(int argc, char *argv[]) { 

    int sockfd;
    int PORT;
    int idPlayer = atoi(argv[1]);
    int running = 1;
    struct sockaddr_in servaddr, nextaddr, prevaddr;

    srand(time(NULL));

    // Define a porta de acordo com o id do jogador
    if (idPlayer == 1)
        PORT = PORT_DEFAULT;
    else
        PORT = PORT_DEFAULT + idPlayer - 1;

    printf("Começando o jogo Yahtzee..\n");
        
    // Cria o socket
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&nextaddr, 0, sizeof(nextaddr)); 
    memset(&prevaddr, 0, sizeof(prevaddr)); 
        
    // Inicializa o servidor
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
        
    nextaddr.sin_family    = AF_INET; // IPv4 
    nextaddr.sin_addr.s_addr = INADDR_ANY;

    prevaddr.sin_family    = AF_INET; // IPv4 
    prevaddr.sin_addr.s_addr = INADDR_ANY;

    // Armazena para qual máquina a atual deve mandar mensagem como próxima
    if (idPlayer == NUM_PLAYERS)
        nextaddr.sin_port = htons(PORT-NUM_PLAYERS+1);
    else
        nextaddr.sin_port = htons(PORT+1); 
        
    // Vincula o socket ao endereço do servidor
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

    message *msg;
    msg = malloc(sizeof(message));
    
    int len, token, origin = 1;
    int cards = 10;
    
    // Inicializa a mensagem
    msg->init = INIT_MARK;
    msg->status = BET;
    
    // Atribui o token a origem
    if (idPlayer == 1) {
       token = 1;
       msg->playerID = idPlayer;
    }
    else
	   token = 0; 
    
    len = sizeof(nextaddr);

    while (running) {

        if(token){

            if (msg->status == BET) {

                // Inicializa rodada e escolhe a combinação
                msg->price = 0;
                msg->playerID = idPlayer;
                msg->playerCards = cards;
                msg->price = choose_combination(msg, cards);
                
            }

            msg->parity = message_parity(msg);
            sendto(sockfd, (message *) msg, sizeof(message), MSG_CONFIRM, (const struct sockaddr *) &nextaddr, len);
        }
        
        // Aguarda uma nova mensagem vinda da máquina anterior
        recvfrom(sockfd, (message *) msg, sizeof(message), MSG_WAITALL, ( struct sockaddr *) &prevaddr, &len);

        // Verifica se houve algum erro na mensagem recebida
        if (parity_error(msg) || init_mark_error(msg)) {
            perror("A mensagem não está correta, o jogo será encerrado.");
            msg->status = ENDGAME;
        }

        switch (msg->status){
            case BET:
                if (!token) {
                    // Verifica quem da rede quer jogar na rodada
                    if(bet(msg, cards))
                        update_choosen_player(msg, idPlayer);
                }
                else {
                    // Depois de percorrer toda a rede, atualiza o status
                    msg->status = PLAY;
                }
            break;
            case PLAY:
                if(msg->playerID == idPlayer) {
                    // Jogador atual joga os dados e computa seu resultado
                    play_dices(msg, &cards);
                    msg->status = GETSCORE;
                }
                
            break;
            case GETSCORE:
                if (token) {
                    // Imprime o score na própria tela e atualiza o status
                    print_score(msg);
                    msg->status = SCORE;
                }
            break;
            case SCORE:
                if(token){
                    if(msg->playerCards > 0){
                        msg->status = TOKEN;
                        token = 0;
                    }
                    else {
                        msg->status = ENDGAME;
                    }
                }
		        else {
                    // Passa por toda a rede imprimindo o resultado da rodada
                    print_score(msg);
                }
            break;
            case TOKEN:
                if (token) {
                    msg->status = BET;
                }
                else {
                    // Atualiza o token para sua passagem
                    token = update_origin(&origin, idPlayer);
                }
            break;
            case ENDGAME:
                printf("Jogador %d perdeu, fim de jogo...\n", msg->playerID);
                running = 0;
            break;

        }
        
        if (!token) {
            msg->parity = message_parity(msg);
	        sendto(sockfd, (message *) msg, sizeof(message), MSG_CONFIRM, (const struct sockaddr *) &nextaddr, len);
        }

    }

    free(msg);

    return 0; 
}
