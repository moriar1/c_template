#define PORT "80"
#define MAXDATASIZE 4096

struct sockaddr;

void *get_in_addr(struct sockaddr *);
int get_connection_socket(const char[static 1]);
int send_http_get(int sockfd, const char[static 1], const char[static 1]);
int send_http_post(int sockfd, const char[static 1], const char[static 1]);
int recv_response(int sockfd);
