// TscBroadcastTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <intrin.h>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>
#include "platform.h"


int main(int argc, char ** argv)
{
    size_t serverCpuId = atoi(argv[1]);
    size_t clientCpuId = atoi(argv[2]);
    size_t samples = atoi(argv[3]);
    std::vector<unsigned long long> tsClient(samples);
    std::vector<unsigned long long> tsServer(samples);
    __declspec(align(64)) struct {
        std::atomic<bool> clientOwns;
    } b;
    b.clientOwns.store(false);
    auto client = std::thread([&tsClient, &b, samples, serverCpuId]() {
        SetThreadAffinity(serverCpuId);
        unsigned int i;
        for(size_t index = 0; index < samples; index++)
        {
            while (b.clientOwns.load() == false)
            {
            }
            unsigned long long ts = __rdtscp(&i);
            b.clientOwns.store(false);
            tsClient[index] = ts;
        }
    });
    auto server = std::thread([&tsServer, &b, samples, clientCpuId]() {
        SetThreadAffinity(clientCpuId);
        unsigned int i;
        for (size_t index = 0; index < samples; index++)
        {
            while (b.clientOwns.load() == true)
            {
            }
            unsigned long long ts = __rdtscp(&i);
            b.clientOwns.store(true);
            tsServer[index] = ts;
        }
    });
    client.join();
    server.join();

    long long avgOffset = 0;
    for (size_t i = 0; i < samples-1; i++)
    {
        long long offset = (2 * (long long)tsClient[i] - (long long)tsServer[i] - (long long)tsServer[i + 1]) / 2;
        avgOffset += offset;
    }

    printf("%I64i\n", avgOffset / (long long)samples - 1);
}