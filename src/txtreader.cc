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

#include "txtreader.h"
#include "packet.h"

#include "utils.h"


/**
 \brief Construtor
 \param in Referência à instância do objeto de stream de input.
 */
txt_reader::txt_reader(std::istream* in) :
	rqt_reader<std::istream*>::rqt_reader(in)
{
	// Nada a fazer...
}


/**
 \brief Verifica se o identificador de tipo do pacote é valido.
 \param token String contendo um identificador de tipo.
 \returns Verdadeiro ou falso, indicando que o token é ou não, válido.
 */
bool txt_reader::valid_packet_type(const std::string& token) const
{
	bool								ret = false;
	std::vector<std::string>			type_tags;
	std::vector<std::string>::iterator  test;

	for (int i = tags::tp_begin_session; i != tags::tp_dead_end; i++)   {
		type_tags.push_back(
		    tags::packet_tags::type(static_cast<tags::packet_type>(i))
		    + tags::packet_tags::context(tags::ctx_request));
	}

	for (test = type_tags.begin(); test != type_tags.end(); test++) {
		if (token == *test) {
			ret = true;
		}
	}

	return ret;
}


/**
 \brief Converte um string em seu identificador de tipo de pacote.
 \param token String a ser processada.
 \returns Identificador enumerado de tipo de pacote.
 */
tags::packet_type txt_reader::type_from_string(const std::string& 
    line) const
{
	tags::packet_type   tp = tags::not_typed;

	for (int i  = tags::tp_begin_session; i < tags::tp_dead_end; i++)   {
		if (line ==
				tags::packet_tags::type(static_cast<tags::packet_type>(i)))  {
			tp  = static_cast<tags::packet_type>(i);
		}
	}

	return tp;
}


/**
 \brief Qualifica uma linha, identificando seu conteúdo.
 \returns Valor enumerado identificando o conteúdo da linha.
 */
in_phase txt_reader::qualify_token(const std::string& line, 
    in_phase dfl_phase) const
{
	in_phase					ret = dfl_phase;

	if (line == tags::packet_tags::begin_tag()) {
		ret = in_start;
	}
	else if (valid_packet_type(line))	{
		ret = in_request;
	}
	else if (line.substr(0, SESSION_ID_TAG_LEN) == SESSION_ID_TAG)  {
		ret = in_ssid;
	}
	else if (line.substr(0, COMMAND_ID_TAG_LEN) == COMMAND_ID_TAG)  {
		ret = in_cmdid;
	}
	else if (line == tags::packet_tags::end_tag())  {
		ret = in_stop;
	}
	else if (line.find(PARAM_SEPARATOR) != std::string::npos) {
		if (dfl_phase != in_text)
			ret = in_param;
	}
	else if (line.find(PARAM_MSG_BEGIN) != std::string::npos)	{
		ret = in_text;
	}

	return ret;
}


/**
 \brief Carrega o conteúdo de pacote serializado para um objeto.
 \details Esquadrinha uma arquivo de texto, alimentando uma instância de objeto
 referênciada pelo argumento passado.
 \param pkt Referência a um objeto request_packet.
 */
