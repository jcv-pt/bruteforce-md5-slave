/*********************   CLASS DECLARATION    *********************/

class commandHandler {

	private:
		ListenSocket * lsock;
		Auth * auth;
		md5Generator * md5Engine;

	public:
		commandHandler(ListenSocket * lsock, Auth * auth, md5Generator * Engine); //constructor
		~commandHandler(void); //destructor

		void handleCommands();
		bool isShutdown();

};


/*********************   FUNCTIONS    *********************/


/** Constructor */
commandHandler::commandHandler(ListenSocket * lsock, Auth * auth, md5Generator * Engine) {

	this->lsock = lsock;
	this->auth = auth;
	this->md5Engine = Engine;

}

/** Destructor */
commandHandler::~commandHandler(void) {

}

void commandHandler::handleCommands(){

	char * read;

	bool running = true;

	while(running){

		//If it has a conection

		if(this->lsock->hasClient())
		{

			//Authenticate

			if(!this->auth->isAuthenticated())
				this->auth->authenticate();
			else
			{

				//Process commands

				read = this->lsock->read();

				if(strcmp(read,"quit") == 0)
				{

					this->auth->disconnect();

				}
				else if(strcmp(read,"shutdown") == 0)
				{

					this->lsock->write("Shutting down daemon...\n");
					std::cout << "Client requested a daemon shutdown, engine is going halted..." << std::endl;

					this->md5Engine->shutdown();					
					this->auth->disconnect();
					
					running = false;
					
				}
				else if(strcmp(read,"process") == 0)
				{

					if(this->md5Engine->isRunning()==true)
					{
						
						this->lsock->write("Error! Cannot process, engine is running, use reset command.");
						std::cout << "Client requested a process but engine is already running." << std::endl;

						continue;
					}

					if(this->md5Engine->isMd5Set()!=true)
					{
						
						this->lsock->write("Error! Cannot process, MD5 is not set, use setmd5 <arg> command.");
						std::cout << "Client requested a process but MD5 is not set." << std::endl;

						continue;
					}

					if(this->md5Engine->isSequenceSet()!=true)
					{
						
						this->lsock->write("Error! Cannot process, sequence is not set, use setsequence <arg> command.");
						std::cout << "Client requested a process but MD5 sequence is not set." << std::endl;

						continue;
					}

				}
				else if(strcmp(read,"setmd5") == 0)
				{

					this->lsock->write("Provide new md5:");

					char * newMd5 =  this->lsock->read();

					this->md5Engine->setMd5(newMd5);

					this->lsock->write("MD5 has been set.");


				}
				else if(strcmp(read,"setsequence") == 0)
				{

					if(this->md5Engine->isRunning()==true)
					{
						
						this->lsock->write("Error! Cannot set new sequence, engine is running, use reset command.");
						std::cout << "Client requested a set sequence but engine is running." << std::endl;

						continue;
					}
std::cout << "New sequence"  << std::endl;

					this->lsock->write("Provide new sequence:");

					int newSeq =  atoi(this->lsock->read());

					this->md5Engine->setSequence(newSeq);

					this->lsock->write("Sequence has been set.");
					std::cout << "New sequence value has been set: '" << newSeq << "'" << std::endl;

				}
				else if(strcmp(read,"usesql") == 0)
				{

					do 
					{
						this->lsock->write("Use MYSQL? (y,n)");

						read = this->lsock->read();

					}while(strcmp(read,"y") != 0 && strcmp(read,"n") != 0);

					if(strcmp(read,"y") == 0)
					{
						this->md5Engine->useSql(true);

						this->lsock->write("MYSQL has been enabled.");
						std::cout << "MYSQL has been enabled."<< std::endl;

					}
					else
					{
						this->md5Engine->useSql(false);

						this->lsock->write("MYSQL has been disabled.");
						std::cout << "MYSQL has been disabled."<< std::endl;
					}


				}
				else if(strcmp(read,"truncate") == 0)
				{

					if(this->md5Engine->isRunning()==true)
					{
						
						this->lsock->write("Error! Cannot truncate, engine is running, use reset command.");
						std::cout << "Client requested a truncate call but engine is running." << std::endl;

						continue;

					}

					do 
					{
						this->lsock->write("Erase all MYSQL records? (y,n)");

						read = this->lsock->read();

					}while(strcmp(read,"y") != 0 && strcmp(read,"n") != 0);

					if(strcmp(read,"y") == 0)
					{
						this->md5Engine->truncate();

						this->lsock->write("MYSQL records have been erased.");
						std::cout << "MYSQL records have been erased, table has been truncated."<< std::endl;

					}


				}
				else if(strcmp(read,"reset") == 0)
				{

					this->lsock->write("Reset has been applied.");

					this->md5Engine->reset();

					std::cout << "Reset has been trigered." << std::endl;

				}
				else if(strcmp(read,"status") == 0)
				{

					std::cout << "Writing status :[" << this->md5Engine->getStatus() << "]"<< std::endl;

					this->lsock->write(this->md5Engine->getStatus().c_str());


				}
				else if(strcmp(read,"goraw") == 0)
				{

					this->lsock->isRaw = true;

					this->lsock->write("Raw command has been trigered.");

					std::cout << "Raw command has been trigered." << std::endl;

				}
				else if(strcmp(read,"getmatch") == 0)
				{

					this->lsock->write(this->md5Engine->getMatch());

				}
				else if(strcmp(read,"isloggedin") == 0)
				{

					this->lsock->write("yes");

				}

			}

		}
		else
			this->lsock->connect();


	}

}
