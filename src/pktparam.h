/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * spooler
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

#ifndef _PKTPARAM_H_
#define _PKTPARAM_H_

#include <string>
#include <vector>
#include <exception>

#include "excp.h"


/**
 \class pkt_param
 \brief Definição de parâmetro pacotes de comando (ou resposta) do spooler.
 */
class pkt_param
{
	public:
											pkt_param(void);
											pkt_param(const pkt_param& pkt);
											pkt_param(const std::string& name);
											pkt_param(const std::string& name, 
											    const std::string& val);
											~pkt_param(void);
		void								name(const std::string& txt);
		const std::string&					name(void) const NO_THROW;
		void								value(const std::string& val);
		void								add_value(const std::string& val);
		const std::string&					value(void)
												throw(pkt_param_exp);
		const std::string&					next_value(void)
												throw(pkt_param_exp);
		int									num_lines(void) const NO_THROW;
		const pkt_param&					operator=(const pkt_param& obj)
												NO_THROW;
		const pkt_param&					operator=(const pkt_param* obj)
												NO_THROW;
		const pkt_param&					copy(const pkt_param* obj)
												NO_THROW;

	protected:

	private:
		std::string							m_name;
		std::vector<std::string>			m_val;
		std::vector<std::string>::iterator  m_idx;
};

#endif // _PKTPARAM_H_
