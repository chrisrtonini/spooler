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

#include "txtwriter.h"
#include "packet.h"


/**
 \brief Construtor
 \param out Referência à instância de objeto de stream de output.
 */
txt_writer::txt_writer(std::ostream* out) :
	rsp_writer<std::ostream*>::rsp_writer(out)
{
	// Nada a fazer...
}


/**
 \brief Grava pacote de resposta.
 \param Instância de objeto contendo pacote de resposta.
 */
void txt_writer::write_packet(const response_packet& pkt)
{
	try {
std::cerr << std::endl << tags::packet_tags::begin_tag() << std::endl;
		*m_out << tags::packet_tags::begin_tag() << std::endl;

std::cerr << tags::packet_tags::type(pkt.type());
		*m_out << tags::packet_tags::type(pkt.type());

std::cerr << pkt.context_tag() << std::endl;
		*m_out << pkt.context_tag() << std::endl;

		if ((pkt.type() == tags::tp_begin_session) ||
		    (pkt.type() == tags::tp_status))   {
std::cerr << SERVER_ID_TAG;
			*m_out << SERVER_ID_TAG;
std::cerr << APP_NAME << " - " << APP_DESCR << " (";
			*m_out << APP_NAME << " - " << APP_DESCR << " (";
std::cerr << APP_VERSION << "." << APP_RELEASE << APP_STATUS;
			*m_out << APP_VERSION << "." << APP_RELEASE << APP_STATUS;
std::cerr << ")" << std::endl;
			*m_out << ")" << std::endl;
		}

		if ((pkt.type() == tags::tp_begin_session) && 
		    (pkt.ret_code() != PKT_RESPONSE_OK))	{
			}
		else	{
std::cerr << SESSION_ID_TAG << pkt.session() << std::endl;
			*m_out << SESSION_ID_TAG << pkt.session() << std::endl;
		}
		
std::cerr << pkt.ret_code() << std::endl;
		*m_out << pkt.ret_code() << std::endl;
		try {
			bool head = true;
			bool outer = true;
			pkt_param obj;
			while (outer)   {
				try {
					if (head)   {
						obj = (const_cast<response_packet&>(pkt)).first_param();
						head = false;
					}
					else	{
						obj = (const_cast<response_packet&>(pkt)).next_param();
					}
					try {
						int lines = obj.num_lines();
std::cerr << obj.value() << std::endl;
						*m_out << obj.value() << std::endl;
						if (lines > 1)  {
							bool inner = true;
							while (inner)	{
								try {
std::cerr << obj.next_value() << std::endl;
									*m_out << obj.next_value() << std::endl;
								}
								catch (pkt_param_exp& e)	{
									inner = false;
								}
							}
						}
					}
					catch (pkt_param_exp& e) {
						// "Nenhum valor definido"
					}
				}
				catch (packet_exp& e)	{
					// "Nenhum parametro definido"
					// ou
					// "Nao ha mais parametros definidos"
					outer = false;
				}
			}
		}
		catch (packet_exp& e)	{
			// "Nenhum parametro definido"
		}
std::cerr << tags::packet_tags::end_tag() << std::endl;
		*m_out << tags::packet_tags::end_tag() << std::endl;
	}
	catch (...) {
		throw;
	}
}
