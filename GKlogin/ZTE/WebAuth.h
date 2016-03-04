#ifndef WEBAUTH_H
#define WEBAUTH_H


char* WebAuth(const char * username, const char *password, const char *ip, const char *base_url, const int timeout, const char *test_url);
char * WebLogout(const char *ip, const char *base_url, const int timeout);
char *WebAuthMessages(const char *url, const char *ip, const int timeout);
#endif