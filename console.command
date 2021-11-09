g++ -Wall -ansi -pthread -std=c++11 main.c -o srv.out
g++ -Wall -ansi -I/usr/include/cppconn -o testapp mysqlHandler.cpp -L/usr/lib -lmysqlcppconn

g++ -Wall -ansi -I/usr/include/cppconn -pthread -std=c++11 main.c -o srv.out -L/usr/lib -lmysqlcppconn
