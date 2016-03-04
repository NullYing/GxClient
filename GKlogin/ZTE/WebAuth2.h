#ifndef WEBAUTH2_H
#define WEBAUTH2_H


char * TYlogin(const char * username, const char *password, char *ip, u_char mac[6], const char * test_url, const int timeout);
char * TYlogout(const char * username, char *ip, u_char mac[6], const char * test_url, const int timeout);
char * TYkeep(const char * username, char *ip, u_char mac[6], const char * test_url, const int timeout);
#endif