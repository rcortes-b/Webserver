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
	if (sig == SIGINT)
		terminate_sig = true;
}

void	connectServer(std::vector<ServerConfig> &server)
{
	std::signal(SIGINT, handleSignal);
	try
	{
		std::map<int, ListeningSocket> listeningMap;
		std::map<int, SimpleSocket> socketsMap;
		for (it_servec it = server.begin(); it != server.end(); it++)
		{
			SimpleSocket socket;
			std::string serverHost = (*it).getHost();
			std::vector<std::string> serverPorts = (*it).getPort();
			socket.setServer(&(*it));
			for (it_strvec it = serverPorts.begin(); it != serverPorts.end(); it++)
			{
				if (socket.setSocket(*it, serverHost))
				{
					for (it_socmap it = socketsMap.begin(); it != socketsMap.end(); it++)
						it->second.clearData();

					throw NoMsgError();
				}
				socketsMap[socket.getServerSocket()] = socket;
			}
		}
	
		t_epolle events[MAX_CONNECTIONS];
		int event_count;
		int epoll_fd = epoll_create(MAX_CONNECTIONS);
		if (epoll_fd < 0)
			throw SysError();
		
		for (it_socmap it = socketsMap.begin(); it != socketsMap.end(); it++)
		{
			t_epolle event;
			event.events = EPOLLIN | EPOLLOUT;
			event.data.fd = it->second.getServerSocket();
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
				errorHandling(epoll_fd, socketsMap, listeningMap);
		}
		while (!terminate_sig)
		{
			event_count = epoll_wait(epoll_fd, events, MAX_CONNECTIONS, -1);
			if (terminate_sig)
				break;
			if (event_count == -1)
				errorHandling(epoll_fd, socketsMap, listeningMap);
			for (int i = 0; i < event_count; i++)
			{
				if (socketsMap.find(events[i].data.fd) != socketsMap.end())
				{
					t_epolle event;
					event.events = EPOLLIN | EPOLLOUT;
					event.data.fd = socketsMap[events[i].data.fd].acceptConnection();
					listeningMap[event.data.fd].setBufferStr("");
					listeningMap[event.data.fd].setServer(socketsMap[events[i].data.fd].getServer());
					int flags = fcntl(event.data.fd, F_GETFL, 0);
					flags |= O_NONBLOCK;
					if (fcntl(event.data.fd, F_SETFL, flags) < 0)
						errorHandling(epoll_fd, socketsMap, listeningMap);
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
						errorHandling(epoll_fd, socketsMap, listeningMap);
				}
				else if (events[i].events & EPOLLIN)
				{
					int clientFd = events[i].data.fd;
					int status = listeningMap[clientFd].readPetition(clientFd);
					if (status == 1)
					{
						listeningMap.erase(clientFd);
						if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientFd, 0))
							errorHandling(epoll_fd, socketsMap, listeningMap);
						close(clientFd);
						errorHandling(epoll_fd, socketsMap, listeningMap);
					}
					else if (status == 2)
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