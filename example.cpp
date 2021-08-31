#include <iostream>

#include "uri_parser.h"

int main()
{
    auto uri = uri::parse_uri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");

    std::cout << uri.scheme << std::endl;               // https
    std::cout << uri.authority.authority << std::endl;  // john.doe@www.example.com:123
    std::cout << uri.authority.userinfo << std::endl;   // john.doe
    std::cout << uri.authority.host << std::endl;       // www.example.com
    std::cout << uri.authority.port << std::endl;       // 123
    std::cout << uri.path << std::endl;                 // /forum/questions/
    std::cout << uri.query_string << std::endl;         // ?tag=networking&order=newest
    std::cout << uri.query.size() << std::endl;         // 2
    std::cout << uri.query.at("tag") << std::endl;   // networking
    std::cout << uri.query.at("order") << std::endl; // newest
    std::cout << uri.fragment << std::endl;             // top
}