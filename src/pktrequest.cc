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

#include "pktrequest.h"


/**
 \brief Construtor
 */
request_packet::request_packet(void) : packet()
{
	try {
		m_context = tags::ctx_request;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor
 \param ssid String contendo a identificação da sessão.
 */
request_packet::request_packet(const std::string& ssid) : packet()
{
	try {
		m_context = tags::ctx_request;
		session(ssid);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor
 \param ssid String contendo a identificação da sessão.
 \param cmd_id String contendo a identificação de comando.
 */
request_packet::request_packet(const std::string& ssid,
    const std::string& cmd_id) : packet()
{
	try {
		m_context = tags::ctx_request;
		session(ssid);
		id(cmd_id);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Destruidor.
 */
request_packet::~request_packet(void)
{
}


/**
 \brief Estabelece a identificação do comando.
 \param cmd_id String contendo a identificação de comando.
 */
void request_packet::request(const std::string& cmd_id)
{
	try {
		id(cmd_id);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a identificação de comando.
 \returns String contendo a identificação de comando.
 */
const std::string& request_packet::request(void) const NO_THROW
{
	return id();
}
