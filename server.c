#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main() {
    // Disable buffering so logs show up immediately
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // 1) Create TCP socket (IPv4)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("socket failed: %s\n", strerror(errno));
        return 1;
    }

    // 2) Allow quick restart (avoid "Address already in use")
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("setsockopt failed: %s\n", strerror(errno));
        return 1;
    }

    // 3) Bind to 0.0.0.0:4221
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4221);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("bind failed: %s\n", strerror(errno));
        return 1;
    }

    // 4) Listen (start accepting connections)
    if (listen(server_fd, 5) < 0) {
        printf("listen failed: %s\n", strerror(errno));
        return 1;
    }

    printf("Listening on port 4221...\n");

    // 5) Accept ONE connection (enough for this stage)
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        printf("accept failed: %s\n", strerror(errno));
        return 1;
    }

    printf("Client connected.\n");

    // Close sockets
    close(client_fd);
    close(server_fd);
    return 0;
}