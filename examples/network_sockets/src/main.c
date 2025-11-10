#include "mynetwork.h"
#include <stdio.h>
#include <unistd.h>

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
