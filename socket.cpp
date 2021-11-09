#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>

#include <netinet/in.h>

/*********************   CLASS DECLARATION    *********************/

class ListenSocket {

	private:
		int sockfd, newsockfd, portno;
		socklen_t clilen,n;
		char ip[INET_ADDRSTRLEN];
		bool socketBusy;
		char buffer[256];
		const char * clientName;
		struct sockaddr_in serv_addr, cli_addr;

		void error(const char * message);

	public:
		ListenSocket(void); //constructor
		~ListenSocket(void); //destructor

		bool isAuth;
		bool isRaw;

		void init();
		void connect();
		void write(const char * command);
		char * read();
		void close();
		bool hasClient(void);
		char * getIp(void);

};

/*********************   FUNCTIONS    *********************/


/** Constructor */
ListenSocket::ListenSocket(void) {

	this->socketBusy = false;
	this->isAuth = false;
	this->portno = 3000;
	this->clientName = "\n#client ";
	this->isRaw = false;

}

/** Destructor */
ListenSocket::~ListenSocket(void) {

}

void ListenSocket::init(){


	printf("Initializing socket on port: %d\n",this->portno);

	this->connect();

}

void ListenSocket::connect(){

	if(this->socketBusy==true)
	{
		this->write("Connection already in use!");
		this->close();
		return;
	}

	int iSetOption = 1;

	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));

	if (this->sockfd < 0) 
		this->error("Could not open listen socket");

	bzero((char *) &this->serv_addr, sizeof(this->serv_addr));

	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(this->portno);

	if (bind(this->sockfd, (struct sockaddr *) &this->serv_addr, sizeof(this->serv_addr)) < 0) 
		this->error("ERROR on binding");

	listen(this->sockfd,1);
	this->clilen = sizeof(this->cli_addr);

	this->newsockfd = accept(this->sockfd, (struct sockaddr *) &this->cli_addr, &this->clilen);

	if (this->newsockfd < 0) 
		this->error("ERROR on accept");

	this->socketBusy = true;

	bzero(this->buffer,256);

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&this->cli_addr;
	struct in_addr ipAddr = pV4Addr->sin_addr;

	inet_ntop( AF_INET, &ipAddr, this->ip, INET_ADDRSTRLEN );

	fd_set fdset;
	struct timeval tv;
	FD_ZERO(&fdset);
	FD_SET(this->newsockfd, &fdset);
	tv.tv_sec = 4;
	tv.tv_usec = 500000;

	if (select(this->newsockfd + 1, NULL, &fdset, NULL, &tv) == 1)
	{
		setsockopt(this->newsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
		setsockopt(this->newsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	}

	this->write("Connection accepted go ahead...\n");

}

void ListenSocket::write(const char * command){

	char * output;

	if(this->isAuth && this->isRaw==false)
	{

		output = (char*) malloc(255*sizeof(char)); 

		strcpy(output, command); /* copy name into the new var */
		strcat(output, this->clientName); /* add the extension */

		this->n = ::write(this->newsockfd,output,strlen(output));

		free(output);

	}
	else
	{
		this->n = ::write(this->newsockfd,command,strlen(command));
	}

	if (this->n < 0) 
		this->error("ERROR writing to socket");

}

char * ListenSocket::read(){

	bzero(this->buffer,256);

	memset(this->buffer, 0, sizeof(this->buffer));

	this->n = ::read(this->newsockfd,this->buffer,255);
	
	if (this->n < 0) 
		this->error("ERROR reading from socket");


	int i=0;

	for(i=0;i<=255;i++)
	{
		if((int)this->buffer[i]==13)
		{
			this->buffer[i] = '\0';
		}
	}

	char * output = (char*) malloc(256*sizeof(char));
	strcpy(output, this->buffer);

	return output;

}

void ListenSocket::close(){

	::close(this->newsockfd);
	::close(this->sockfd);

	std::cout << "Connection closed on " << this->ip << std::endl;

	this->socketBusy = false;
	this->isAuth = false;

}

bool ListenSocket::hasClient(void)
{
	return this->socketBusy;
}


void ListenSocket::error(const char * msg)
{
	perror(msg);
	exit(1);
}

char * ListenSocket::getIp(void)
{

	return this->ip;

}



