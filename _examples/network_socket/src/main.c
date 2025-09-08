#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "80"
#define MAXDATASIZE 4096

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int get_connection_socket(const char hostname[static 1]) {
  int sockfd, rv;
  struct addrinfo hints, *servinfo, *p;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(hostname, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    inet_ntop(p->ai_family, get_in_addr(p->ai_addr), s, sizeof s);
    printf("client: attempting connection to %s\n", s);

    if (-1 == connect(sockfd, p->ai_addr, p->ai_addrlen)) {
      perror("client: connect");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return -1;
  }

  inet_ntop(p->ai_family, get_in_addr(p->ai_addr), s, sizeof s);
  printf("client: connected to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure
  return sockfd;
}

int send_http_get(int sockfd, const char hostname[static 1],
                  const char path[static 1]) {
  char request[1024];
  long bytes_sent, total_sent = 0;
  int len = snprintf(request, sizeof(request),
                     "GET %s HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "Connection: close\r\n"
                     "\r\n",
                     path, hostname);

  if (len >= (int)sizeof(request)) {
    fprintf(stderr, "Request too large!\n");
    return -1;
  }

  for (; total_sent < len; total_sent += bytes_sent) {
    bytes_sent = send(sockfd, request + total_sent, len - total_sent, 0);
    if (bytes_sent < 0) {
      perror("send");
      return -1;
    }
  }
  return 0;
}

int send_http_post(int sockfd, const char hostname[static 1],
                   const char path[static 1]) {
  char request[1024];
  long bytes_sent, total_sent = 0;
  const char *body = "{\"name\": \"MYNAME\"}";

  int body_len = (int)strlen(body);
  int len = snprintf(request, sizeof(request),
                     "POST %s HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "Connection: close\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: %d\r\n"
                     "\r\n"
                     "%s\r\n",
                     path, hostname, body_len, body);

  if (len >= (int)sizeof(request)) {
    fprintf(stderr, "Request too large!\n");
    return -1;
  }

  for (; total_sent < len; total_sent += bytes_sent) {
    bytes_sent = send(sockfd, request + total_sent, len - total_sent, 0);
    if (bytes_sent < 0) {
      perror("send");
      return -1;
    }
  }
  return 0;
}

int recv_response(int sockfd) {
  long numbytes;
  char buf[MAXDATASIZE];

  while (1) {
    numbytes = recv(sockfd, buf, sizeof buf - 1, 0);

    if (numbytes > 0) {
      buf[numbytes] = '\0';
      printf("%s", buf);
    } else if (numbytes == 0) {
      printf("\n--- Connection closed by server ---\n");
      break;
    } else {
      perror("recv");
      return -1;
    }
  }
  return 0;
}

int main(void) {
  const char hostname[] = "example.net";
  const char path[] = "/";
  int sockfd = get_connection_socket(hostname);
  int status = 0;

  if (sockfd < 0) {
    fprintf(stderr, "Failed to establish connection.\n");
    return 1;
  }

  if (send_http_get(sockfd, hostname, path) != 0) {
    status = 1;
    goto cleanup;
  }

  if (recv_response(sockfd) != 0) {
    status = 1;
    goto cleanup;
  }

cleanup:
  close(sockfd);
  return status;
}
