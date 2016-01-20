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

#ifndef _PACKET_H_
#define _PACKET_H_


#include <string>
#include <vector>
#include <exception>

#include "pktparam.h"
#include "pkttags.h"

#include "excp.h"


/**
 \class packet
 \brief Definição de pacote para comandos (e respostas) do spooler.
 */
class packet
{
	public:
											packet(void);
											~packet(void);
		const tags::packet_type&			type(void) const NO_THROW;
		const std::string&					session(void) const NO_THROW;
		const std::string&					id(void) const NO_THROW;
		void								add_param(const pkt_param& param);
		void								add_param(const pkt_param* param);
		void								add_param(const std::string& key,
														const std::string& val);
		const pkt_param&					first_param(void)
												throw(packet_exp);
		const pkt_param&					next_param(void)
												throw(packet_exp);
		void								session(const std::string& ssid);
		void								type(tags::packet_type tp);
		void								id(const std::string& id);

	protected:
		void								dispose_params(void);
		std::vector<pkt_param>				m_params;
		std::vector<pkt_param>::iterator	m_idx;

	private:
		tags::packet_type					m_type;
		std::string							m_session;
		std::string							m_id;
};

#endif // _PACKET_H_
