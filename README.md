# OS-Project-2-
This project is part of Operating Systems Course. It is build in Linux environment. In this	project	we implemented a	multi-threaded	server that processes	keyword search queries from	clients.We implement both the	server program and the client program.	 Multiple	 clients can request service from the	 server	 concurrently.The	service	given	will	be	a	keyword	search	service.	Upon	receiving	a	keyword	from a	client,	the	server	will	search the	keyword	in	an	input	text	file	and	will	send	back	the	line numbers of	the	lines	having the keyword at	least once (i.e.,	 line	 numbers	 of	matching	 lines)

	The server program will	take	the parameters :		<shm_name>	<inputfilename>	<sem_name>
<shm_name> is the name of the shared memory segment that will be created by the server.
 <inputfilename> is the name of the input file.
<sem_name>  is a prefix for naming your semaphores
  
 The client program will	take	the parameters :  	<shm_name>	<keyword>	<sem_name>
 <shm_name> is the name of the shared memory segment that will be created by the server.
 <keyword> is the keyword that will be searched. 
 <sem_name>  is a prefix for naming your semaphores

