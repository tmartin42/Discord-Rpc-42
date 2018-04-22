/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   discord-42-test.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmartin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/21 18:52:11 by tmartin           #+#    #+#             */
/*   Updated: 2018/04/22 15:51:27 by tmartin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "../discord-install/include/discord_register.h"
#include "../discord-install/include/discord_rpc.h"
#include <sys/utsname.h>
#include <stdlib.h>

bool	connected = false;

static void updateDiscordPresence(time_t t)
{
	char 					buffer[256];
	struct utsname 			*buf;
	char 					*login;
	DiscordRichPresence 	discordPresence;
	char					*tmpptr;
	bool					failed;

	memset(&discordPresence, 0, sizeof(discordPresence));
	buf = (utsname *)malloc(sizeof(utsname));
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
				sprintf(buffer, "At: %s", buf->nodename);
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


static int prompt(char* line, size_t size)
{
	int res;
	char* nl;
	printf("\n> ");
	fflush(stdout);
	res = fgets(line, (int)size, stdin) ? 1 : 0;
	line[size - 1] = 0;
	nl = strchr(line, '\n');
	if (nl) {
		*nl = 0;
	}
	return res;
}

static void handleDiscordReady(const DiscordUser* connectedUser)
{
	printf("\nDiscord: connected to user %s#%s - %s\n",
			connectedUser->username,
			connectedUser->discriminator,
			connectedUser->userId);
	connected = true;
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
	printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
	Discord_ClearPresence();
}

static void handleDiscordError(int errcode, const char* message)
{
	printf("\nDiscord: error (%d: %s)\n", errcode, message);
	Discord_ClearPresence();
}

static void handleDiscordJoin(const char* secret)
{
	printf("\nDiscord: join (%s)\n", secret);
}

static void handleDiscordSpectate(const char* secret)
{
	printf("\nDiscord: spectate (%s)\n", secret);
}

static void handleDiscordJoinRequest(const DiscordUser* request)
{
	printf("\nDiscord: join request from %s#%s - %s\n",
			request->username,
			request->discriminator,
			request->userId);
	Discord_Respond(request->userId, DISCORD_REPLY_NO);
}


int main(void)
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;

	Discord_Initialize("437292685569556490", &handlers, 1, NULL);

	char line[512];
	char* space;
	time_t t = time(0);
	updateDiscordPresence(t);
	while (!connected)
	{
		Discord_RunCallbacks();
		sleep(10);
	}
	updateDiscordPresence(t);
	while (prompt(line, sizeof(line))) {
		if (line[0]) {
			if (line[0] == 'q') {
				break;
			}
			updateDiscordPresence(t);
		}
		Discord_RunCallbacks();
	}
	printf("Shutting down...\n");
	Discord_ClearPresence();
	sleep(2);
	Discord_Shutdown();
	sleep(2);
	return (0);
}
