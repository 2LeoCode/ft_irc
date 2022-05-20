/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Leo Suardi <lsuardi@student.42.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 14:26:16 by lsuardi           #+#    #+#             */
/*   Updated: 2022/05/19 23:16:43 by Leo Suardi       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cstdlib>
#include <climits>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <set>
#include <list>
#include <algorithm>
#include <cstdarg>
#include "data.hpp"

// https://datatracker.ietf.org/doc/html/rfc1459#section-6

enum e_replies {

	// COMMAND RESPONSES

	RPL_TRACELINK = 200,
	RPL_TRACECONNECTING,
	RPL_TRACEHANDSHAKE,
	RPL_TRACEUNKNOWN,
	RPL_TRACEOPERATOR,
	RPL_TRACEUSER,
	RPL_TRACESERVER,
	RPL_TRACENEWTYPE = 208,
	RPL_STATSLINKINFO = 211,
	RPL_STATSCOMMANDS,
	RPL_STATSCLINE,
	RPL_STATSNLINE,
	RPL_STATSILINE,
	RPL_STATSKLINE,
	RPL_STATSYLINE = 218,
	RPL_ENDOFSTATS,
	RPL_UMODEIS = 221,
	RPL_STATSLLINE = 241,
	RPL_STATSUPTIME,
	RPL_STATSOLINE,
	RPL_STATSHLINE,
	RPL_LUSERCLIENT = 251,
	RPL_LUSEROP,
	RPL_LUSERUNKNOWN,
	RPL_LUSERCHANNELS,
	RPL_LUSERME,
	RPL_ADMINME,
	RPL_ADMINLOC1,
	RPL_ADMINLOC2,
	RPL_ADMINEMAIL,
	RPL_TRACELOG = 261,
	RPL_NONE = 300,
	RPL_AWAY,
	RPL_USERHOST,
	RPL_ISON,
	RPL_UNAWAY,
	RPL_NOWAWAY,
	RPL_WHOISUSER = 311,
	RPL_WHOISSERVER,
	RPL_WHOISOPERATOR,
	RPL_WHOWASUSER,
	RPL_ENDOFWHO,
	RPL_WHOISIDLE = 317,
	RPL_ENDOFWHOIS,
	RPL_WHOISCHANNELS,
	RPL_LISTSTART = 321,
	RPL_LIST,
	RPL_LISTEND,
	RPL_CHANNELMODEIS,
	RPL_NOTOPIC = 331,
	RPL_TOPIC,
	RPL_INVITING = 341,
	RPL_SUMMONING,
	RPL_VERSION = 351,
	RPL_WHOREPLY,
	RPL_NAMREPLY,
	RPL_LINKS = 364,
	RPL_ENDOFLINKS,
	RPL_ENDOFNAMES,
	RPL_BANLIST,
	RPL_ENDOFBANLIST,
	RPL_ENDOFWHOWAS,
	RPL_INFO = 371,
	RPL_MOTD,
	RPL_ENDOFINFO = 374,
	RPL_MOTDSTART,
	RPL_ENDOFMOTD,
	RPL_YOUREOPER = 381,
	RPL_REHASHING,
	RPL_TIME = 391,
	RPL_USERSSTART,
	RPL_USERS,
	RPL_ENDOFUSERS,
	RPL_NOUSERS,


	// ERROR REPLIES

	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHSERVER,
	ERR_NOSUCHCHANNEL,
	ERR_CANNOTSENDTOCHAN,
	ERR_TOOMANYCHANNELS,
	ERR_WASNOSUCHNICK,
	ERR_TOOMANYTARGETS,
	ERR_NOORIGIN = 409,
	ERR_INVALIDCAPCMD,
	ERR_NORECIPIENT,
	ERR_NOTEXTTOSEND,
	ERR_NOTOPLEVEL,
	ERR_WILDTOPLEVEL,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NOMOTD,
	ERR_NOADMININFO,
	ERR_FILEERROR,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME,
	ERR_NICKNAMEINUSE,
	ERR_NICKCOLLISION = 436,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL,
	ERR_USERONCHANNEL,
	ERR_NOLOGIN,
	ERR_SUMMONDISABLED,
	ERR_USERSDISABLED,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED,
	ERR_NOPERMFORHOST,
	ERR_PASSWDMISMATCH,
	ERR_YOUREBANNEDCREEP,
	ERR_KEYSET = 467,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE,
	ERR_INVITEONLYCHAN,
	ERR_BANNEDFROMCHAN,
	ERR_BADCHANNELKEY,
	ERR_NOPRIVILEGES = 481,
	ERR_CHANOPRIVSNEEDED,
	ERR_CANTKILLSERVER,
	ERR_NOOPERHOST = 491,
	ERR_UMODEUNKNOWNFLAG = 501,
	ERR_USERSDONTMATCH
};

namespace irc
{
	using std::string;
	using std::vector;
	using std::map;
	using std::queue;
	using std::set;
	using std::list;
	using std::pair;
	using std::count;
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::ostringstream;
	using std::istringstream;
	using std::ifstream;
	using std::make_pair;
	using std::exception;
	using std::runtime_error;
	using std::out_of_range;
	using data::Trie;
}

#include "utils.hpp"
#include "irc/Channel/Channel.hpp"
#include "irc/Client/Client.hpp"
#include "irc/Server/Server.hpp"
