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

#ifndef _PKTRESPONSE_H_
#define _PKTRESPONSE_H_

#include "packet.h"


#define PKT_RESPONSE_OK		0
#define PKT_RESPONSE_ERR	1


/**
 \class response_packet
 \brief Estrutura do pacote de resposta.
 \todo Ajustar a inicialização da propriedade m_retcode, de modo a esta
 representar um valor de "não inicialização", caso seja lida antes de ser
 formalmente atribuída.
 */
class response_packet: public packet 
{
	public:
								response_packet(void);
								response_packet(const std::string& ssid);
								response_packet(const std::string& ssid, 
								    const std::string& cmd_id);
								~response_packet(void);
		void					response(const std::string& cmd_id);
		const std::string&		response(void) const NO_THROW;
		std::string				context_tag(void) const NO_THROW;
		void					ret_code(int code) NO_THROW;
		int						ret_code(void) const NO_THROW;

	private:
		tags::packet_context	m_context;
		int						m_retcode;
};

#endif // _PKTRESPONSE_H_
