#include <arpa/inet.h>
#include <byteswap.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#define DISCOVER_PORT                  25
#define QUERY_SIZE                    128
#define RESPONSE_SIZE                 408

#pragma pack(push, 1)
struct response
{
    uint32_t unknown;
    uint8_t  version[6];
    char     model[32];
    char     name[32];
    char     serial[34];
    uint32_t unknown2[3];
    char     ssid[64];
    char     password[64];
    uint32_t unknown3[3];
    uint8_t  zipcode[12];
    char     p2pm[16];
    char     p2ps[16];
    char     paw[16];
    uint32_t unknown4[12];
    char     mac[18];
    char     ip[18];
    uint32_t port;

    friend bool operator< (const response &l, const response &r)
    {
        return l.port < r.port;
    }
};
#pragma pack(pop)

bool gKeepGoing = true;
pthread_t gRecvP1;
std::mutex gResponseMutex;
std::vector<response> gFound;
char gDeviceSerial[34];

struct broadcastParams
{
    int socket;
    uint16_t port;
};

struct sockaddr_in buildServerType(in_addr_t address, in_port_t port)
{
    struct sockaddr_in server;
    memset(&server, '\0', sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = address;
    server.sin_port = htons(port);

    return server;
}

uint8_t query[QUERY_SIZE];

void buildQuery()
{
    memset(query, '\0', sizeof(query));

    time_t now = time(nullptr);
    now += 20637;
    now = bswap_32(now) >> 16;

    query[24] = 0xE2;
    query[25] = 0x07;
    query[26] = 0x0C;
    query[27] = 0x09;
    query[28] = now >> 8;
    query[29] = now & 0x00FF;
}

void quit(int sock)
{
    pthread_join(gRecvP1, nullptr);
    close(sock);
}

void* recvBroadcast(void* arg)
{
    broadcastParams* params = (broadcastParams*)arg;

    struct sockaddr_in server = buildServerType(INADDR_BROADCAST, params->port);

    response target;
    socklen_t length;
    uint8_t buffer[RESPONSE_SIZE];

    while(gKeepGoing)
    {
        ssize_t recv = recvfrom(params->socket, buffer, RESPONSE_SIZE, 0, (sockaddr*)&server, &length);
        memcpy(&target, buffer, RESPONSE_SIZE);

        std::lock_guard<std::mutex> lock(gResponseMutex);
        if(!strcmp(target.serial, gDeviceSerial))
        {
            gFound.push_back(target);
            gKeepGoing = false;
        }
    }

    return nullptr;
}

namespace wion 
{
    void discover(const char* device)
    {
        memset(gDeviceSerial, '\0', 34);
        memcpy(gDeviceSerial, device, strlen(device));

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sock < 0) return;

        int32_t broadcast = 1;
        int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
        if(ret != 0) return;

        struct sockaddr_in server = buildServerType(INADDR_BROADCAST, DISCOVER_PORT);

        broadcastParams params;
        params.socket = sock;
        params.port = DISCOVER_PORT;

        pthread_create(&gRecvP1,  nullptr, recvBroadcast, &params);

        while(gKeepGoing)
        {
            buildQuery();
            sendto(sock, query, QUERY_SIZE, 0, (sockaddr*)&server, sizeof(server));
            usleep(1000*72);
        }

        quit(sock);
    }
}