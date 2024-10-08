# include "../includes/SimpleSocket.hpp"

bool terminate_sig = false;

static void	errorHandling(int epoll_fd, std::vector<SimpleSocket> socketsVec)
{
	close(epoll_fd);
	for (it_socvec it = socketsVec.begin(); it != socketsVec.end(); it++)
		(*it).clearData();
	SysError();
}

static void	handleSignal(int sig)
{
	std::cout << "\nEXIT SERVER" << '\n';
	terminate_sig = true;
}

void	connectServer(void)
{
	std::cout << "START" << '\n';
	std::signal(SIGINT, handleSignal);
	//placeholder parametro
	std::vector<std::string> test_ports;
	test_ports.push_back("8080");
	test_ports.push_back("1024");
	// De momento no uso NADA del archivo config lo preparo todo con variables y despues lo juntamos
	// QUIERO MEJORAR ESO A POLL O EPOLL EL SELECT ES UN CAGAO
	// PRIMER DE TOT FER BUCLES A TOTES LES COSES QUE ES NECESSITI
	std::vector<SimpleSocket> socketsVec;
	SimpleSocket socket;
	for (it_strvec it = test_ports.begin(); it != test_ports.end(); it++)
	{
		socket.setSocket(*it);
		socketsVec.push_back(socket);
	}

	int	serverSocket;
	t_epolle events[MAX_CONNECTIONS];
	size_t event_count;
	int epoll_fd = epoll_create(MAX_CONNECTIONS);
	if (epoll_fd < 0)
		throw SysError();
	
	for (it_socvec it = socketsVec.begin(); it != socketsVec.end(); it++)
	{
		t_epolle event;
		event.events = EPOLLIN | EPOLLOUT;
		event.data.fd = (*it).getServerSocket();
		std::cout << (*it).getServerSocket() << '\n';
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
		{
			std::cout << "Error on fd: " << event.data.fd << '\n';
			errorHandling(epoll_fd, socketsVec);
		}
	}
	while (!terminate_sig)
	{
		std::cout << "IN SERVER" << '\n';
		std::cout << "BEFORE EPOLL" << '\n';
		// ultimo parametro es TIMEOUT lo podemos usar (-1 = infinite)
		event_count = epoll_wait(epoll_fd, events, MAX_CONNECTIONS, -1);
		if (terminate_sig)
			break;
		std::cout << "EPOLL FOUND! count: " << event_count << '\n';

		for (size_t i = 0; i < event_count; i++)
		{
			std::cout << events[i].data.fd << '\n';
			it_socvec it;
			for (it = socketsVec.begin(); it != socketsVec.end(); it++)
				if (events[i].data.fd == (*it).getServerSocket())
					break;
			if (it != socketsVec.end())
			{
				t_epolle event;
				event.events = EPOLLIN | EPOLLOUT;
				event.data.fd = (*it).acceptConnection();
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
					errorHandling(epoll_fd, socketsVec);
			}
			else
			{
				if (SimpleSocket::readPetition(events[i].data.fd))
					errorHandling(epoll_fd, socketsVec);
				if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, 0))
					errorHandling(epoll_fd, socketsVec);
			}
		}
	}
	close(epoll_fd);
	for (it_socvec it = socketsVec.begin(); it != socketsVec.end(); it++)
		(*it).clearData();
}