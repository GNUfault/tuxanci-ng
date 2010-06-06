#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#ifndef __WIN32__
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#else /* __WIN32__ */
#include <windows.h>
#include <wininet.h>
#endif /* __WIN32__ */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h> 
#include <sys/stat.h> 

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

void daemonize()
{
#ifndef __WIN32__
	int i, lockfd, ipid;
	char pid[16];
	char spid[64];

	ipid = getpid();

	/* detach if asked */
	if (ipid == 1) {
		return;				/* already a daemon */
	}

	/* fork to guarantee we are not process group leader */
	i = fork();

	if (i < 0) {
		exit(1);			/* fork error */
	} else if (i > 0) {
		exit(0);			/* parent exits */
	}

	/* child (daemon) continues */
	setsid();				/* obtain a new process group */
	ipid = getpid() + 1;

	printf("The Tuxanci game server started with PID %d\n", ipid);
	/* fork again so we become process group leader
	 * and cannot regain a controlling tty
	 */
	i = fork();

	if (i < 0) {
		exit(1);			/* fork error */
	} else if (i > 0) {
		exit(0);			/* parent exits */
	}

	/* close all fds (descriptors) */
	for (i = getdtablesize(); i >= 0; --i) {
		close(i);
	}

	/* close parent fds and send output to fds 0, 1 and 2 to bitbucket */
	i = open("/dev/null", O_RDWR);

	if (i < 0) {
		exit(1);
	}

	dup(i);
	dup(i);					/* handle standard I/O */

	sprintf(spid, "/tmp/tuxanci-server.pid");

	/* create local lock */
	lockfd = open(spid, O_RDWR | O_CREAT, 0640);

	if (lockfd < 0) {
		perror("[Error] Lock: open\n");
		exit(1);
	}
	#ifndef __CYGWIN__
	/* lock the file */
	if (lockf(lockfd, F_TLOCK, 0) < 0) {
		error("Lock: lockf");
		warning("The Tuxanci game server is already running");
		exit(0);
	}
	#else /* __CYGWIN__ */
	/* lock the file */
	{
		struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_start = 0;
		lock.l_whence = SEEK_SET;
		lock.l_len = 0;
		
		if (fcntl(lockfd, F_SETLK, &lock) < 0) {
			warning("The Tuxanci game server is already running");
			exit(0);
		}
	}
	#endif /* __CYGWIN__ */

	/* write to pid to lockfile */
	snprintf(pid, 16, "%d\n", getpid());
	write(lockfd, pid, strlen(pid));

	/* restrict created files to 0750 */
	umask(027);
#endif /* __WIN32__ */
}

static int public_server_register()
{
#ifndef __WIN32__
	int s;
#else /* __WIN32__ */
	SOCKET s;
#endif /* __WIN32__ */

	struct sockaddr_in server;
	char *master_server_ip;

	master_server_ip = dns_resolv(NET_MASTER_SERVER_DOMAIN);

	if (master_server_ip == NULL) {
		warning("Did not obtained IP of the MasterServer");
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(NET_MASTER_PORT);
	server.sin_addr.s_addr = inet_addr(master_server_ip);

	free(master_server_ip);

	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		warning("Unable to open socket for connection to MasterServer");
		return -1;
	}

	/* connect to MasterServer */
	if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
		warning("Unable to estabilish connection to MasterServer");
		return -1;
	}

	typedef struct {
		unsigned char cmd;
		unsigned port;
		unsigned ip;
	} __PACKED__ register_head;

	register_head *head = (register_head *) malloc(sizeof(register_head));

	head->cmd = 'p';
	head->port = atoi(public_server_get_setting("PORT4", "--port4", "6800"));
	head->ip = 0;				/* TODO */

	/* send request for server list */
	int r = send(s, head, sizeof(register_head), 0);

	free(head);

#ifndef __WIN32__
	close(s);
#else /* __WIN32__ */
	closesocket(s);
#endif /* __WIN32__ */

	if (r == -1) {
		return -1;
	} else {
		return 0;
	}
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

	ret = net_multiplayer_init_for_game_server(p_ip4, port4, p_ip6, port6);

	return ret;
}

static void load_arena()
{
	choice_arenaFile = arena_file_get_file_format_net_name(public_server_get_setting("ARENA", "--arena", "FAGN"));

	if (choice_arenaFile == NULL) {
		error("Unable to load arena [%s]", public_server_get_setting("ARENA", "--arena", "FAGN"));
		/* TODO: Why not to log it? */
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
		/* Error message has been already printed */
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
		error("Unable to initialize network socket");
		/* TODO: Why not to log it? */
		return -1;
	}

	server_set_max_clients(atoi(public_server_get_setting("MAX_CLIENTS", "--max-clients", "32")));

	if (atoi(public_server_get_setting("LAN_ONLY", "--lan-only", "0")) == 0) {
		if (public_server_register() < 0) {
			error("Unable to contact MasterServer");
			/* TODO: Why not to log it? */
		}
	} else {
		debug("Starting LAN-only server");
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

	lastActive = timer_get_current_time();

	arena_event(arena);
}

void my_handler_quit(int n)
{
	debug("Received signal %d", n);
	/* TODO: Why not to log it? */

	isSignalEnd = TRUE;

	public_server_quit();
}

void public_server_quit()
{
	debug("Shutting down the Tuxanci game server");
	/* TODO: Why not to log it? */

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
#endif /* __WIN32__ */
	if (public_server_init() < 0) {
		public_server_quit();
		return -1;
	}

	char *s = public_server_get_setting("DAEMON", "--daemon", "0");

	if (atoi(s)) {
		daemonize();
	}

	for (;;) {
		public_server_event();
	}

	return 0;
}
