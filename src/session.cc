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

#include "session.h"


/**
 \brief Construtor.
 */
session::session(void)
{
	m_session_id = "";
}


/**
 \brief Construtor.
 \param ssid String contendo identificador de sessão.
 */
session::session(const std::string& ssid)
{
	m_session_id = ssid;
}


/**
 \brief Obtém o identificador de sessão associado à instância de objeto.
 \returns String.
 */
const std::string& session::session_id(void) const
{
	return m_session_id;
}


/**
 \brief Gera um novo identificador de sessão.
 \details Caso o objeto já esteja associado a um identificador, retorna este.
 \returns String.
 */
const std::string& session::generate(void)
{
	if (!created()) {
		srandom(time(NULL));
		long valor = random();

		char buff[20];
		memset(buff, 0, sizeof(buff));
		snprintf(buff, 19, "%19x", valor);
		size_t len = strlen(buff);
		for (size_t i = 0; i < 19; i++)
			buff[i] = (isspace(buff[i]) ? '0' : buff[i]);
		strfry(buff);

		m_session_id = "sgi_id_";
		m_session_id += buff;
	}

	return m_session_id;
}


/**
 \brief Testa um identificador, comparando-o ao do objeto.
 \returns Valor lógico.
 */
bool session::match(const std::string& ssid) const
{
	return str_iequals(ssid, session_id());
}


/**
 \brief Testa um identificador, comparando-o ao do objeto.
 \returns Valor lógico.
 */
bool session::operator==(const std::string& ssid)
{
	return match(ssid);
}


/**
 \brief Destrói o identificador contido pelo objeto.
 */
void session::reset(void)
{
	m_session_id = "";
}


/**
 \brief Verifica se há um identificador associado ao objeto.
 \returns Valor lógico.
 */
bool session::created(void)
{
	return (!m_session_id.empty());
}