void txt_reader::read_packet(request_packet* pkt)
{
	std::string			line,
						msg,
						rqt_tag,
						session_id,
						command_id,
						param_id,
						param_val;
	std::ios::iostate	excp = m_in->exceptions();
	in_phase			pkt_phase,
						prc_phase = in_idle,
						dfl_phase = in_unknown;
	bool				has_stop  = false;
	size_t				pos;
	pkt_param			text;

	do  {
		m_in->exceptions(std::ios::failbit | std::ios::badbit);
		try {
			std::getline(*m_in, line);
		}
		catch (const std::ios::failure& err) {
			if (!m_in->eof())	{
				m_in->exceptions(excp);
				throw;
			}
		}

		pkt_phase = qualify_token(line, dfl_phase);

		switch (prc_phase)  {

			// Máquina de estados "dormente".
			// Único estado passível de transição, é o início de coleta
			// ----------------------------------------------------------------
			case in_idle		:
				if (pkt_phase != in_start)  {
					m_in->exceptions(excp);
					msg = MSG_EXPECTED_START + std::string(" != ") + line;
					throw rqt_reader_exp(msg);
				}
				else
					prc_phase = in_request;
				break;
				

			// Aguardando tag de fim da coleta.
			// Nenhuma transição de estado possível.
			// ----------------------------------------------------------------
			case in_stop		:
				if (pkt_phase != in_stop)   {
					m_in->exceptions(excp);
					msg = MSG_EXPECTED_STOP + std::string(" != ") + line;
					throw rqt_reader_exp(msg);
				}
				has_stop = true;
				break;
				

			// Coleta de identificador de tipo de pacote obrigatória.
			// Se o pacote solicitar a abertura de uma nova sessão, ou o reset
			// da atual, a única transição de estado possível, é o fim da
			// coleta.
			// Qualquer outro estado tem como estado seguinte, a coleta do
			// identificador da sessão.
			// ----------------------------------------------------------------
			case in_request		:
				if (pkt_phase != in_request)	{
					m_in->exceptions(excp);
					msg = MSG_EXPECTED_REQUEST + std::string(" != ") + line;
					throw rqt_reader_exp(msg);
				}
				
				// O "contexto" pode ser descartado, pois "qualify_token"
				// garante que apenas pacotes "_request" estejam sendo
				// processados.
				// No entando, é preciso extrair a tag de "tipo", para realizar
				// o setup do pacote.
				pos = line.rfind(tags::packet_tags::context(tags::ctx_request));
				if (pos != std::string::npos)   {
					rqt_tag = line.substr(0, pos);
					try {
						pkt->type(type_from_string(rqt_tag));
					}
					catch (...) {
						m_in->exceptions(excp);
						throw;
					}
				}
				else	{
					m_in->exceptions(excp);
					msg = MSG_REQUEST_NOT_FOUND + std::string(" (") +
							line + std::string(")");
					throw rqt_reader_exp(msg);
				}

				// Seleção do próximo estado
				if (
(rqt_tag != tags::packet_tags::type(tags::tp_begin_session)) &&
(rqt_tag != tags::packet_tags::type(tags::tp_reset)) &&
(rqt_tag != tags::packet_tags::type(tags::tp_status))
				    )  {
					prc_phase = in_ssid;
				}
				else	{
					prc_phase = in_stop;
				}
				break;
				

			// Coleta do identificador de sessão, e atribuição da máquina de
			// estados, para aguardar um identificador de comando.
			// Aguardando identificador de sessão.
			// Uma vez coletado, as transições de estado variam de acordo com
			// o tipo da requisição.
			// Caso se trate de encerramento de sessão, o estado seguinte
			// deverá ser o fim da coleta. Qualquer outra requisição, implica
			// na coleta de uma tag de comando.
			// ----------------------------------------------------------------
			case in_ssid		:
				if (pkt_phase != in_ssid)   {
					m_in->exceptions(excp);
					msg = MSG_EXPECTED_SSID + std::string(" != ") + line;
					throw rqt_reader_exp(msg);
				}

				try {
					session_id = line.substr(SESSION_ID_TAG_LEN);
					pkt->session(session_id);
				}
				catch (...) {
					m_in->exceptions(excp);
					throw;
				}

				// Seleção do próximo estado
				if (rqt_tag == tags::packet_tags::type(tags::tp_end_session))  {
					prc_phase = in_stop;
				}
				else	{
					prc_phase = in_cmdid;
				}
				break;

				
			// Aguardando identificador de comando.
			// A única transição de estado possível, é para coleta de 
			// parâmetros.
			// ----------------------------------------------------------------
			case in_cmdid		:
				if (pkt_phase != in_cmdid)  {
					m_in->exceptions(excp);
					msg = MSG_EXPECTED_CMDID + std::string(" != ") + line;
					throw rqt_reader_exp(msg);
				}
				try {
					command_id = line.substr(COMMAND_ID_TAG_LEN);
					pkt->id(command_id);
				}
				catch (...) {
					m_in->exceptions(excp);
					throw;
				}
				prc_phase = in_param;
				break;

				
			// Coleta de parâmetros. OPCIONAL.
			// Neste estado pode ocorrer transição para coleta de "texto",
			// do fim da coleta, e para o próximo parâmetro.
			// ----------------------------------------------------------------
			case in_param		:
				if ((pkt_phase != in_param) && (pkt_phase != in_stop) &&
						(pkt_phase != in_text))  {
					m_in->exceptions(excp);
					msg = MSG_UNEXPECTED_TOKEN + std::string(": ") + line;
					throw rqt_reader_exp(msg);
				}
				// Não há (mais) parâmetro(s)
				else if (pkt_phase == in_stop)  {
					prc_phase = in_stop;
					has_stop = true;
				}
				// Coleta do parâmetro
				else if (pkt_phase == in_param) {
					pos = line.find(PARAM_SEPARATOR);
					if (pos != std::string::npos)   {
						try {
							param_id = line.substr(0, pos);
							param_val = line.substr(pos+1);
							pkt->add_param(param_id, param_val);
							xdebug_log(std::string("Param: ") + 
							    dquote(param_id.c_str()) + 
							    dquote(param_val.c_str()));
						}
						catch (...) {
							throw;
						}
					}
					else	{
						m_in->exceptions(excp);
						msg = MSG_PARAM_MALFORMED + std::string(": ") = line;
						throw rqt_reader_exp(msg);
					}
				}
				// O parâmetro é uma mensagem de texto
				else if (pkt_phase == in_text)  {
					text.name("mensagem");
					dfl_phase = in_text;
					prc_phase = in_text;
				}
				break;

			// Coleta mensagem de texto.
			// Uma vez concluída, sinaliza a transição para o estado de coleta
			// de novo parâmetro.
			// ----------------------------------------------------------------
			case in_text		:
				if (pkt_phase != in_text)   {
					m_in->exceptions(excp);
					msg = MSG_MESSAGE_MALFORMED + std::string(": ") + line;
					throw rqt_reader_exp(msg);
				}
				if (line.find(PARAM_MSG_END) != std::string::npos)   {
					dfl_phase = in_unknown;
					prc_phase = in_param;
					pkt->add_param(text);
				}
				else	{
					try {
						text.add_value(line);
					}
					catch (...) {
						throw;
					}
				}
				break;

			// Token desconhecido. Não se enquadra na expectativa do processador
			// de estados.
			// ----------------------------------------------------------------
			default				:
				msg = MSG_UNKNOWN_TOKEN + std::string(": ") + line;
				throw rqt_reader_exp(msg);
		}
	} while ((!m_in->eof()) && (!has_stop));

	m_in->exceptions(excp);
}
