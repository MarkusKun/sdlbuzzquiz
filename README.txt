SDL-Buzz-Quiz (or whatever)

At the moment, this is a simple quizgame. It was 
written with the purpose of being fun.

The idea originated from some game-session at the OH14
in germany: We had a PS2, eight buzzers and a beamer.
Originally we thought that people might have more fun
with the Buzz-Junior games and wouldn't like to
try the more "serious" quiz-games - after all, the 
quizgames tend to depend on "common knowledge", something
that is rather spare in the usual OH14-game-session crowd. 

We were wrong. We also were surprised that most of the
evening, they didn't try the "usual" game, guided by the 
quizmaster and using different modes: They preferred the
rather simple "one question, four answers; players taking
the correct answer get points based on their speed"-approach.

And they had lots of fun.

In our eternal quest to provide even more fun, we looked at
the game and tried to find points to tweak.

The first "flaw" was the fixed arsenal of questions. Granted,
the Playstation-disc comes with a large number of them but still
it's a finite number. Also - as stated above - those question
could be better-scoped. Basically, it seemed like a good idea
to support *any* catalogue of question.
This also coincided with the wish of a "OH14-centered" quizgame -
containing questions about our building, Dortmund, Computer
Science in general, ... 

The second "flaw" was that it is a Playstation2 game and
we'd have to have a PS2 whenever we wanted to play it. If it
were a PC game, we'd have a large number of available "consoles"
nearby as many people carry a laptop and we have lots of 
poolcomputers near the beamer. Basically, it seemed like a good
idea to code a new game from scratch.
This also coincided with the "open-source"-approach, the
usage of USB-Connectors on the Buzzer, the possibility of
storing questions in a PC format and the third flaw.

The third "flaw" was the limited number of players: The
Playstation games only support eight players at most. This
mainly boiled down to the PS2 only having two USB ports and 
limited screen space for display of the players and the fact
that eight people in front of a TV already is a bit of crowd
and with some more, people might not see the screen anymore.
Of course, by writing a new game from scratch, we could 
purge the hard-coded-limit of eight.

The fourth "flaw" was the inflexibility of players. If you
had seven players and then started a game of 100 questions and
a possible eight player entered the room a minute later, you 
either had to restart the game completely - or play without him.
On the other hand, if someone got tired after the first 70 
question, there was no way of removing him from the game.
As this game was supposed to be a "party-game", it seemed
like a good idea to support joining and parting mid-game.

Previous Work:
Supporting input from Playstation Buzzers on PC is rather
simple: Each 4-Buzzer-USB-device registers as a 20-button-joystick
with a rather simple mapping.

Still, I only found one game supporting them:
Musikquiz is a game, taking a playlist in m3u-Format and
then playing songs from it. The players (up to four) have
to identify the song. First one to buzz and then correct the
right answer wins points.

In my opinion, it's rather limited. It's a Windows-game,
only supports four players and can only be used for music.
Still, it can be quite some fun, so you might want to 
check it out:
http://www.familie-siegert.de/musik.htm

It seems like quizxpress also focusses on quizzes and supports
the buzzers. I haven't yet tried it. It's commercial software.
http://www.quizxpress.com/



We had a huge laugh as we put some Zelda-Soundtrack-CDs into
a playlist and watched in horror how some huge Zelda-fan
identified most of them in the first few seconds - He
seemed to identify the songs by the noise before the music started.

Usage of this game:
You _can_ play this game using the keyboard but this was mainly
added for debugging. This game was written with buzzers in mind,
so connect your USB-Buzzers to the computer before starting
the game. There should be no limit to the number of Buzzers
connected (apart from the generic "127 devices on a USB-controller",
but if you ever get to this limit, I want a picture of the 508 
people crammed into the room).

Admin screen:
The game first starts into the admin screen. Later, you can
call the admin-screen by pressing the red buzzer button on any
of the buzzers or by pressing escape on your keyboard.
The admin interface is used to delete players or change their
names. You can also quit the game from it. As each pressed button
is displayed, it may also double for testing.
Note that "adding players" is no special function for the admin-
interface. Players can be added at any time (simply press a button
on the appropiate buzzer). By adding them during the admin-
interface, you can rename them very quickly - and test all
of their buttons.

Add player: Press a button on his buzzer.
Delete player: Mouseclick the "DEL" before his name
Rename player: Click the name
Return to game: Click the bar on the lower end of the screen.
Quit the game: Click the "X" on the upper-right end of the screen
Test buttons: Mash the buttons on the buzzers and enjoy the lightshow.

Game screen:
Top: Question
Left: Possible answers and their colors
Button: Player names, score, reaction times, chosen answer.

Configuration:
Configuration is done with xml-files. More information follows.

Adding questions:
The questions are contained in xml-files. More information follows.

Legal stuff:
Play it for free, change the code for free, but think
about publishing it with code, again (GNU GPLv3).

The used font is "Liberation". Its license is GNU?

XML is read with RapidXML. Its license is ???







