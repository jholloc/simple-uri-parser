#include <catch2/catch_test_macros.hpp>

#include "uri_parser.h"

TEST_CASE( "Empty string is invalid", "[scheme]" ) {
    auto uri = uri::parse_uri("");
    REQUIRE( uri.error == uri::Error::InvalidScheme );
}

TEST_CASE( "Empty scheme is invalid", "[scheme]" ) {
    auto uri = uri::parse_uri(":");
    REQUIRE( uri.scheme.empty() );
    REQUIRE( uri.error == uri::Error::InvalidScheme );
}

TEST_CASE( "Scheme containing invalid characters is invalid", "[scheme]" ) {
    auto uri = uri::parse_uri("news*:");
    REQUIRE( uri.error == uri::Error::InvalidScheme );
}

TEST_CASE( "Valid scheme", "[scheme]" ) {
    auto uri = uri::parse_uri("news:");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.scheme == "news" );
}

TEST_CASE( "Valid authority with just host", "[auth]" ) {
    auto uri = uri::parse_uri("news://example.com");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.authority.host == "example.com" );
}

TEST_CASE( "Valid authority with userinfo & host", "[auth]" ) {
    auto uri = uri::parse_uri("news://user@example.com");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.authority.userinfo == "user" );
    REQUIRE( uri.authority.host == "example.com" );
}

TEST_CASE( "Valid authority with userinfo, host & port", "[auth]" ) {
    auto uri = uri::parse_uri("news://user@example.com:5432");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.authority.userinfo == "user" );
    REQUIRE( uri.authority.host == "example.com" );
    REQUIRE( uri.authority.port == 5432 );
}

TEST_CASE( "Valid authority with invalid port", "[auth]" ) {
    auto uri = uri::parse_uri("news://user@example.com:52f");
    REQUIRE( uri.error == uri::Error::InvalidPort );
}

TEST_CASE( "Valid empty path with authority", "[path]" ) {
    auto uri = uri::parse_uri("news://user@example.com:5432/");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/" );
}

TEST_CASE( "Valid path with authority", "[path]" ) {
    auto uri = uri::parse_uri("news://user@example.com:5432/test/path");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/test/path" );
}

TEST_CASE( "Valid path with empty authority", "[path]" ) {
    auto uri = uri::parse_uri("news:///test/path");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/test/path" );
}

TEST_CASE( "Valid path with no authority", "[path]" ) {
    auto uri = uri::parse_uri("news:/test/path");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/test/path" );
}

TEST_CASE( "Valid path without slashes and with no authority", "[path]" ) {
    auto uri = uri::parse_uri("news:test_path");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "test_path" );
}

TEST_CASE( "Valid query with path and authority", "[query]" ) {
    auto uri = uri::parse_uri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/forum/questions/" );
    REQUIRE( uri.query_string == "tag=networking&order=newest" );
    REQUIRE( uri.query.size() == 2 );
    REQUIRE( uri.query.at("tag") == "networking" );
    REQUIRE( uri.query.at("order") == "newest" );
}

TEST_CASE( "Valid query with path, authority and fragment", "[query]" ) {
    auto uri = uri::parse_uri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/forum/questions/" );
    REQUIRE( uri.query_string == "tag=networking&order=newest" );
    REQUIRE( uri.query.size() == 2 );
    REQUIRE( uri.query.at("tag") == "networking" );
    REQUIRE( uri.query.at("order") == "newest" );
}

TEST_CASE( "Valid query with empty arg", "[query]" ) {
    auto uri = uri::parse_uri("https://john.doe@www.example.com:123/forum/questions/?tag;order=newest");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.path == "/forum/questions/" );
    REQUIRE( uri.query.size() == 2 );
    REQUIRE( uri.query.at("tag").empty() );
    REQUIRE( uri.query.at("order") == "newest" );
}

TEST_CASE( "Valid valid fragment with query", "[fragment]" ) {
    auto uri = uri::parse_uri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.fragment == "top" );
}

TEST_CASE( "Test IPv4 authority", "[full]" ) {
    auto uri = uri::parse_uri("telnet://192.0.2.16:80/");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.scheme == "telnet" );
    REQUIRE( uri.authority.host == "192.0.2.16" );
    REQUIRE( uri.authority.port == 80 );
    REQUIRE( uri.path == "/" );
    REQUIRE( uri.query.empty() );
}

TEST_CASE( "Test IPv6 authority", "[full]" ) {
    auto uri = uri::parse_uri("ldap://[2001:db8::7]/c=GB?objectClass?one");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.scheme == "ldap" );
    REQUIRE( uri.authority.host == "[2001:db8::7]" );
    REQUIRE( uri.path == "/c=GB" );
    REQUIRE( uri.query.size() == 2 );
    REQUIRE( uri.query.at("objectClass").empty() );
    REQUIRE( uri.query.at("one").empty() );
}

TEST_CASE( "Test numeric path", "[full]" ) {
    auto uri = uri::parse_uri("tel:+1-816-555-1212");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.scheme == "tel" );
    REQUIRE( uri.path == "+1-816-555-1212" );
    REQUIRE( uri.query.empty() );
}

TEST_CASE( "Test path with colons with no authority", "[full]" ) {
    auto uri = uri::parse_uri("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
    REQUIRE( uri.error == uri::Error::None );
    REQUIRE( uri.scheme == "urn" );
    REQUIRE( uri.path == "oasis:names:specification:docbook:dtd:xml:4.1.2" );
    REQUIRE( uri.query.empty() );
}
