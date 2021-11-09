/*********************   CLASS DECLARATION    *********************/

class md5Generator {

	private:
		char * md5ToMatch;
		char * md5Temp;
		int md5Sequence;
		bool matchFound;
		bool waittingSequence;

		bool sequenceIsSet;
		bool md5IsSet;

		bool useMysql;

		bool running;
		
		char * dictionary;

		int * matrix;

		int sizeOfDict;

		int iterationsPerSec;
		
		long double curPercentage;
		long double totalIterations;

		int timeLeft;

		mysqlHandler * dbHandler;

		void genMd5(void);

	public:
		md5Generator(void); //constructor
		~md5Generator(void); //destructor

		void process();
		void setMd5(char * md5);
		void setSequence(int sequence);
		void initMatrix(void);
		char * getMatch(void);
		int getIterationsPerSec(void);
		void shutdown(void);

		bool isRunning(void);
		bool isMd5Set(void);
		bool isSequenceSet(void);

		std::string getStatus(void);
	
		void reset(void);

		void useSql(bool state);

		bool truncate(void);

};


/*********************   FUNCTIONS    *********************/


/** Constructor */
md5Generator::md5Generator(void) {

	//Init vars

	this->matchFound = false;
	this->waittingSequence = true;

	this->sequenceIsSet = false;
	this->md5IsSet = false;

	this->running = false;

	this->useMysql = true;

	this->iterationsPerSec = 0;
	this->curPercentage = 0;
	this->totalIterations = 0;
	this->timeLeft = 0;
	
	//Init dictionary

	this->dictionary = (char*)"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`{|}~!\"#$%&'()*+,-./:;<=>?@ ";

	this->sizeOfDict = strlen(this->dictionary);

	//Init Mysql

	this->dbHandler = new mysqlHandler();
}

/** Destructor */
md5Generator::~md5Generator(void) {

}

void md5Generator::genMd5(void){

	int i;

	//Increment sequence

	for(i =this->md5Sequence-1; i >= 0; i--)
	{

		this->matrix[i]++;

		if(i==0 && this->matrix[i]>this->sizeOfDict-1)
		{
			this->waittingSequence = true;
			break;			
		}

		if(this->matrix[i]<this->sizeOfDict)
		{
			break;
		}

		if(this->matrix[i]>=this->sizeOfDict)
		{
			this->matrix[i]=0;
		}
		


	}

	//Build temp string
	
	for(i =0; i < this->md5Sequence; i++)
	{
		this->md5Temp[i] = this->dictionary[this->matrix[i]];		
		
	}

}

void md5Generator::process(){

	while(this->running==false)
	{

		if(this->waittingSequence==true || !this->isMd5Set() || !this->isSequenceSet())
			continue;

		int iterationCounter = 0;
		int i;
		long double totalCombinations = pow(this->sizeOfDict,this->md5Sequence);

		std::string insertQueryStatement = "";
		int insertQueryCount = 0;

		auto t1 = std::chrono::high_resolution_clock::now();

		while(this->waittingSequence == false && this->matchFound==false && this->running==false)
		{

			std::string testMd5 = md5(this->md5Temp);

			//Compare to see if it matches

			if(strcmp(this->md5ToMatch,testMd5.c_str()) == 0)
			{
				std::cout << std::endl;
				std::cout << termcolor::green << " ---------------------- !!! SUCCESS !!! ---------------------- " << std::endl << termcolor::white <<" Match found! : '" << this->md5Temp << "' - " << md5(this->md5Temp) << std::endl;
				this->matchFound = true;
			}

			//Store in the database

			if(this->useMysql)
			{

				std::string queryStatement = "(null,':text',':hash',':sequence',':iteration'), ";
				
				this->dbHandler->setField(queryStatement, ":hash", testMd5.c_str());

				this->dbHandler->setField(queryStatement, ":sequence", this->md5Sequence);
				this->dbHandler->setField(queryStatement, ":iteration", totalIterations+1);

				//Compile seq

				std::string md5Tmp = "";

				for(i =0; i < this->md5Sequence; i++)
				{
				
					//Escape

					std::string tmpseq = "";

					tmpseq.push_back(this->dictionary[this->matrix[i]]);

					this->dbHandler->setField(tmpseq, "\\","\\\\");
					this->dbHandler->setField(tmpseq, "'","\\'");
					this->dbHandler->setField(tmpseq, "`","\\`");

					md5Tmp += tmpseq;
				}

				this->dbHandler->setField(queryStatement, ":text", md5Tmp.c_str());

				insertQueryCount++;

				insertQueryStatement += queryStatement;

				//Run query at every 50 rows (supose to be faster)

				if(insertQueryCount>=50 || this->matchFound==true || this->waittingSequence==true)
				{

					std::string query = "insert into hash_list values ";

					query += insertQueryStatement.substr(0, insertQueryStatement.size()-2);

					query += ";";

					this->dbHandler->query((char*)query.c_str());

					query = "";

					insertQueryStatement = "";

					insertQueryCount=0;

				}

			}

			//Calculate iterations per sec

			iterationCounter++;
			this->totalIterations++;

			auto t2 = std::chrono::high_resolution_clock::now();

			if(std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()>=1000)
			{

				this->curPercentage = (this->totalIterations*100/totalCombinations);
				this->iterationsPerSec = iterationCounter;
				this->timeLeft = ((totalCombinations-this->totalIterations)/this->iterationsPerSec)/60;

				if(this->timeLeft>100)
					this->timeLeft = 100;
				
				t1 = std::chrono::high_resolution_clock::now();
				iterationCounter = 0;

				std::cout << "MD5Engine Running: @ " <<  this->iterationsPerSec << " iterations p/sec, " << std::fixed << std::setprecision(2) << this->curPercentage << " %, ~"<< this->timeLeft << " minutes left...\r";
				std::cout.flush();

			} 
	
			//Generate new iterarion
			
			if(!this->matchFound)
				this->genMd5();	
	
		}


	}

}

