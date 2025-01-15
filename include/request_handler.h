#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

int server_fd = -1; // Socket global para fechar no tratador de sinal

void handle_sigint();

void process_request(int client_fd);

#endif // REQUEST_HANDLER_H