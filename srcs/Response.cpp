#include "Response.hpp"

Response::Response() :
_locBlocSelect(false), _isDir(false), _autoindex(false),
_closeConnection(false), _isResFormPage(false)	{}

Response::Response(const Request &req, const Server &s, char **envp) :
_serv(s), _req(req), _locBlocSelect(false),
_isDir(false), _autoindex(false), _closeConnection(false),
_isResFormPage(false), _envp(envp), _defaultPage(_req, _serv)	{}

<<<<<<< HEAD
Response::Response(Response const &src) { *this = src; }
=======
Response::Response() {}

Response::Response(Request req, std::vector<Server> vctServ, std::map<int, int> clientServer, char **envp) :
					_req(req), _vctServ(vctServ), _clientServer(clientServer), _locBlocSelect(false),
					_isDir(false), _autoindex(false), _closeConnection(false), _envp(envp){
}

Response::Response(Response const &src) {
	*this = src;
}
>>>>>>> main

Response::~Response() {}

Response	&Response::operator=(Response const &rhs) {
	if (this != &rhs)
	{
		this->_serv = rhs._serv;
		this->_req = rhs._req;
		this->_path = rhs._path;
		this->_errPath = rhs._errPath;
		this->_httpRep = rhs._httpRep;
		this->_statusCode = rhs._statusCode;
		this->_locBlocSelect = rhs._locBlocSelect;
		this->_locBloc = rhs._locBloc;
		this->_isDir = rhs._isDir;
		this->_autoindex = rhs._autoindex;
		this->_closeConnection = rhs._closeConnection;
		this->_envp = rhs._envp;
		this->_defaultPage = rhs._defaultPage;
	}
	return *this;
}

//	GETTER
Server		Response::getServ() const {
	return this->_serv;
}

bool		Response::getlocBlocSelect() const {
	return this->_locBlocSelect;
}

Location	Response::getLocBloc() const {
	return this->_locBloc;
}

bool		Response::getCloseConnection() const {
	return this->_closeConnection;
}

std::string	Response::rightRoot() {
	std::string	root;

	if (this->_locBlocSelect and this->_locBloc.getRootSet())
		root = this->_locBloc.getRoot();
	else
		root = this->_serv.getRoot();

	return root;
}

std::vector<std::string>	Response::rightIndex() {
	std::vector<std::string>	index;

	if (this->_locBloc.getIndexSet())
		index = this->_locBloc.getIndex();
	else
		index = this->_serv.getIndex();

	return index;
}

/*	Reconstitue le chemin complet du fichier avec la directive root + 
	l'url qu'a entre l'utilisateur. Une fois le chemin reconstitue :
	- Si c'est un fichier, on l'ajoute a notre vct de path.
	- Si c'est un dossier, on ajoute un a un a notre vct de path, le path
	des fichiers indiques dans la directive index du fichier de conf
	et on regardera plus tard si ces fichiers existent ou pas. On regarde aussi si l'
	autoindex est "on" pour pouvoir l'afficher en cas de besoin.
	- Sinon, on met notre bool a true (donc erreur) */
bool	Response::rightPathLocation() {
	struct stat					fileOrDir;
	std::string					root = this->rightRoot();
	std::string					newPath;
	std::vector<std::string>	index = this->rightIndex();

	memset(&fileOrDir, 0, sizeof(fileOrDir));
	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath().erase(0, this->_locBloc.getPath().size());
	root += newPath;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
		this->_path.push_back(root);
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
		if (this->_locBloc.getAutoindexSet())
			this->_autoindex = this->_locBloc.getAutoindex();
		else if (this->_serv.getAutoindexSet())
			this->_autoindex = this->_serv.getAutoindex();
	}
	else
		return true;
	return false;
}

/*	Pareil que au dessus mais si un aucun bloc de Location est selectionne */
bool	Response::rightPathServer() {
	struct stat					fileOrDir;
	//std::string					root = this->rightRoot();
	std::string					root = this->_serv.getRoot();
	std::string					newPath;
	std::vector<std::string>	index;

	memset(&fileOrDir, 0, sizeof(fileOrDir));
	if (root[0] == '/')
		root.erase(0, 1);
	newPath = this->_req.getPath();
	root += newPath;
	stat(root.c_str(), &fileOrDir);
	if (S_ISREG(fileOrDir.st_mode))
		this->_path.push_back(root);
	else if (S_ISDIR(fileOrDir.st_mode))
	{
		this->_isDir = true;
		if (root[root.size() - 1] != '/')
			root += "/";
		index = this->_serv.getIndex();
		for (size_t i = 0; i < index.size(); i++)
			this->_path.push_back(index[i].insert(0, root));
		if (this->_serv.getAutoindexSet())
			this->_autoindex = this->_serv.getAutoindex();
	}
	else
		return true;
	return false;
}


bool	Response::rightPath() {
	bool	err;

	if (this->_locBlocSelect)
<<<<<<< HEAD
	{
		std::cout << "locBlockSelect: true" << std::endl;
		this->rightPathLocation(&err);
	}
=======
		err = this->rightPathLocation();
>>>>>>> main
	else
		err = this->rightPathServer();

	return err;
}

