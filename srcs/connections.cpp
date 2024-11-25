# include "../includes/SimpleSocket.hpp"
# include "../includes/ServerConfig.hpp"
# include "../includes/ListeningSocket.hpp"

bool terminate_sig = false;

void	errorHandling(int epoll_fd, std::map<int, SimpleSocket> socketsMap, std::map<int, ListeningSocket> listeningMap)
{
	close(epoll_fd);
	for (it_socmap it = socketsMap.begin(); it != socketsMap.end(); it++)
		it->second.clearData();
	for (it_lstsocmap it = listeningMap.begin(); it != listeningMap.end(); it++)
		close(it->first);
	throw SysError();
}

void	handleSignal(int sig)
{
	(void)sig; //what is this
	std::cout << "\nEXIT SERVER" << '\n';
	terminate_sig = true;
}

void	connectServer(std::vector<ServerConfig> &server)
{
	std::cout << "START" << '\n';
	std::signal(SIGINT, handleSignal);
	try
	{
		//std::map<int, std::string> listeningSockets;
		std::map<int, ListeningSocket> listeningMap;
		std::map<int, SimpleSocket> socketsMap;
		//EL WHILE GENERAL DE SERVERS DEBE SER AQUI server[i] no server[0]
		for (it_servec it = server.begin(); it != server.end(); it++)
		{
			SimpleSocket socket;
			std::string serverHost = (*it).getHost();
			std::vector<std::string> serverPorts = (*it).getPort();
			socket.setServer(&(*it));
			for (it_strvec it = serverPorts.begin(); it != serverPorts.end(); it++)
			{
				socket.setSocket(*it, serverHost); //tmp socket
				socketsMap[socket.getServerSocket()] = socket; //save socket
			}
		}
		/*DSADASDASDASDFDADSFADSFADSFDSFASFDSFDSFDSFDSDFSDFSD:v ñldfkblkdf,blñdf,bldf,bçñdgf ,
		ñvf ,ñ
		lblfd
		CHECK PARA INVESTIGAR*/
		t_epolle events[MAX_CONNECTIONS];
		int event_count;
		int epoll_fd = epoll_create(MAX_CONNECTIONS); // PARA INVESTIGAR, este int almacena la info de tantos epolls(fd) como definidos como parametro en epoll_create
		if (epoll_fd < 0)
			throw SysError();
		
		for (it_socmap it = socketsMap.begin(); it != socketsMap.end(); it++)
		{
			t_epolle event;
			event.events = EPOLLIN;
			event.data.fd = it->second.getServerSocket();
			std::cout << "fd: " << it->second.getServerSocket() << '\n';
			/*epoll_ctl --> epoll_fd (direccion dnd estan la info de los epolls, EPOLL_CTL_ADD (para añadir el fd de la conexion abierta, es decir la conexion abierta por el puerto
			a la direccion de los epolls, y event es la estructura donde ha definido EPOLLIN, es decir, que solo sea de lo que escriba el cliente "lectura"))*/
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
			{
				std::cout << "Error on fd: " << event.data.fd << '\n';
				errorHandling(epoll_fd, socketsMap, listeningMap);
			}
		}
		while (!terminate_sig)
		{
			std::cout << "IN SERVER" << '\n';

			std::cout << "BEFORE EPOLL" << '\n';
			// ultimo parametro es TIMEOUT lo podemos usar (-1 = infinite)
			event_count = epoll_wait(epoll_fd, events, MAX_CONNECTIONS, -1); //esto crea un bucle infinito hasta que pasa algo (mediante browser)
			if (terminate_sig)
				break;
			if (event_count == -1)
				errorHandling(epoll_fd, socketsMap, listeningMap);
			std::cout << "EPOLL FOUND! count: " << event_count << '\n';

			for (int i = 0; i < event_count; i++)
			{
				std::cout << events[i].data.fd << '\n';
				if (socketsMap.find(events[i].data.fd) != socketsMap.end())
				{
					t_epolle event;
					event.events = EPOLLIN | EPOLLET;
					event.data.fd = socketsMap[events[i].data.fd].acceptConnection();
					listeningMap[event.data.fd].setBuffer("");
					listeningMap[event.data.fd].setServer(socketsMap[events[i].data.fd].getServer());
					int flags = fcntl(event.data.fd, F_GETFL, 0);
					flags |= O_NONBLOCK;
					if (fcntl(event.data.fd, F_SETFL, flags) < 0)
						errorHandling(epoll_fd, socketsMap, listeningMap);
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
						errorHandling(epoll_fd, socketsMap, listeningMap);
				}
				else //para llegar al else la tiene que aceptar en el if
				{	
					int clientFd = events[i].data.fd;
					int status = SimpleSocket::readPetition(clientFd, listeningMap[clientFd].getBuffer(), *(listeningMap[clientFd].getServer()));
					if (status == 1) // ERROR
					{
						listeningMap.erase(clientFd);
						if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientFd, 0))
							errorHandling(epoll_fd, socketsMap, listeningMap);
						close(clientFd);
						errorHandling(epoll_fd, socketsMap, listeningMap);
					}
					else if (status == 2) // CLOSED CONNECTION
					{
						listeningMap.erase(clientFd);
						if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientFd, 0))
							errorHandling(epoll_fd, socketsMap, listeningMap);
						close(clientFd);
					}
				}
			}
		}
		close(epoll_fd);
		for (it_socmap it = socketsMap.begin(); it != socketsMap.end(); it++)
			it->second.clearData();
		for (it_lstsocmap it = listeningMap.begin(); it != listeningMap.end(); it++)
			close(it->first);
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}