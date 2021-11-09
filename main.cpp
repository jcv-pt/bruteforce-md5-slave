#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <thread>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <time.h>
#include "console-color.h"
#include "mysqlHandler.cpp"
#include "md5.h"
#include "md5.cpp"
#include "md5Generator.cpp"
#include "socket.cpp"
#include "auth.cpp"
#include "commandHandler.cpp"
 
using std::cout; using std::endl;
 
int main(int argc, char *argv[])
{

	//Welcome message :)

	std::cout << std::endl << "MD5 Brute Force - Slave \n --------------------------------------------------------- \n Authors: Joao Vieira [info@joao-vieira.pt] & Rui Gamboias [rui.ismat@gmail.com] \n Description: - This engine uses brute force techniques to determine MD5 hash into strings.\nThis project was developed under the subject of 'Distributed Computation' of the ISMAT University @ Portugal.\n Year : ISMAT @ 2016\n --------------------------------------------------------- \n" << std::endl;
	
	// Initialize listening socket

	ListenSocket  lsock;

	lsock.init();

	//Authenticate client

	Auth  auth(&lsock);

	//Md5 Engine

	md5Generator md5Engine;

	std::thread threadEHandler(&md5Generator::process, &md5Engine);

	//Command Handler

	commandHandler cH(&lsock,&auth,&md5Engine);
	
	std::thread threadCHandler(&commandHandler::handleCommands, &cH);

	//Make main wait for shutdown

	threadCHandler.join();
	threadEHandler.join();

	return 0;

}	
