#ifndef SIMPLE_URI_PARSER_LIBRARY_H
#define SIMPLE_URI_PARSER_LIBRARY_H

#include <string>
#include <unordered_map>
#include <algorithm>

namespace uri {

using query_type = std::unordered_map<std::string, std::string>;

enum class Error {
    None,
    InvalidScheme,
    InvalidPort,
};

struct Authority {
    std::string authority;
    std::string userinfo;
    std::string host;
    long port = 0;
};

namespace impl {

bool valid_scheme(std::string_view scheme) {
    if (scheme.empty()) {
        return false;
    }
    auto pos = std::find_if_not(scheme.begin(), scheme.end(), [&](char c){
        return std::isalnum(c) || c == '+' || c == '.' || c == '-';
    });
    return pos == scheme.end();
}

std::tuple<std::string, Error, std::string_view> parse_scheme(std::string_view uri) {
    auto pos = uri.find(':');
    if (pos == std::string_view::npos) {
        return { "", Error::InvalidScheme, uri };
    }

    auto scheme = uri.substr(0, pos);
    if (!impl::valid_scheme(scheme)) {
        return { "", Error::InvalidScheme, uri };
    }
    std::string scheme_string{ scheme };
    std::transform(scheme_string.begin(), scheme_string.end(), scheme_string.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    uri = uri.substr(pos + 1);
    return { scheme_string, Error::None, uri };
}

std::tuple<Authority, Error, std::string_view> parse_authority(std::string_view uri) {
    Authority authority;

    bool has_authority = uri.length() >= 2 && uri[0] == '/' && uri[1] == '/';
    if (!has_authority) {
        return { authority, Error::None, uri };
    }

    auto pos = uri.substr(2).find('/');
    auto auth_string = uri.substr(2, pos);
    authority.authority = auth_string;

    pos = auth_string.find('@');
    if (pos != std::string_view::npos) {
        authority.userinfo = std::string(auth_string.substr(0, pos));
        auth_string = auth_string.substr(pos + 1);
    }

    char* end_ptr = nullptr;
    if (!auth_string.empty() && auth_string[0] != '[') {
        pos = auth_string.find(':');
        if (pos != std::string_view::npos) {
            authority.port = std::strtol(&auth_string[pos + 1], &end_ptr, 10);
            if (end_ptr != auth_string.end()) {
                return { authority, Error::InvalidPort, auth_string };
            }
        }
    }

    authority.host = auth_string.substr(0, pos);

    return { authority, Error::None, end_ptr == nullptr ? auth_string.end() : end_ptr };
}

std::tuple<std::string, Error, std::string_view> parse_path(std::string_view uri) {
    auto pos = uri.find_first_of("#?");
    if (pos == std::string_view::npos) {
        auto path = std::string(uri);
        return { path, Error::None, uri.end() };
    } else {
        auto path = std::string(uri.substr(0, pos));
        return { path, Error::None, uri.substr(pos + 1) };
    }
}

std::tuple<query_type, std::string, Error, std::string_view> parse_query(std::string_view uri) {
    auto hash_pos = uri.find('#');
    auto query_substring = uri.substr(0, hash_pos);
    auto query_string = std::string(query_substring);
    query_type query;
    while (!query_substring.empty()) {
        auto delim_pos = query_substring.find_first_of("&;?", 0);
        auto arg = query_substring.substr(0, delim_pos);
        auto equals_pos = arg.find('=');
        if (equals_pos == std::string_view::npos) {
            query[std::string(arg)] = "";
        } else {
            query[std::string(arg.substr(0, equals_pos))] = arg.substr(equals_pos + 1);
        }
        if (delim_pos == std::string_view::npos) {
            query_substring = "";
        } else {
            query_substring = query_substring.substr(delim_pos + 1);
        }
    }

    return {query, query_string, Error::None, uri.substr(hash_pos + 1) };
}

std::tuple<std::string, Error, std::string_view> parse_fragment(std::string_view uri) {
    return { std::string(uri), Error::None, uri };
}

} // namespace impl

struct Uri {
    Error error;
    std::string scheme;
    Authority authority = {};
    std::string path;
    query_type query = {};
    std::string query_string;
    std::string fragment;
};

Uri parse_uri(std::string_view uri) {
    Error error;

    std::string scheme;
    std::tie(scheme, error, uri) = impl::parse_scheme(uri);
    if (error != Error::None) {
        return Uri{ error };
    }

    Authority authority;
    std::tie(authority, error, uri) = impl::parse_authority(uri);
    if (error != Error::None) {
        return Uri{ error };
    }

    std::string path;
    std::tie(path, error, uri) = impl::parse_path(uri);
    if (error != Error::None) {
        return Uri{ error };
    }

    query_type query;
    std::string query_string;
    std::tie(query, query_string, error, uri) = impl::parse_query(uri);
    if (error != Error::None) {
        return Uri{ error };
    }

    std::string fragment;
    std::tie(fragment, error, uri) = impl::parse_fragment(uri);
    if (error != Error::None) {
        return Uri{ error };
    }

    return Uri{
        .error = Error::None,
        .scheme = scheme,
        .authority = authority,
        .path = path,
        .query = query,
        .query_string = query_string,
        .fragment = fragment,
    };
}

} // namespace uri

#endif // SIMPLE_URI_PARSER_LIBRARY_H
