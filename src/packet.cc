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

#include "packet.h"


/**
 \brief Construtor
 */
packet::packet(void)
{
	try {
		m_params.clear();
		m_idx = m_params.begin();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Destruidor
 */
packet::~packet(void)
{
	dispose_params();
}


/**
 \brief Elimina instâncias dinâmicas de parâmetros
 */
void packet::dispose_params(void)
{
//	for (m_idx = m_params.begin(); m_idx != m_params.end(); m_idx++)	{
//		delete &(*m_idx);
//	}
}


/**
 \brief Adiciona parâmetro ao pacote.
 \param param Objeto pkt_param.
 */
void packet::add_param(const pkt_param& param)
{
	try {
		pkt_param* tmp = new pkt_param(param);
		
		m_params.push_back(*tmp);
		m_idx++;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona parâmetro ao pacote.
 \param param Ponteiro para um objeto pkt_param.
 */
void packet::add_param(const pkt_param* param)
{
	try {
		pkt_param*  tmp = new pkt_param(*param);

		m_params.push_back(*tmp);
		m_idx++;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona parâmetro ao pacote.
 \param key Chave para o novo parâmetro.
 \param val Valor do novo parâmetro.
 */
void packet::add_param(const std::string& key, const std::string& val)
{
	try {
		pkt_param*  tmp = new pkt_param(key, val);

		m_params.push_back(*tmp);
		m_idx++;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o primeiro parâmetro do pacote.
 \details Além de obter o primeiro parâmetro, também prepara controles internos
 para obtenção do parâmetro subsequente, caso exista.
 \returns Instancia de pkt_param.
 \throws packet_exp Caso não haja nenhum parâmetro contido pelo pacote.
 */
const pkt_param& packet::first_param(void) throw(packet_exp)
{
	if (!m_params.empty())  {
		m_idx = m_params.begin();
		m_idx++;
		return m_params.front();
	}

	throw packet_exp("Nenhum parametro definido");
}


/**
 \brief Obtém o próximo parâmetro do pacote.
 \details Retorna o parâmetro seguinte, da lista contida pelo pacote, e
 incrementa as propriedades (internas) do objeto, de modo a apontar para o
 próximo parâmetro.
 \returns Instância de pkt_param
 \throws packet_exp Caso seja atingido o fim da lista de parâmetros.
 */
const pkt_param& packet::next_param(void) throw(packet_exp)
{
	if ((!m_params.empty()) && (m_idx != m_params.end()))   {
		return m_params[std::distance(m_params.begin(), m_idx++)];
	}

	throw packet_exp("Nao ha mais parametros definidos");
}


/**
 \brief Define o tipo do pacote.
 \details O tipo do pacote, afeta o tratamento sintático do mesmo, pois há
 particularidades de acordo com o estado do spooler, para abertura de sessão,
 submissão de comandos e encerramento de sessão.
 */
void packet::type(tags::packet_type tp)
{
	try {
		m_type = tp;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o tipo do pacote.
 \returns Tipo enumerado indicando o tipo do pacote.
 */
const tags::packet_type& packet::type(void) const NO_THROW
{
	return m_type;
}


/**
 \brief Define o identificador de sessão para o pacote.
 \param ssid Identificador de sessão.
 */
void packet::session(const std::string& ssid)
{
	try {
		m_session = ssid;
	}
	catch (...) {
		throw;
	}
}
 

/**
 \brief Obtém o identificador de sessão do pacote.
 \returns String contendo o identificador de sessão.
 */
const std::string& packet::session(void) const NO_THROW
{
	return m_session;
}


/**
 \brief Define o identificador de comando para o pacote.
 \param id String contendo a identificação de comando, válida para o spooler.
 */
void packet::id(const std::string& id)
{
	try {
		m_id = id;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o identificador do comando do pacote.
 \returns String contendo a identificação de comando.
 */
const std::string& packet::id(void) const NO_THROW
{
	return m_id;
}
