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

#include "pktresponse.h"


/**
 \brief Construtor.
 */
response_packet::response_packet(void) : packet()
{
	try {
		m_context = tags::ctx_response;
		m_retcode = 0;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor.
 \param ssid String contendo a identificação de sessão.
 */
response_packet::response_packet(const std::string& ssid) : packet()
{
	try {
		m_context = tags::ctx_response;
		session(ssid);
		m_retcode = 0;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor.
 \param ssid String contendo a identificação de sessão.
 \param cmd_id String contendo a identificação de comando.
 */
response_packet::response_packet(const std::string& ssid,
    const std::string& cmd_id) : packet()
{
	try {
		m_context = tags::ctx_response;
		session(ssid);
		id(cmd_id);
		m_retcode = 0;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Destruidor.
 */
response_packet::~response_packet(void)
{
}


/**
 \brief Define o identificador de comando.
 \param cmd_id String contendo a identificação de comando.
 */
void response_packet::response(const std::string& cmd_id)
{
	try {
		id(cmd_id);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o identificador de comando.
 \returns String contendo a identificação de comando.
 */
const std::string& response_packet::response(void) const NO_THROW
{
	return id();
}


/**
 \brief Obtém a tag identificador de contexto
 \returns String contendo "_response"
 */
std::string response_packet::context_tag(void) const NO_THROW
{
	return tags::packet_tags::context(m_context);
}


/**
 \brief Atribui código de retorno ao pacote.
 \returns Valor inteiro representando código de retorno.
 */
void response_packet::ret_code(int code) NO_THROW
{
	m_retcode = code;
}


/**
 \brief Obme código de retorno do pacote.
 \returns Valor inteiro contendo o código de retorno.
 */
int response_packet::ret_code(void) const NO_THROW
{
	return m_retcode;
}
