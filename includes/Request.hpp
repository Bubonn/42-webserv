#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <sys/socket.h>
# include <vector>
# include <map>

class Request {

	public:
	Request();
	Request(int fd);
	Request(Request const &src);
	~Request();

	Request	&operator=(Request const &rhs);

	int			getFd() const;
	bool		getErrRequest() const;
	bool		getcloseConnection() const;
	std::string	getMethod() const;
	std::string	getPath() const;
	std::string	getHttpVersion() const;
	std::string	getHost() const;
	std::string	getPort() const;
	std::string	getConnection() const;
	std::string	getAccept() const;

	bool		getConnectionSet() const;
	bool		getAcceptSet() const;

	private:
	int									_fd;
	bool								_errRequest;
	bool								_parsArgsGet;
	bool								_closeConnection;
	std::string							_method;
	std::string							_path;
	std::string							_httpVersion;
	std::string							_host;
	std::string							_port;
	std::string							_connection;
	bool								_connectionSet;
	std::string							_accept;
	bool								_acceptSet;
	std::map<std::string, std::string>	_argsGet;

	void	(Request::*functPtr[6])(std::vector<std::string>);

	int		parsRequest(int fd);

	void	setMethodVersionPath(std::vector<std::string> strSplit);
	void	setHostPort(std::vector<std::string> strSplit);
	void	setConnection(std::vector<std::string> strSplit);
	void	setAccept(std::vector<std::string> strSplit);


};

std::ostream &operator<<( std::ostream & o, Request const & rhs);

#endif