/*********************   CLASS DECLARATION    *********************/

class Auth {

	private:
		const char * user;
		const char * password;
		ListenSocket * lsock;
		bool isAuth;

	public:
		Auth(ListenSocket * lsock); //constructor
		~Auth(void); //destructor

		bool authenticate();
		bool isAuthenticated();
		void disconnect();
		const char * getUser(void);

};


/*********************   FUNCTIONS    *********************/


/** Constructor */
Auth::Auth(ListenSocket * lsock) {

	this->user = "admin";
	this->password = "password";

	this->lsock = lsock;

	this->isAuth = false;

}

/** Destructor */
Auth::~Auth(void) {

}

bool Auth::authenticate(){

	//char * input;

	int attemps = 0;

	this->lsock->write("Authentication required, please login:\n");

	std::cout << "New connection from host " << this->lsock->getIp() << ", authenticating..." << std::endl;

	for(attemps=0;attemps<=2;attemps++)
	{

		char * username =  this->lsock->read();

		char * password = this->lsock->read();

		if(strcmp(this->user,username) == 0 && strlen(username)>=1)
		{
			this->user = username;

			if(strcmp(this->password,password)==0 && strlen(password)>=1)
			{

				this->lsock->write("Authentication successfull!");

				this->isAuth = true;
				this->lsock->isAuth = true;

				std::cout << "Client is logged on! "<< std::endl;
		
				return true;

			}
			else
			{
				std::cout << "Auth is wrong: ["<< password<< "][" << username << "]" << std::endl;
				this->lsock->write("Authentication failed,password is wrong try again!\n");
			}

		}
		else
		{
			std::cout << "Auth is wrong: ["<< password<< "][" << username << "]" << std::endl;
			this->lsock->write("Authentication failed,username is wrong try again!\n");
		}

	}

	std::cout << "Connection attemps exhausted, client was disconnected."<< std::endl;

	this->lsock->close();	

	return false;

}

const char * Auth::getUser(){

	return this->user;

}

bool Auth::isAuthenticated(){

	return this->isAuth;

}

void Auth::disconnect(){

	this->isAuth = false;
	this->lsock->isAuth = false;

	this->lsock->write("Closing connection...");
	std::cout << "Client closed the connection."<< std::endl;

	this->lsock->close();

}
