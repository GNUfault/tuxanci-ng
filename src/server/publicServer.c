
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#ifndef __WIN32
#    include <sys/socket.h>
#    include <sys/select.h>
#    include <arpa/inet.h>
#else
#    include <windows.h>
#    include <wininet.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "main.h"
#include "list.h"
#include "tux.h"
#include "idManager.h"
#include "myTimer.h"
#include "arena.h"
#include "item.h"
#include "shot.h"
#include "arenaFile.h"
#include "proto.h"
#include "modules.h"
#include "net_multiplayer.h"

#include "publicServer.h"
#include "serverConfigFile.h"
#include "highScore.h"
#include "log.h"

#include "dns.h"

static arena_t *arena;
static bool_t isSignalEnd;
static arenaFile_t *choice_arenaFile;

extern int errno;

#define	__PACKED__ __attribute__ ((__packed__))

void world_inc_round()
{
}

char *public_server_get_setting(char *env, char *param, char *default_val)
{
	return getParamElse(param, server_configFile_get_value(env, default_val));
}


#ifndef __WIN32__
void daemonize ()
{
	int i, lockfd;
  	char pid[16];
	char spid[64];

	int ipid = getpid ();

  	/* detach if asked */
  	if (ipid == 1)
		return;				/* already a daemon */

    	/* fork to guarantee we are not process group leader */
    	i = fork ();

    	if (i < 0)
      		exit (1);			/* fork error */
    	if (i > 0)
      		exit (0);			/* parent exits */

    	/* child (daemon) continues */
    	setsid ();				/* obtain a new process group */
	ipid = getpid ()+1;

	printf ("> started with pid -> %d\n", ipid);
    	/* fork again so we become process group leader 
     	 * and cannot regain a controlling tty 
     	 */
    	i = fork ();

    	if (i < 0)
      		exit (1);			/* fork error */
    	else if (i > 0)
		exit (0);			/* parent exits */

    	/* close all fds */
    	for (i = getdtablesize (); i >= 0; --i)
      		close (i);			/* close all descriptors */

    	/* close parent fds and send output to fds 0, 1 and 2 to bitbucket */
    	i = open ("/dev/null", O_RDWR);

    	if (i < 0)
      		exit (1);

    	dup (i);
    	dup (i);				/* handle standart I/O */

	sprintf (spid, "/tmp/tuxanci-server-%d.pid", ipid);

  	/* create local lock */
  	lockfd = open (spid, O_RDWR | O_CREAT, 0640);

  	if (lockfd < 0) {
    		perror ("lock: open");
    		exit (1);
  	}
	#ifndef __CYGWIN__
	/* lock the file */
	if (lockf (lockfd, F_TLOCK, 0) < 0) {
		perror ("lock: lockf");
		printf ("> tuxanci-server is already running.\n");
		exit (0);
	}
	#else
	/* lock the file */
	{
	struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_start = 0;
		lock.l_whence = SEEK_SET;
		lock.l_len = 0;
		
		if (fcntl (lockfd, F_SETLK, &lock) < 0) {
			printf ("> tuxanci-server is already running.\n");
			exit (0);
		}
	}
	#endif
	/* write to pid to lockfile */
	snprintf (pid, 16, "%d\n", getpid ());
	write (lockfd, pid, strlen (pid));

	/* restrict created files to 0750 */
	umask (027);
}
#endif

static int public_server_register()
{
#ifndef __WIN32
	int s;
#else
	SOCKET s;
#endif

	/* TODO: dodelat TCP makro */
	struct sockaddr_in server;
	char *master_server_ip;

	master_server_ip = gns_resolv(NET_MASTER_SERVER_DOMAIN);

	//printf("master_server_ip = %s\n", master_server_ip);

	if (master_server_ip == NULL)	// master server is down
	{
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(NET_MASTER_PORT);
	server.sin_addr.s_addr = inet_addr(master_server_ip);

	free(master_server_ip);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return 0;
	}

	/* Set to nonblocking socket mode */
#ifndef __WIN32__
	int oldFlag;

	oldFlag = fcntl(s, F_GETFL, 0);

	if (fcntl(s, F_SETFL, oldFlag | O_NONBLOCK) == -1) {
		return -1;
	}
#else
	unsigned long arg = 1;
	// Operation is  FIONBIO. Parameter is pointer on non-zero number.
	if (ioctlsocket(s, FIONBIO, &arg) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}
#endif

	if (connect(s, (struct sockaddr *) &server, sizeof(server)) == -1) {
#ifndef __WIN32__
		if (errno != EINPROGRESS)
			return -1;
#else
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			WSACleanup();
			return -1;
		}
#endif
	}

	struct timeval tv;

	fd_set myset;

	FD_ZERO(&myset);
	FD_SET(s, &myset);

	tv.tv_sec = 3;
	tv.tv_usec = 0;

	int ret = select(s + 1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return -1;

	if (ret == 0)
		return -1;

	FD_ZERO(&myset);
	FD_SET(s, &myset);

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	ret = select(s + 1, NULL, &myset, NULL, &tv);

	if (ret == -1)
		return -1;

	if (ret == 0)
		return -1;


	typedef struct {
		unsigned char cmd;
		unsigned port;
		unsigned ip;
	} __PACKED__ register_head;

	char *str = (char *) malloc(sizeof(register_head));

	register_head *head = (register_head *) str;

	head->cmd = 'p';
	head->port = atoi(public_server_get_setting("PORT4", "--port4", "6800"));
	head->ip = 0;				// TODO

	/* send request for server list */
	int r = send(s, str, 9, 0);

	free(str);

	if (r == -1) {
#ifndef __WIN32
		close(s);
#else
		closesocket(s);
#endif
		return -1;
	}
#ifndef __WIN32
	close(s);
#else
	closesocket(s);
#endif

	return 0;
}

