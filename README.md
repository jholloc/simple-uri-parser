![Build](https://github.com/jholloc/simple-uri-parser/actions/workflows/cmake.yml/badge.svg)

# Simple URI Parser

This is a simple, header only C++ library to allow parsing of URIs as defined by
the URI generic schema (https://datatracker.ietf.org/doc/html/rfc3986).

This schema looks like

```
URI = scheme:[//authority]path[?query][#fragment]
```

Where the authority component is divided into the following components:

```
authority = [userinfo@]host[:port]
```

## Usage

The library provides one function `uri::parse_uri` which takes a string argument and 
return a `uri::URI` structure. This structure has public fields which contain all the
data parsed from the URI.

If an invalid URI is passed then the `uri.error` field is set to one of the errors in
the `uri::Error` enum class and an empty `URI` structure is returned.

### Example

```c++
#include <iostream>

#include "uri_parser.h"

int main()
{
    auto uri = uri::parse_uri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    
    std::cout << uri.scheme << std::endl;               // https
    std::cout << uri.authority.userinfo << std::endl;   // john.doe
    std::cout << uri.authority.host << std::endl;       // www.example.com
    std::cout << uri.authority.port << std::endl;       // 123
    std::cout << uri.path << std::endl;                 // /forum/questions/
    std::cout << uri.query_string << std::endl;         // ?tag=networking&order=newest
    std::cout << uri.query.size() << std::endl;         // 2
    std::cout << uri.query.at("tag") << std::endl;      // networking
    std::cout << uri.query.at("order") << std::endl;    // newest
    std::cout << uri.fragment << std::endl;             // top
}
```

## Running tests

While the library is header only and can be included without any build required,
there are a number of tests included which can be built and run using the following
(from the library root directory):

```bash
cmake -Bbuild
cmake --build build/
./build/tests
```
