/*
 *  (C) Copyright 2006 ZeXx86 (zexx86@gmail.com)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#include <stdio.h>
#include <string>
#include <signal.h>
#include <iostream>
#include <netinet/in.h>
#include "sockets.h"
#include "resolve.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


bool cont = 1;

using namespace std;

/*************************************************************\
|		   	ARGUMENTS		              |
\*************************************************************/

#ifndef __WIN32__
void daemonize ()
{
	int i, lockfd;
  	char pid[16];

	int ipid = getpid ();

  	/* detach if asked */
  	if (ipid == 1)
		return;			/* already a daemon */

    	/* fork to guarantee we are not process group leader */
    	i = fork ();
    	if (i < 0)
      		exit (1);			/* fork error */
    	if (i > 0)
      		exit (0);			/* parent exits */

    	/* child (daemon) continues */
    	setsid ();			/* obtain a new process group */
	ipid = getpid ()+1;

	printf ("> Master server is running now\n> started with pid -> %d\n", ipid);
    	/* fork again so we become process group leader 
     	 * and cannot regain a controlling tty 
     	 */
    	i = fork ();
    	if (i < 0)
      		exit (1);			/* fork error */
    	if (i > 0)
      	exit (0);			/* parent exits */

    	/* close all fds */
    	for (i = getdtablesize (); i >= 0; --i)
      		close (i);		/* close all descriptors */

    	/* close parent fds and send output to fds 0, 1 and 2 to bitbucket */
    	i = open ("/dev/null", O_RDWR);
    	if (i < 0)
      		exit (1);
    	dup (i);
    	dup (i);			/* handle standart I/O */

  	/* create local lock */
  	lockfd = open ("master.pid", O_RDWR | O_CREAT, 0640);
  	if (lockfd < 0) {
    		perror ("lock: open");
    		exit (1);
  	}
	#ifndef __CYGWIN__
	/* lock the file */
	if (lockf (lockfd, F_TLOCK, 0) < 0) {
		perror ("lock: lockf");
		printf ("Master server is already running.\n");
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
			printf ("Master server is already running.\n");
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

int arguments (int argc, char *argv[])
{
	if (argc == 1)
		return 0;

	if (argc == 2) {
#ifndef __WIN32__
		if (!strcmp (argv[1], "-d")) {
			daemonize ();
			return 0;
		}
#endif
	}

	return 1;
}

void init ()
{
	init_sockets ();
	init_resolve ();
}

/*************************************************************\
|		   	MAIN THREAD		              |
\*************************************************************/

int main(int argc, char *argv[])
{
	arguments (argc, argv);

	init ();

	puts ("> Master server is started");

	while (cont)
		loop ();

	puts ("> Server going down");

//	close(mainSocket);

	return 0;
}