/*	On va regarder parmis tous les paths selectionnes si au moins un existe, si c'est le cas,
	on renvoie le bon path sinon, on met notre bool err a true. */
std::string	Response::testAllPaths(bool *err) {
	size_t		i = 0;
	std::string	rightPath;
	struct stat	file;

	while (i < this->_path.size())
	{
		stat(this->_path[i].c_str(), &file);

		if (S_ISREG(file.st_mode))
		{
			std::ifstream tmp(this->_path[i].c_str(), std::ios::in | std::ios::binary);

			if (!tmp)
			{
				this->_isDir = true;
				*err = true;
			}
			else
			{
				rightPath = this->_path[i];
				*err = false;
				this->_statusCode = 200;
				break;
			}
		}
		i++;
		if (i == this->_path.size())
		{
			*err = true;
			break ;
		}
	}
	return rightPath;
}

/*	On regarde par rapport au code erreur si une page a ete set pour ce code erreur
	dans notre fichier de configuration en reconstituant le path */
std::string	Response::rightPathErr(bool &pageFind) {
	std::string									root = rightRoot();
	std::map<int, std::string>					mapErr;
	std::map<int, std::string>::const_iterator	it;
	std::string									rightPath;

	if (this->_locBlocSelect and this->_locBloc.getErrorPageSet())
	{
		mapErr = this->_locBloc.getErrorPage();
		it = mapErr.find(this->_statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = this->_locBloc.getRoot();
			if (root[0] == '/')
				root.erase(0, 1);
			if (root[root.size() - 1] != '/')
				root += "/";
			root += rightPath;
			rightPath = root;
		}
	}
	if (!pageFind and it != mapErr.end())
	{
		mapErr = this->_serv.getErrorPage();
		it = mapErr.find(this->_statusCode);
		if (it != mapErr.end())
		{
			pageFind = true;
			rightPath = it->second;
			root = this->_serv.getRoot();
			if (root[0] == '/')
				root.erase(0, 1);
			if (root[root.size() - 1] != '/')
				root += "/";
			root += rightPath;
			rightPath = root;
		}
	}
	return rightPath;
}

<<<<<<< HEAD
=======
std::string	Response::createDefaultErrorPage() {
	std::ofstream file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

	file << "<!DOCTYPE html>" << std::endl;
	file << "<html lang=\"en\">" << std::endl;
	file << "<head>" << std::endl;
	file << "	<meta charset=\"UTF-8\">" << std::endl;
	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	file << "	<title>Webserv " + ft_itos(this->_statusCode) + "</title>" << std::endl;
	file << "</head>" << std::endl;
	file << "<body>" << std::endl;
	file << "	<h1>Default page Error " + ft_itos(this->_statusCode) + " :(</h1>" << std::endl;
	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();

	return "/tmp/tmpFile.html";
}


/*	Cree un lien pour chaque dossier / fichier*/
void	Response::fileAndDir(std::ofstream &file, bool getDir, std::string path) {
	DIR				*dir;
	struct dirent	*entry;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if ((getDir and entry->d_type == DT_DIR) or (!getDir and entry->d_type != DT_DIR))
			{
				if (strlen(entry->d_name) != 1 or entry->d_name[0] != '.')
				{
					file << "<a href=\"http://" + this->_serv.getHost() + ":" + ft_itos(this->_serv.getPort()) +
					this->_req.getPath();
					if (this->_req.getPath()[this->_req.getPath().size() - 1] != '/')
						file << "/";
					file << entry->d_name << "\">" << entry->d_name;
					if (entry->d_type == DT_DIR)
						file << "/";
					file << "</a><br>" << std::endl;
				}
			}
		}
		closedir(dir);
	} 
	else
		perror("Impossible d'ouvrir le répertoire");
}

/*	Affiche l'autoindex, deja les dossiers puis les ficheirs (comme nginx) */
std::string	Response::createAutoindexPage() {
	std::string		path = this->_path[0];
	size_t			pos = path.find_last_of('/');
	std::ofstream	file("/tmp/tmpFile.html", std::ios::out | std::ios::trunc);

	path.erase(pos, path.size() - pos);
	path += "/";

	file << "<!DOCTYPE html>" << std::endl;
	file << "<html lang=\"en\">" << std::endl;
	file << "<head>" << std::endl;
	file << "	<meta charset=\"UTF-8\">" << std::endl;
	file << "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">" << std::endl;
	file << "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	file << "	<title>Webserv Autoindex</title>" << std::endl;
	file << "</head>" << std::endl;
	file << "<body>" << std::endl;
	file << "	<h1>Index of " + this->_req.getPath() + "</h1>" << std::endl;

	this->fileAndDir(file, true, path);
	this->fileAndDir(file, false, path);

	file << "</body>" << std::endl;
	file << "</html>" << std::endl;
	file.close();

	return "/tmp/tmpFile.html";
}

