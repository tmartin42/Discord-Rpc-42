/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   discord-42-test.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/21 18:52:11 by tmartin           #+#    #+#             */
/*   Updated: 2018/04/22 19:47:29 by tmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../discord-install/include/discord_register.h"
#include "../discord-install/include/discord_rpc.h"
#include <sys/utsname.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>

bool		debug = false;

void		ctrlcHandler(int s) {
	if (debug)
		std::cout << "Caught signal " << s << std::endl;
	exit(1);
}

void 		exitHandler(void) {
	if (debug)
		std::cout << "Shutting Down..." << std::endl;
	Discord_ClearPresence();
	sleep(2);
	Discord_Shutdown();
	sleep(2);
}

static void updateDiscordPresence(time_t t)
{
	char 					buffer[256];
	struct utsname 			*buf;
	char 					*login;
	DiscordRichPresence 	discordPresence;
	char					*tmpptr;
	bool					failed;

	memset(&discordPresence, 0, sizeof(discordPresence));
	if (!(buf = (utsname *)malloc(sizeof(utsname))))
		exit(0);
	if ((login = getlogin()))
		discordPresence.state = login;
	else
		discordPresence.state = "";
	failed = true;
	if (!uname(buf)) {
		if (strlen(buf->nodename) > 0) {
			tmpptr = strchr(buf->nodename, '.');
			if (tmpptr) {
				tmpptr[0] = '\0';
				sprintf(buffer, "%s", buf->nodename);
				failed = false;
				if (buf->nodename[1] == '1')
					discordPresence.smallImageKey = "e1";
				else if (buf->nodename[1] == '2')
					discordPresence.smallImageKey = "e2";
				else if (buf->nodename[1] == '3')
					discordPresence.smallImageKey = "e3";
				else
					discordPresence.smallImageKey = "e";
			}

		}
	}
	if (failed == true) {
		sprintf(buffer, "Logged");
		discordPresence.smallImageKey = "e";
	}
	discordPresence.details = buffer;
	discordPresence.startTimestamp = t;
	discordPresence.largeImageKey = "42";
	Discord_UpdatePresence(&discordPresence);
	free(buf);
}

static void handleDiscordReady(const DiscordUser* connectedUser)
{
	if (debug)
		std::cout << "\n Discord: connected to user " << connectedUser->username << std::endl;
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
	if (debug)
		std::cout << "\nDiscord: disconnected (" << errcode << ": " << message << ")" << std::endl;
	Discord_ClearPresence();
}

static void handleDiscordError(int errcode, const char* message)
{
	if (debug)
		std::cout << "\nDiscord: disconnected (" << errcode << ": " << message << ")" << std::endl;
	Discord_ClearPresence();
}

static void handleDiscordJoin(const char* secret)
{
	if (debug)
		std::cout << "\nDiscord: join" << std::endl;
	printf("\nDiscord: join (%s)\n", secret);
}

static void handleDiscordSpectate(const char* secret)
{
	if (debug)
		std::cout << "\nDiscord: spectate" << std::endl;
}

static void handleDiscordJoinRequest(const DiscordUser* request)
{
	if (debug)
		std::cout << "\n Discord: join request from user " << request->username << std::endl;
	Discord_Respond(request->userId, DISCORD_REPLY_NO);
}


static void 	initDiscordRPC(void) {
	
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;
	Discord_Initialize("437292685569556490", &handlers, 1, NULL);
}

static void 	initSignals(void) {	
	struct sigaction sigIntHandler;
	
	sigIntHandler.sa_handler = ctrlcHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	
	sigaction(SIGHUP, &sigIntHandler,NULL);
	sigaction(SIGQUIT, &sigIntHandler,NULL);
	sigaction(SIGTERM, &sigIntHandler,NULL);
	sigaction(SIGINT, &sigIntHandler, NULL);
	std::atexit(exitHandler);
}

int main(int ac, char **av)
{
	time_t t = time(0);
	
	if (ac == 2 && av[1][0] == '-' && av[1][1] == 'd')
		debug = true;
	initDiscordRPC();
	initSignals();
	
	while (42) {
		sleep(10);
		updateDiscordPresence(t);
		Discord_RunCallbacks();
	}
	return (0);
}