void md5Generator::setMd5(char * md5){

	this->md5ToMatch = md5;
	this->md5Temp = (char*) malloc( sizeof(char) * strlen(md5)  );

	this->md5IsSet = true;

	std::cout << "New Md5 has been set: '" << this->md5ToMatch << "'" << std::endl;

	//Check if records exists on the db...

	std::cout << "Looking for hash '" << this->md5ToMatch << "' in database..." << std::endl;

	sql::ResultSet * res;

	std::string queryStatement = "select * from hash_list where hash=':hash'";

	dbHandler->setField(queryStatement, ":hash", this->md5ToMatch);

	res = dbHandler->query((char*)queryStatement.c_str());

	if(res->rowsCount()>=1)
	{

		//Match Found!

		while (res->next()) {

			std::cout << std::endl;
			std::cout << termcolor::green << " ---------------------- !!! SUCCESS !!! ---------------------- " << std::endl << termcolor::white <<" Match found in database! : '" <<  res->getString("text") << "' - " <<  res->getString("hash") << std::endl;
		
			this->matchFound = true;
			this->md5Temp = (char*)res->getString("text").c_str();

			break;

		}
		

	}
	else
		std::cout << "No matches found from database." << std::endl;

}

bool md5Generator::truncate(void){

	dbHandler->query((char*)"truncate hash_list;");

	return true;

}

void md5Generator::setSequence(int sequence){

	if(this->matchFound==true)
		return;

	this->md5Sequence = sequence;

	this->initMatrix();

	this->sequenceIsSet = true;

	this->waittingSequence = false;

}

void md5Generator::reset(void){

	this->waittingSequence = true;

	this->sequenceIsSet = false;

	this->matchFound = false;

}


void md5Generator::initMatrix(void){
	
	int i,i2;

	//Allocate memory space for the matrix

	this->matrix = (int*) malloc( sizeof(int) * this->md5Sequence  );

	//Check MYSQL for the last proccessed iteration from this sequence...

	sql::ResultSet * res;

	std::string queryStatement = "select * from hash_list where sequence=':sequence' order by id desc limit 1;";

	std::string sequence = std::to_string(this->md5Sequence);

	dbHandler->setField(queryStatement, ":sequence", sequence);

	res = dbHandler->query((char*)queryStatement.c_str());

	if(res->rowsCount()>=1)
	{

		//Initialize matrix from last iteration

		while (res->next()) {

			char * tmp = (char*)res->getString("text")->c_str();

			for(i =0; i < this->md5Sequence; i++)
			{
				for(i2=0;i2<this->sizeOfDict;i2++)
				{
					if(tmp[i]==this->dictionary[i2])
					{
						this->matrix[i] = i2;
						break;
					}
				}

				this->md5Temp[i] = this->dictionary[this->matrix[i]];	
			}

			this->totalIterations = atoi((char*)res->getString("iteration")->c_str());


		}


	}
	else
	{
		//Init matrix as empty

		for(i =0; i < this->md5Sequence; i++)
		{
			this->matrix[i] = 0;
			this->md5Temp[i] = this->dictionary[this->matrix[i]];
		}

	}


}

char * md5Generator::getMatch(){

	return this->md5Temp;

}

int md5Generator::getIterationsPerSec(){

	return this->iterationsPerSec;

}

void md5Generator::shutdown(){

	this->running = true;

}

bool md5Generator::isRunning(){

	if(this->waittingSequence == true || this->matchFound==true)
		return false;

	return true;

}

bool md5Generator::isMd5Set(){

	return this->md5IsSet;

}

bool md5Generator::isSequenceSet(){

	return this->sequenceIsSet;

}

void md5Generator::useSql(bool state){

	this->useMysql = state;

}

std::string md5Generator::getStatus(void){

	std::string str = "";

	if(this->isRunning())
	{
		str += "running|";
	}
	else if(this->matchFound==true)
	{
		str += "matchfound|";
	}
	else if(this->waittingSequence == true)
	{
		str += "wait4sec|";
	}
	else
	{
		str += "idle|";
	}

 	str +=  std::to_string(this->iterationsPerSec);
	str +=  "|";
	str +=  std::to_string(this->curPercentage);
	str +=  "|";
	str +=  std::to_string(this->timeLeft);

	return str;

}

