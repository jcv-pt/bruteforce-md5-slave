## Brute Force - MD5 : Raspberry PI Cluster

[![License](https://img.shields.io/packagist/l/cakephp/app.svg?style=flat-square)](https://packagist.org/packages/cakephp/app)

Brute Force - MD5 : Raspberry PI Cluster

**APP:  Slave App**

This application is part of the project "Brute Force - MD5 in Raspberry PI Cluster", a brute force routine using MD5 dictionary developed for pure academic purposes.

Project is available to be consulted in https://www.joao-vieira.pt/portfolio/application-development/item/25-brute-force-md5-in-raspberry-pi-cluster

## Installation

1. Git clone the repo

2. Install the following:

   1. ```
      sudo mysql_secure_installation
      sudo apt-get install libmysqlcppconn-dev
      ```

3. Run `mysql_create_table.sql` file against your MYSQL server

4. Updated the `mysqlHandler.cpp` with database credentials

5. Compile with GCC

## Version

Alpha; This application was designed as part of an academic project to serve a demo purpose.

## Configuration

Read and edit `mysqlHandler.cpp` and setup the database host & credentials

## License

Distributed under the MIT License. See `LICENSE` for more information.

## Contacts

Developer - [João Vieira](https://www.joao-vieira.pt)

Project Link: https://www.joao-vieira.pt/portfolio/application-development/item/25-brute-force-md5-in-raspberry-pi-cluster
