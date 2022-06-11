/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsuardi <lsuardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/11 14:27:27 by lsuardi           #+#    #+#             */
/*   Updated: 2022/06/11 14:46:41 by lsuardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

#define MODE_APPLY( true_action, false_action ) \
	do\
		if (add)\
			true_action ;\
		else\
			false_action ;\
	while (0)

namespace irc
{

	void	Server::m_execMode( Client &sender, const vector<string> &arg )
	{
		static map< char, int >	userModes = INIT_USERMODES(),
								channelModes = INIT_CHANNELMODES();
		bool					add = true;
		ostringstream			response;

		if (arg.size() < 2)
			response << m_prefix() << ERR_NEEDMOREPARAMS << " * MODE :Not enough parameters" << m_endl();
		else if (!m_isLogged(sender))
			response << m_prefix() << ERR_NOTREGISTERED << " * MODE :You have not registered" << m_endl();
		else
		{
			string::const_iterator it;

			if (arg.size() > 2)
			{
				it = arg[2].begin();
				if (*it == '-' || *it == '+')
					add = (*it++ == '+');
			}
			if (*arg[1].data() == '&' || *arg[1].data() == '#')
			{
				// CHANNEL MODES
				Channel	*chan;

				try
				{
					chan = &m_channels.at(arg[1]);
				}
				catch (const exception& e)
				{
					response << m_prefix() << ERR_NOSUCHCHANNEL << ' ' << arg[1] << " :No such channel" << m_endl();
					goto END;
				}
				if (!sender.hasMode(UMODE_OPERATOR) && chan->users.find(&sender) == chan->users.end())
				{
					response << m_prefix() << ERR_NOTONCHANNEL << ' ' << arg[1] << " :You're not on that channel" << m_endl();
					goto END;
				}
				if (arg.size() == 2)
				{
					response << m_prefix() << RPL_CHANNELMODEIS << ' ' << sender.nickname << ' ' << arg[1] << " :" << chan->getModes() << m_endl();
					goto END;
				}
				if (!sender.hasMode(UMODE_OPERATOR) && !chan->isOperator(sender))
					response << m_prefix() << ERR_CHANOPRIVSNEEDED << ' ' << chan->name << " :You're not channel operator" << m_endl();
				else
				{
					int	nextArg = 3;

					while (it != arg[2].end())
					{
						Client			*user;
						bool			banByHost = false;
						const string	*key;
						size_t			limit;

						if (strchr("olbvk", *it))
						{
							if (arg.size() <= (size_t)nextArg)
							{
								response << m_prefix() << ERR_NEEDMOREPARAMS << " * MODE :Not enough parameters" << m_endl();
								++it;
								continue ;
							}
							if (*it == 'k')
							{
								if (add)
									key = &arg[nextArg++];
							}
							else if (*it == 'l')
							{
								if (add)
								{
									istringstream	is(arg[nextArg++]);
									is >> limit;
									if (!is.eof() || !limit)
									{
										++it;
										continue ;
									}
								}
							}
							else if (*arg[nextArg].data() == '@')
							{
								try
								{
									user = &m_findClientByHost(arg[nextArg++]);
								}
								catch (...)
								{
									++it;
									continue ;
								}
								banByHost = true;
							}
							else
							{
								try
								{
									user = &m_findClient(arg[nextArg++]);
								}
								catch (...)
								{
									response << m_prefix() << ERR_NOSUCHNICK << ' ' << arg[nextArg - 1] << " :No such nick" << m_endl();
									++it;
									continue ;
								}
							}
						}
						ostringstream info;
						switch (*it)
						{
							case 'o':
								MODE_APPLY(chan->op(*user), chan->deop(*user));
								info << sender.makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << 'o';
								break ;
							case 'l':
								MODE_APPLY(chan->userLimit = limit, chan->userLimit = 0);
								info << sender.makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << 'l';
								if (add) info << ' ' << chan->userLimit;
								info << m_endl();
								break ;
							case 'b':
								info << sender.makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << "b ";
								if (banByHost)
								{
									MODE_APPLY(chan->banHostname(user->hostname), chan->unbanHostname(user->hostname));
									info << user->hostname;
								}
								else
								{
									MODE_APPLY(chan->banNickname(user->nickname), chan->unbanNickname(user->nickname));
									info << user->nickname;
								}
								info << m_endl();
								break ;
							case 'v':
								MODE_APPLY(chan->voice(*user), chan->unvoice(*user));
								info << sender.makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << "v " << user->nickname << m_endl();
								break ;
							case 'k':
								MODE_APPLY(chan->password = *key, chan->password.clear());
								info << sender.makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << 'k';
								if (add) info << ' ' << *key;
								info << m_endl();
								break ;
							default:
								try
								{
									MODE_APPLY(chan->addMode(channelModes.at(*it)), chan->delMode(channelModes.at(*it)));
									info << sender.makePrefix() << "MODE " << chan->name << ' ' << (add ? '+' : '-') << *it << m_endl();
								}
								catch (...)
								{
									info << m_prefix() << ERR_UNKNOWNMODE << ' ' << *it << " :is unknown mode char to me" << m_endl();
								}
						}
						typedef set< const Client* >::const_iterator CliIter;
						for (CliIter it2 = chan->users.begin(); it2 != chan->users.end(); ++it2)
							m_appendToSend((*it2)->sockfd, info.str());
						info.clear();
						++it;
					}
				}
			}
			else
			{
				// USER MODES
				Client	*user;

				try
				{
					user = &m_findClient(arg[1]);
				}
				catch (...)
				{
					response << m_prefix() << ERR_NOSUCHNICK << ' ' << arg[1] << " :No such nick" << m_endl();
					goto END;
				}
				if (arg.size() == 2)
				{
					response << sender.makePrefix() << RPL_UMODEIS << ' ' << arg[1] << " :" << user->getModes() << m_endl();
					goto END;	
				}
				while (it != arg[2].end())
				{
					ostringstream info;

					if (!sender.hasMode(UMODE_OPERATOR))
					{
						if (*it == 'o')
							response << m_prefix() << ERR_NOPRIVILEGES << " * MODE :Permission Denied- You're not an IRC operator" << m_endl();
						else if (arg[1] != sender.nickname)
							response << m_prefix() << ERR_USERSDONTMATCH << " * MODE :Cant change mode for other users" << m_endl();
						else
							goto APPLY;
						++it;
						continue ;
					}
					APPLY:
					try
					{
						MODE_APPLY(user->addMode(userModes.at(*it)), user->delMode(userModes.at(*it)));
						info << sender.makePrefix() << "MODE " << user->nickname << ' ' << (add ? '+' : '-') << *it << endl;
					}
					catch (...)
					{
						response << m_prefix() << ERR_UNKNOWNMODE << ' ' << *it << " :is unknown mode char to me" << m_endl();
					}
					if (sender.sockfd != user->sockfd)
						m_appendToSend(user->sockfd, info.str());
					response << info.str();
					info.clear();
					++it;
				}
			}
		}

		END:
		m_appendToSend(sender.sockfd, response.str());
	}

}
