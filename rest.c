/*
 *  REST web API implementation
 *  Author: Naymushin D.P.
 */

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define h_addr  h_addr_list[0]  // we need only first address in the list

/* Structure of request */
typedef struct _REQUEST {

    unsigned char type; // 0 - GET, 1 - POST
    signed char time; // -1 - NONE, 0 - UTC, 1 - LOCAL
    signed char format; // -1 - NONE, 0 - RFC3339, 1 - UNIX

} REQUEST;

/* Prototype of function that forms string of the request */
char* form_request(REQUEST req);

/* Prototype of function that establish connection */
void set_connection(char *req_text);

/* Prototype of function that send request */
void send_request(int sc, char *req_text);

/* Prototype of function that receive response and print it on the screen */
void get_answer(int sc);

/* Variables for detecting errors of entered arguments */
unsigned char arg1_error;
unsigned char arg2_error;
unsigned char arg3_error;

/* Variable for counter */
unsigned char i;

/* Main function */
int main(int argc, char * argv[]) {

    REQUEST *req = (REQUEST*)malloc(sizeof(REQUEST));

    if(argc == 1) {

        req->type = 0;
        req->time = -1;
        req->format = -1;            

    } else if(argc == 2) {

        arg1_error = 1;

        if(!strcmp(argv[1], "-post")) {

            req->type = 1;
            req->time = -1;
            req->format = -1;

            arg1_error = 0;
        }

    } else if(argc == 3) {

        arg1_error = 1;

        req->type = 0;

        if(!strcmp(argv[1], "-t") && !strcmp(argv[2], "utc")) {

            req->time = 0;
            req->format = -1;

            arg1_error = 0;
        }

        if(!strcmp(argv[1], "-t") && !strcmp(argv[2], "local")) {

            req->time = 1;
            req->format = -1;

            arg1_error = 0;
        }

        if(!strcmp(argv[1], "-f") && !strcmp(argv[2], "internet")) {

            req->time = -1;
            req->format = 0;

            arg1_error = 0;
        }

        if(!strcmp(argv[1], "-f") && !strcmp(argv[2], "unix")) {

            req->time = -1;
            req->format = 1;

            arg1_error = 0;
        }

    } else if(argc == 4) {

        arg1_error = 1;
        arg2_error = 1;

        for(i = 1; i < 4; i++) {

            if(!strcmp(argv[i], "-post")) {

                req->type = 1;

                arg1_error = 0;
            }
        }

        for(i = 1; i < 3; i++) {

            if(!strcmp(argv[i], "-t") && !strcmp(argv[i + 1], "utc")) {

                req->time = 0;
                req->format = -1;

                arg2_error = 0;
            }

            if(!strcmp(argv[i], "-t") && !strcmp(argv[i + 1], "local")) {

                req->time = 1;
                req->format = -1;

                arg2_error = 0;
            }

            if(!strcmp(argv[i], "-f") && !strcmp(argv[i + 1], "internet")) {

                req->time = -1;
                req->format = 0;

                arg2_error = 0;
            }

            if(!strcmp(argv[i], "-f") && !strcmp(argv[i + 1], "unix")) {

                req->time = -1;
                req->format = 1;

                arg2_error = 0;
            }
        }

    } else if(argc == 5) {

        arg1_error = 1;
        arg2_error = 1;

        req->type = 0;

        for(i = 1; i < 4; i += 2) {

            if(!strcmp(argv[i], "-t") && !strcmp(argv[i + 1], "utc")) {

                req->time = 0;
                req->format = -1;

                arg1_error = 0;
            }

            if(!strcmp(argv[i], "-t") && !strcmp(argv[i + 1], "local")) {

                req->time = 1;
                req->format = -1;

                arg1_error = 0;
            }

            if(!strcmp(argv[i], "-f") && !strcmp(argv[i + 1], "internet")) {

                req->time = -1;
                req->format = 0;

                arg2_error = 0;
            }

            if(!strcmp(argv[i], "-f") && !strcmp(argv[i + 1], "unix")) {

                req->time = -1;
                req->format = 1;

                arg2_error = 0;
            }
        }

    } else if(argc == 6) {

        arg1_error = 1;
        arg2_error = 1;
        arg3_error = 1;

        for(i = 1; i < 6; i++) {

            if(!strcmp(argv[i], "-post")) {

                req->type = 1;

                arg1_error = 0;
            }
        }

        for(i = 1; i < 5; i++) {

            if(!strcmp(argv[i], "-t") && !strcmp(argv[i + 1], "utc")) {

                req->time = 0;
                req->format = -1;

                arg2_error = 0;
            }

            if(!strcmp(argv[i], "-t") && !strcmp(argv[i + 1], "local")) {

                req->time = 1;
                req->format = -1;

                arg2_error = 0;
            }

            if(!strcmp(argv[i], "-f") && !strcmp(argv[i + 1], "internet")) {

                req->time = -1;
                req->format = 0;

                arg3_error = 0;
            }

            if(!strcmp(argv[i], "-f") && !strcmp(argv[i + 1], "unix")) {

                req->time = -1;
                req->format = 1;

                arg3_error = 0;
            }
        }

    } else {

        arg1_error = 1;
    }

    if(arg1_error || arg2_error || arg3_error) {

        printf("\nError: wrong input!\n");
        printf("List of available commands: -post, -t utc, -t local,\n");
        printf("-f unix, -f internet\n");

        return 0;
    }

    char *request_str = form_request(*req);
        
    set_connection(request_str);

    free(req);
    free(request_str);

    return 0;
}