static int public_server_initNetwork()
{
	char ip4[STR_IP_SIZE];
	char ip6[STR_IP_SIZE];
	char *p_ip4, *p_ip6;
	int port4;
	int port6;
	int ret;

	ret = -1;

	strcpy(ip4, public_server_get_setting("IP4", "--ip4", "none"));
	strcpy(ip6, public_server_get_setting("IP6", "--ip6", "none"));

	p_ip4 = ip4;

	if (strcmp(ip4, "none") == 0) {
		p_ip4 = NULL;
	}

	p_ip6 = ip6;

	if (strcmp(ip6, "none") == 0) {
		p_ip6 = NULL;
	}

	port4 = atoi(public_server_get_setting("PORT4", "--port4", "6800"));
	port6 = atoi(public_server_get_setting("PORT6", "--port6", "6800"));

	//ret = initNetMulitplayerPublicServer(p_ip4, port4, p_ip6, port6);

	ret = net_multiplayer_init_for_game_server(p_ip4, port4, p_ip6, port6);

	return ret;
}

static void load_arena()
{
	choice_arenaFile = arena_file_get_file_format_net_name(public_server_get_setting("ARENA", "--arena", "FAGN"));

	if (choice_arenaFile == NULL) {
		fprintf(stderr, _("I dont load arena %s!\n"), public_server_get_setting("ARENA", "--arena", "FAGN"));
		exit(-1);
	}

	arena = arena_file_get_arena(choice_arenaFile);

	arena_set_current(arena);
}

int public_server_init()
{
	int ret;
	int i;

	id_init_list();
	module_init();
	arena_file_init();
	tux_init();
	item_init();
	shot_init();
	server_configFile_init();

	ret = log_init(public_server_get_setting("LOG_FILE", "--log-file", "/tmp/tuxanci-server.log"));

	if (ret < 0) {
		fprintf(stderr, _("I was unable to open config file!\n"));
		return -1;
	}

	high_score_init(public_server_get_setting("SCORE_FILE", "--score-file", "/tmp/highscore.txt"));

	load_arena();

	for (i = 0; i < atoi(public_server_get_setting("ITEM", "--item", "10")); i++) {
		item_add_new_item(arena->spaceItem, ID_UNKNOWN);
	}

	isSignalEnd = FALSE;

	ret = public_server_initNetwork();

	if (ret < 0) {
		printf(_("Unable to initialize network socket!\n"));
		return -1;
	}

	server_set_max_clients(atoi (public_server_get_setting("MAX_CLIENTS", "--max-clients", "32")));

	if (public_server_register() < 0) {
		printf(_("Unable to contact MasterServer!)\n"));
	}

	return 0;
}

arenaFile_t *choice_arena_get()
{
	return choice_arenaFile;
}

void public_server_event()
{
	static my_time_t lastActive = 0;
	my_time_t interval;

	net_multiplayer_event();

	if (isSignalEnd == TRUE) {
		public_server_quit();
	}

	if (lastActive == 0) {
		lastActive = timer_get_current_time();
	}

	interval = timer_get_current_time() - lastActive;

	if (interval < 50) {
		return;
	}
	//printf("interval = %d\n", interval);

	lastActive = timer_get_current_time();

	arena_event(arena);
}

void my_handler_quit(int n)
{
	DEBUG_MSG("my_handler_quit\n");

	isSignalEnd = TRUE;
}

void public_server_quit()
{
	DEBUG_MSG(_("Quitting public server\n"));

	net_multiplayer_quit();
	arena_destroy(arena);

	tux_quit();
	item_quiy();
	shot_quit();

	arena_file_quit();
	server_configFile_quit();
	module_quit();
	id_quit_list();
	high_score_quit();
	log_quit();

	exit(0);
}

int public_server_start()
{
#ifndef __WIN32__
	signal(SIGINT, my_handler_quit);
	signal(SIGTERM, my_handler_quit);
	signal(SIGQUIT, my_handler_quit);
#endif
	if (public_server_init() < 0) {
		public_server_quit();
		return -1;
	}

	char *s = public_server_get_setting("DAEMON", "--daemon", "none");

	if (atoi(s))
		daemonize();

	for (;;)
		public_server_event();

	return 0;
}
