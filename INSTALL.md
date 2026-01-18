# HOW TO INSTALL TUXANCI

## Client
Requirements: `cmake >= 2.6.0`, `SDL`, `SDL_image`, `SDL_ttf`, (`SDL_mixer`), (`OpenGL`),
	      (`Gettext`), `libzip`
(Those in brackets are optional and can be omitted if you choose appropriate
switches while building the game. But we recommend to have all of them.)

Note: you'll also need "-devel" versions of library packages.

The following how-to is written for GNU/Linux, but we are working on ports and
how-tos for other platforms too. If you want to help us, feel free to contact
us! We will really appreciate it.

Note: if you don't know all available cmake switches which can be used, you can
run `ccmake` command (it seems to be ncurses based) instead of `cmake`. And
please do enable the debug -- it helps us when you find a bug.

```
$ mkdir build
$ cmake .. -DENABLE_DEBUG=1
$ make
# make install
```

Now it is installed in your system. And that's all. :c) You can run the game
simply by running `tuxanci` command in the console (you'll see much of debug
output, but that's ok).

Tip: please think about running this command too (before the game itself):

```
$ ulimit -c unlimited
```

It enables generating of coredumps which are very helpful when the game crashes
as it contains much information about the crash which happend. You also need to
have debug enabled when compiling -- otherwise the coredump won't contain any
useful information for us.

Note: if you want to uninstall the game, run this command:

```
# make uninstall
```

## Server
Requirements: `cmake >= 2.6.0`, `libzip`

Note: you'll also need "-devel" version of libzip library package.

The server is written for GNU/Linux and is not planned to be ported to other
platforms (may be to BSD yes, but surely not to Windows ;c)).

Note: if you don't know all available cmake switches which can be used, you can
run `ccmake` command (it seems to be ncurses based) instead of `cmake`. And
please do enable the debug -- it helps us when you find a bug.

```
$ mkdir build
$ cmake .. -DBUILD_SERVER=1 -DENABLE_DEBUG=1
$ make
# make install
```

Now it is installed in your system. You should configure it before running. The
configuration file is situated in /etc/tuxanci-server/server.conf. It contains
some kind of example configuration which is usable but you'll probably prefer
your own one. Have a look to /etc/tuxanci-server/arena.conf -- there are listed
all available arenas and at the end of each line is "network name" of each
arena. This network name is to be used in server.conf file within "ARENA"
property.

When you set IP6 property to ::, it will also accept IPv4 connections and IP4
property won't be taken into account. LAN_ONLY means that the server will not
register at MasterServer. You can let be the SUPPORT_CLIENTS property as it is
going to be removed soon (default value is OK).

If you want to run more than one Tuxanci game server, you need to have more
configuration files (one for each game server). Specify which one to use by
--config-file switch (e.g. --config-file=/etc/tuxanci-server/server-2.conf)
when running the server.

Note: RC script is not prepared yet. You have to write your own one.

The game server itself can be started with following:

```
$ tuxanci-server
```

(If you have set appropriate paths and port, you don't need to have root
privileges to start the server. Note: droping privileges is not implemented
yet.)

Tip: if you have enabled debug when compiling, please think about running this
command before starting the game server:

```
$ ulimit -c unlimited
```

It enables generating of coredumps which are very helpful when the game server
crashes as it contains much information about the crash which happend.

Note: if you want to uninstall the game server, run this command:

```
# make uninstall
```
