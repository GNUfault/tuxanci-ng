# Tuxánci
![Tuxánci](data/tuxanci.svg)
###### (I kept the bad grammer)
## ABOUT TUXANCI
Tuxanci is first tux shooter. In the absolute beginning it was inspired by
a well-known Czech game Bulanci. Today Tuxanci goes its own way and is much
better then Bulanci (except the graphics though).

Game supports 1 player and 2 players game on one computer and also supports
gameplay over network (LAN/Internet [IPv4 and IPv6]).

Goal in this game is to shoot enemy Tux before he does so. Tux can shoot only
in X and Y axes so no diagonal shooting is available yet (in fact it is even
not being planned at the moment).

## Weapon list
### Pistol
Basic gun - one shot at the time
### Two Pistols
2x Basic gun - two shots at once
### Machine gun
Few shots per fire, small intervals
### Shot gun
5 shots per fire, high dispersion
### Laser
Shoots fast and long coherent light
### Mine
Put them around, step on them *KABOOM*
### Bomb ball
Fires bomb which mirrors any obstacles and explodes only if it
hits penguin.
## Bonus list
- Speedy tux: tux move faster
- Infinite ammo: you have really big backpack of ammo (fire at will)
- 4way shoot: tux fire on shoot in every direction (cost only 1 ammo)
- Teleport: if somebody shoots at Tux he teleport himself away
- Ghost: Tux can move and shoot through obstacles.
### Note
Bonuses last only few seconds and if tux take another bonus then
first one loses its effects.

## GAME USAGE
Starting game is simple `tuxanci-VERSION` or `tuxanci-server-V`. based on what we
compiled (based on presumption user installed into directories where path
is set).

If you are using bundled binary and tuxanci are reporting "File not found!"
error, you need to launch it directly from BIN directory 
(eg.: `$ cd tuxanci/bin && ./tuxanci-VERSION`).

For client we create some configuration files in $HOME. There is created
`.tuxanci` directory and in there are pretty selfexplaining .conf files.

For server we create directory `/etc/tuxanci-server/server.conf` where you could
do some changes for it (again pretty selfexplaining).

## Controls
| Category       | Action                     | Key(s)                                |
|----------------|----------------------------|---------------------------------------|
| **Player One** | Movement                   | Cursor arrows                         |
|                | Fire                       | Zero [0] NumPad                       |
|                | Change weapon              | One [1] NumPad                        |
| **Player Two** | Movement                   | W (up), S (down), A (left), D (right) |
|                | Fire                       | Q                                     |
|                | Change weapon              | Tab                                   |
| **Common**     | Fullscreen/window mode     | F1 (dangerous with multiple monitors) |
|                | Save game                  | F2                                    |
|                | Change splitting of screen | F3 (local multiplayer in big arena)   |
|                | Pause                      | P                                     |
|                | Quit                       | Esc                                   |

## HAVE PROBLEM/FOUND BUG/ETC...
If you found some bug, have feature request, or need some help with, please open a [issue](https://github.com/GNUfault/extreme-tuxanci/issues).

## Original Tuxánci
The original Tuxánci can be found [here](https://repo.or.cz/tuxanci.git).

## NOTE
This is also on OSX and Windows, not just Linux.
Both of those ports don't compile and no longer maintained 
(Due to me not being able to maintain them by myself.
You can send me a email at <bluMATRIKZ@gmail.com> 
or open a issue if you want to maintain one of them).

## MORE
[xHire](https://github.com/xHire) (one of the former developers) has a [website](https://www.semirocket.science/projects/tuxanci/) with more information about Tuxánci.