/* Function that forms string of request */
char *form_request(REQUEST req) {

    const char *GET_HEADER_PART1 = "GET /WebApi/time";
    const char *GET_HEADER_PART2 = " HTTP/1.0\n\n";

    const char *POST_HEADER_PART1 = "POST /WebApi/time HTTP/1.0\nContent-Type:";
    const char *POST_HEADER_PART2 = "application/x-www-form-urlencoded\nContent-Length:";

    char *answer = (char*)calloc(130, sizeof(char));

    if(req.type == 0) {

        if(req.time == -1 && req.format == -1) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, GET_HEADER_PART2);

        } else if(req.time == -1 && req.format == 0) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?format=internet");
            answer = strcat(answer, GET_HEADER_PART2);            

        } else if(req.time == -1 && req.format == 1) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?format=unix");
            answer = strcat(answer, GET_HEADER_PART2);            

        } else if(req.time == 0 && req.format == -1) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?type=utc");
            answer = strcat(answer, GET_HEADER_PART2);            

        } else if(req.time == 0 && req.format == 0) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?type=utc&format=internet");
            answer = strcat(answer, GET_HEADER_PART2);            

        } else if(req.time == 0 && req.format == 1) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?type=utc&format=unix");
            answer = strcat(answer, GET_HEADER_PART2);            

        } else if(req.time == 1 && req.format == -1) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?type=local");
            answer = strcat(answer, GET_HEADER_PART2);            

        } else if(req.time == 1 && req.format == 0) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?type=local&format=internet");
            answer = strcat(answer, GET_HEADER_PART2);             

        } else if(req.time == 1 && req.format == 1) {

            answer = strcat(answer, GET_HEADER_PART1);
            answer = strcat(answer, "?type=local&format=unix");
            answer = strcat(answer, GET_HEADER_PART2);            

        }

    } else {

        if(req.time == -1 && req.format == -1) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "0\n\n\n");

        } else if(req.time == -1 && req.format == 0) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "15\n\nformat=internet\n");            

        } else if(req.time == -1 && req.format == 1) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "11\n\nformat=unix\n");           

        } else if(req.time == 0 && req.format == -1) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "8\n\ntype=utc\n");               

        } else if(req.time == 0 && req.format == 0) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "24\n\ntype=utc&format=internet\n");               

        } else if(req.time == 0 && req.format == 1) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "20\n\ntype=utc&format=unix\n");            

        } else if(req.time == 1 && req.format == -1) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "10\n\ntype=local\n");            

        } else if(req.time == 1 && req.format == 0) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "26\n\ntype=local&format=internet\n");             

        } else if(req.time == 1 && req.format == 1) {

            answer = strcat(answer, POST_HEADER_PART1);
            answer = strcat(answer, POST_HEADER_PART2);
            answer = strcat(answer, "22\n\ntype=local&format=unix\n");            

        }

    }

    return answer;
}

/* Function that establish connection */
void set_connection(char* req_text) {

    int sc;

    const char *URL = "www.iu3.bmstu.ru";
    const int port = 80;

    struct sockaddr_in ad;
    struct hostent *ht;
    
    sc = socket(AF_INET, SOCK_STREAM, 0);

    if (sc < 0) {

        printf("\nAn error occurred while creating the socket!\n");

        return;
    }    

    ht = (struct hostent*)gethostbyname(URL);

    if (ht == NULL) {

        printf("\nAn error occurred while distributing the IP-address!\n");

        return;
    }

    /* Prepares structure by filling it with zeros */
    memset(&ad, 0, sizeof(ad));

    ad.sin_family = AF_INET;

    /* Copies first address from 'hostent' structure */
    memcpy(&ad.sin_addr.s_addr, ht->h_addr, ht->h_length);

    /* Server port number */
    ad.sin_port = htons(port);

    if (connect(sc, (struct sockaddr *)&ad,sizeof(ad)) < 0) {

        printf("\nAn error ocurred while establishing the connection!\n");

        return;
    }    

    send_request(sc, req_text);
}       

/* Function that sends request */
void send_request(int sc, char* req_text) {

    int n = 0;
    int len = strlen(req_text);

    while (len > 0) {

        n = write(sc, &req_text[n], len);
        
        if (n < 0)
            break;
        
        len -= n;
    }

    if (n < 0) { 

        printf("\nAn error occurred while sending request!\n");
        return;
    }

    get_answer(sc);
}

/* Function that receives and prints response on the screen */
void get_answer(int sc) {

    char resp[1024]; 

    int n = read(sc, resp, 1023);

    if (n > 0) {

        resp[n] = 0;

        if(strstr(resp, "200") == NULL) {

            printf("Error: code %c%c%c\n", resp[9], resp[10], resp[11]);
            return;
        }

        printf("%s", resp);
        
    } else
        printf("An error occurred while receiving response!\n");

    printf("\n");
}