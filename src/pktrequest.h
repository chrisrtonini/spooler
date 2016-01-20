/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * spooler
 * Copyright (C) Chris Robert Tonini 2015 <chrisrtonini@gmail.com>
 * 
 * spooler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * spooler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PKTREQUEST_H_
#define _PKTREQUEST_H_

#include "packet.h"


/**
 \class request_packet
 \brief Estrutura de pacote de comando.
 */
class request_packet: public packet 
{
	public:
								request_packet(void);
								request_packet(const std::string& ssid);
								request_packet(const std::string& ssid, 
								    const std::string& cmd_id);
								~request_packet(void);
		void					request(const std::string& cmd_id);
		const std::string&		request(void) const NO_THROW;

	private:
		tags::packet_context	m_context;
};

#endif // _PKTREQUEST_H_
