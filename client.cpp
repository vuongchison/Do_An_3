#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int main()
{

	const char* ip = "192.168.16.112";

    sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = (in_port_t) htons(554);
    addr.sin_addr.s_addr = inet_addr(ip);
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (connect(s, (sockaddr*) &addr, sizeof(addr)) == -1)
    {
        printf("Connect error.\n");
        return 1;
    }
    else
        printf("Connected\n");

    //Gửi gói tin OPTIONS
    char msg[512];
    sprintf(msg, "OPTIONS rtsp://%s/onvif1 RTSP/1.0\r\nCSeq: 1\r\nUser-Agent: VCS\r\r\n\n", ip);
   
    if (send(s, msg, strlen(msg), 0) == -1)
    {
        printf("Error.\n");
        return 1;
    }
    char buf[256];
    int res = recv(s, buf, sizeof(buf), 0);
    if (res == 0 || res == -1)
    {
    	printf("Error.\n");
        return 1;
    }


    //DESCRIBE
    sprintf(msg, "DESCRIBE rtsp://%s/onvif1 RTSP/1.0\r\nCSeq: 2\r\nUser-Agent: VCS\r\nAccept: application/sdp\r\r\n\n", ip);
    send(s, msg, strlen(msg), 0);
    
    recv(s, buf, sizeof(buf), 0);
    printf("%s.\n", buf);

    //SETUP
    sprintf(msg, "SETUP rtsp://%s/onvif1/track1 RTSP/1.0\r\nCSeq: 3\r\nUser-Agent: VCS\r\nTransport: RTP/AVP;unicast;client_port=30004-30005\r\r\n\n", ip);
	send(s, msg, strlen(msg), 0);
    recv(s, buf, sizeof(buf), 0);
    printf("%s.\n", buf);

    //PLAY
    sprintf(msg, "PLAY rtsp://%s/onvif1 RTSP/1.0\r\nCSeq: 5\r\nUser-Agent: VCS\r\nSession:  2bd7c725Range: npt=0.000-\r\r\n\n", ip);
	send(s, msg, strlen(msg), 0);
    recv(s, buf, sizeof(buf), 0);
    printf("%s.\n", buf);
    
    sockaddr_in recvAddr;
    recvAddr.sin_family = PF_INET;
    recvAddr.sin_port = htons(30004);
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int recvSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bind(recvSocket, (sockaddr*) &recvAddr, sizeof(recvAddr));

    socklen_t len = sizeof(recvAddr);

    //Nhận 100 gói tin RTP chứa dữ liệu 
   	char rtpBuf[2048];
   	for(int i = 0; i < 100; i++)
   {
        int res = recvfrom(recvSocket, rtpBuf, sizeof(rtpBuf), 0, (sockaddr*) &recvAddr, &len);
        printf("%d byte\n%s\n", res, rtpBuf);
   }
    
       
   //PAUSE
    sprintf(msg, "PAUSE rtsp://%s/onvif1 RTSP/1.0\r\nCSeq: 6\r\nUser-Agent: VCS\r\r\n\n", ip);    
    send(s, msg, strlen(msg), 0);
    recv(s, buf, sizeof(buf), 0);

    return 0;
}