>>>>>>> main
/*	On va appeler les differentes fonctions qui check les differents paths.
	2 cas sont possibles :
	- Aucun fichier n'existe, on va definir le code erreur
	et appeler une fct qui va regarder si une page d'erreur a ete set dans le fichier de conf par rapport a
	ce code erreur.
	- Le ficheir existe et la pas de soucis :)

	Ensuite, si le path indique est un dossier, on cree la page autoindex.
	sinon, si la page demandee n'existe pas && que la page n'a pas ete set dans le fichier de conf,
	on la cree
	*/
void	Response::sendData() {
	std::string	res;
	std::string	path;
	bool		err;

	if (!(err = this->rightPath()))
		path = this->testAllPaths(&err);
	if (err)
	{
		bool	pageFind = false;

<<<<<<< HEAD
		// ce if est temporaire
		if (this->_isResFormPage)
		{
			path = this->_defaultPage.createResFormPage();
			this->_statusCode = 200;
			pageFind = true;
		}
=======
		if (this->_isDir)
			this->_statusCode = 403;
>>>>>>> main
		else
			this->_statusCode = 404;

		path = this->rightPathErr(pageFind);

		std::ifstream tmp(path.c_str(), std::ios::in | std::ios::binary);

		if (this->_autoindex)
			path = this->_defaultPage.createAutoindexPage(this->_path);
		else if (!tmp or !pageFind)
			path = this->_defaultPage.createDefaultErrorPage(this->_statusCode);
		else
			tmp.close();
	}

	this->sendHeader(path);
}

void	Response::sendHeader(std::string path)
{
	Header			header(this->_req, path, &this->_statusCode, this->_serv, this);
	std::string		res;

	if ((this->_locBlocSelect and this->_locBloc.getHttpRedirSet()) or
		this->_serv.getHttpRedirSet())
	{
		res = "HTTP/1.1 301 Moved Permanently\nLocation: ";
		if (this->_locBlocSelect and this->_locBloc.getHttpRedirSet())
			res += this->_locBloc.getHttpRedir();
		else
			res += this->_serv.getHttpRedir();
		res += "\n\n";
		write(this->_req.getFd(), res.c_str(), res.size());
		this->_closeConnection = true;
		return ;
	}
	res = header.getHeader();
	// std::cout << res << std::endl;
	if (this->_statusCode == 406)
	{
		path = this->_defaultPage.createDefaultErrorPage(this->_statusCode);
		Header	headerBis(this->_req, path, &this->_statusCode, this->_serv, this);
		res = header.getHeader();
	}
	write(this->_req.getFd(), res.c_str(), res.size());
	this->sendPage(path);
}

void	Response::sendPage(std::string path) {
	std::ifstream	file(path.c_str(), std::ios::in | std::ios::binary);
	std::string		page;

	if (file)
	{
		std::ostringstream ss;
		ss << file.rdbuf();
		page = ss.str();
	}
	write(this->_req.getFd(), page.c_str(), page.size());
	file.close();

	if (this->_req.getConnection() == "close")
		this->_closeConnection = true;
}

<<<<<<< HEAD
=======
void	Response::selectServerBlock() {
	std::vector<Server>			tmp;
	std::string					host;
	std::vector<Server>			conf = this->_vctServ;
	bool						err = false;
	int							fd = this->_clientServer[this->_req.getFd()];

	host = conf[fd].getHost();

	for (size_t i = 0; i < conf.size(); i++)
	{
		if (conf[i].getHost() == host and
				conf[i].getPort() == ft_stoi(this->_req.getPort(), &err))
			tmp.push_back(conf[i]);
	}
	if (tmp.size() == 1)
	{
		this->_serv = tmp[0];
		return ;
	}
	else if (tmp.size() > 1)
		std::cout << "Fct selectServerBlock err, plusieurs blocs preselectionnes" << std::endl;

	// Si ce msg apparait, plusieurs blocs ont etes pre-selectionnes
	// mais pas departages, il faut les departages avec server name.
	std::cout << "Fct selectServerBlock err" << std::endl;
	return ;
}

>>>>>>> main
void	Response::selectLocationBlock() {
	std::vector<Location>	locations = this->_serv.getVctLocation();
	std::string 			strBlocLoc;
	Location				tmp;
	std::string				req = this->_req.getPath();
	size_t j;

	for (size_t i = 0; i < locations.size(); i++)
	{
		strBlocLoc = locations[i].getPath();
		j = 0;
		if (!strncmp(strBlocLoc.c_str(), this->_req.getPath().c_str(), strBlocLoc.length()))
		{
			j += strBlocLoc.length();
			if (j && strBlocLoc.size() > tmp.getPath().size())
			{
				this->_locBlocSelect = true;
				tmp = locations[i];
			}
		}
	}
	if (!this->_locBlocSelect)
	{
		for (size_t i = 0; i < vctLoc.size(); i++)
		{
			if (vctLoc[i].getPath() == "/")
			{
				this->_locBlocSelect = true;
				tmp = vctLoc[i];
				break ;
			}
		}
	}
	if (this->_locBlocSelect)
		this->_locBloc = tmp;
}

std::ostream	&operator<<(std::ostream &out, const Response &res)
{
	(void)res;
	out << "///////////////		REPONSE		////////////////\n";
	
	return (out);
}
