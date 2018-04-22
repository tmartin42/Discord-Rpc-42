# Discord-Rpc-42
 A little program to tell discord where you're connected at school. It shows since when you're logged, where you're logged, and also your login (this might change as people might not want to show it)

 For now you need to use the discord app and not browser discord (browser discord can't detect "games"), and you also need to launch my program or you can launch it in the background when you open your term (you need to setup it yourself)
## How to install it:
 First you will need to download and install the **[Discord-rpc SDK](https://github.com/discordapp/discord-rpc)**
 and compile it with the my program:

 ``` clang++ -framework Foundation -framework CoreServices ./discord-42-test.cpp ../discord-rpc/builds/osx-static/src/libdiscord-rpc.a  -o Discord-Rpc-42 ```

 (The -framework are for the objective-C used in the discord SDK)

### Usage:
Start the program. It takes time for it to connect to discord, (i also dont want to spam so i put a sleep() in it).

When you want to quit it, just write q in the prompt. If you dont do that, it will disconnect horribly. Please dont do that.

### What I want to do in the future:


* - [ ] Find a way to launch it without installing it
* - [ ] Change "playing" message to working
* - [ ] Solve Objective-C string leak
* - [*] Clean disconnect when using ctrl-c and other thingys
* - [ ] ~~Adopt a kitten~~

**Thanks for the support !**

*tmartin42*
