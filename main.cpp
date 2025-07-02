/*
    @copyright gurotopia (c) 25-5-2024
    @version perent SHA: cd52024244eec9735d487149e22fd3551bf46aeb (1/7/25)
*/
#include "include/pch.hpp"
#include "include/event_type/__event_type.hpp"

#include "include/https/https.hpp"
#include <filesystem>

int main()
{
    /* libary version checker */
#if defined(_MSC_VER)
    printf("microsoft/mimalloc beta-%d\n", MI_MALLOC_VERSION);
    printf("ZTzTopia/enet %d.%d.%d\n", ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH);
    printf("sqlite/sqlite3 %s\n", sqlite3_sourceid());
    printf("openssl/openssl %s\n", OpenSSL_version(OPENSSL_VERSION_STRING));
#else
    printf("\e[38;5;248mmicrosoft/mimalloc \e[1;37mbeta-%d\e[0m\n", MI_MALLOC_VERSION);
    printf("\e[38;5;248mZTzTopia/enet \e[1;37m%d.%d.%d\e[0m\n", ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH);
    printf("\e[38;5;248msqlite/sqlite3 \e[1;37m%s\e[0m\n", sqlite3_sourceid());
    printf("\e[38;5;248mopenssl/openssl \e[1;37m%s\e[0m\n", OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
    if (!std::filesystem::exists("db")) std::filesystem::create_directory("db");

    std::thread(&https::listener, "127.0.0.1", 17091).detach();
    {
        ENetCallbacks callbacks{
            .malloc = &mi_malloc,
            .free = &mi_free,
            .no_memory = []() { printf("ENet memory overflow\n"); }
        };
        enet_initialize_with_callbacks(ENET_VERSION, &callbacks);
    } // @note delete callbacks
    {
        ENetAddress address{};
        address.type = ENET_ADDRESS_TYPE_IPV4;
        address.port = 17091;
        enet_address_is_any(&address);

        server = enet_host_create (ENET_ADDRESS_TYPE_IPV4, &address, 50zu, 2zu, 0, 0);
    } // @note delete address
    
    server->usingNewPacketForServer = true;
    server->checksum = enet_crc32;
    enet_host_compress_with_range_coder(server);
    {
        const uintmax_t size = std::filesystem::file_size("items.dat");

        im_data.resize(im_data.size() + size); // @note state + items.dat
        im_data[0zu] = std::byte{ 04 }; // @note 04 00 00 00
        im_data[4zu] = std::byte{ 0x10 }; // @note 16 00 00 00
        im_data[16zu] = std::byte{ 0x08 }; // @note 08 00 00 00
        *reinterpret_cast<std::uintmax_t*>(&im_data[56zu]) = size;

        std::ifstream("items.dat", std::ios::binary)
            .read(reinterpret_cast<char*>(&im_data[60zu]), size);
    } // @note delete size
    cache_items();

    ENetEvent event{};
    while (true)
        while (enet_host_service(server, &event, 1/*ms*/) > 0)
            if (const auto i = event_pool.find(event.type); i != event_pool.end())
                i->second(event);
    return 0;
}
