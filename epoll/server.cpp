#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

using namespace std;

#define MAXLINE 5
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 8080
#define INFTIM 1000

void setnonblocking(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    int i, maxi, listenfd, connfd, sockfd,epfd,nfds, portnumber;
    ssize_t n;
    char line[MAXLINE];
    socklen_t clilen;


    if ( 2 == argc )
    {
        if( (portnumber = atoi(argv[1])) < 0 )
        {
            fprintf(stderr,"Usage:%s portnumber/a/n",argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr,"Usage:%s portnumber/a/n",argv[0]);
        return 1;
    }



    //ev used to regiter and events used to store events need to be solved

    struct epoll_event ev,events[20];
    //how many socket need to listen

    epfd=epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //set socket to be unblock

    //setnonblocking(listenfd);


    ev.data.fd=listenfd;

    ev.events=EPOLLIN|EPOLLET;
    //ev.events=EPOLLIN;

    //set listenfd to be listen event
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    
    
    bzero(&serveraddr, sizeof(serveraddr));


    serveraddr.sin_family = AF_INET;
    char *local_addr="127.0.0.1";
    inet_aton(local_addr,&(serveraddr.sin_addr));//htons(portnumber);    
    serveraddr.sin_port=htons(portnumber);

    //bind
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));

    //listen
    listen(listenfd, LISTENQ);
    maxi = 0;
    for ( ; ; ) {
        //wait epoll to happen

        nfds=epoll_wait(epfd,events,20,500);
        //handle

        for(i=0;i<nfds;++i)
        {
            //new connect event
            if(events[i].data.fd==listenfd)

            {
                connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen);
                if(connfd<0){
                    perror("connfd<0");
                    exit(1);
                }
                //setnonblocking(connfd);

                char *str = inet_ntoa(clientaddr.sin_addr);
                cout << "accapt a connection from " << str << endl;
                
                //set fd as connect

                ev.data.fd=connfd;
            
                //expect read next time
                ev.events=EPOLLIN|EPOLLET;
                //ev.events=EPOLLIN;

                //change registered ev

                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
            }
            //if it's read event
            else if(events[i].events&EPOLLIN)

            {
                cout << "EPOLLIN" << endl;
                if ( (sockfd = events[i].data.fd) < 0)
                    continue;
                if ( (n = read(sockfd, line, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        close(sockfd);
                        events[i].data.fd = -1;
                    } else
                        std::cout<<"readline error"<<std::endl;
                } else if (n == 0) {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                line[n] = '/0';
                cout << "read " << line << endl;
                //set fd for write

                ev.data.fd=sockfd;
                
                //expect write next time
                ev.events=EPOLLOUT|EPOLLET;
                //change registerd ev

                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);

            }
            else if(events[i].events&EPOLLOUT)

            {
                sockfd = events[i].data.fd;
                write(sockfd, line, n);
                //set ev

                ev.data.fd=sockfd;
                
                //expect read next time
                ev.events=EPOLLIN|EPOLLET;
                //change registered ev

                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            }
        }
    }
    return 0;
}
