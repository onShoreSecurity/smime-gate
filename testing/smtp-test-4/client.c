/**
 * smtp-test-4 (client) - interactive SMTP client, command written by hand,
 *                        no client logic
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "system.h"
#include "smtp.h"
#include "smtp-lib.h"
#include "smtp-types.h"

#define SMTP_PORT   5780

void str_cli (int sockfd);


int main (int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("usage: client <IPaddress>");

    /* create socket */
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SMTP_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    /* connect to server */
    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    /* run client */
    str_cli(sockfd);

    exit(0);
}

void str_cli (int sockfd)
{
    ssize_t n;
    size_t len;
    char line[100];
    char mail_data[] = 
        "From: Sender <sender@example.org>\r\n"
        "Message-Id: <200908251906.k7GKfq6v011871@tree.slackware.lan>\r\n"
        "Date: Tue, 25 Aug 2009 19:06:38 -0500\r\n"
        "To: Recipient1 <rcpt1@example.org>, Recipient2 <rcpt2@example.org>\r\n"
        "Subject: Register with the Linux counter project\r\n"
        "User-Agent: nail 11.25 7/29/05\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/plain; charset=us-ascii\r\n"
        "Content-Transfer-Encoding: 7bit\r\n"
        "\r\n"
        "(Adapted from a question in the Linux-FAQ)\r\n"
        "\r\n"
        "How Many People Use Linux?\r\n"
        "\r\n"
        "Linux is freely available, and no one is required to register with\r\n"
        "any central authority, so it is difficult to know.  Several businesses\r\n"
        "survive solely on selling and supporting Linux.  Linux newsgroups are\r\n"
        "some of the most heavily read on Usenet.  Accurate numbers are hard to\r\n"
        "come by, but the number is almost certainly in the millions.\r\n"
        "\r\n";

    for (;;) {
        /* get a line from server */
        if ( (n = smtp_readline(sockfd, line, MAXLINE)) == 0) {
            printf("Closed connection!\n");
            return;
        }
        /* print it */
        printf("R: |%s|\n", line);

        /* get a command (line) from stdin */
        fgets(line, 100, stdin);
        len = strlen(line);
        line[len-1] = '\0';
        len--;

        /* if command id "mdata", sent mail data */
        if (0 == strncmp(line, "mdata", 5)) {
            len = strlen(mail_data);
            writen(sockfd, mail_data, len); /* send mail data */
            writen(sockfd, ".\r\n", 3);     /* end with .CRLF */

            printf("Mail data sent!\n");
        }
        /* else print and send command do server */
        else {
            printf("C: |%s|\n", line);

            line[len++] = '\r';
            line[len++] = '\n';
            line[len++] = '\0';

            writen(sockfd, line, len-1);
        }
    }
}

