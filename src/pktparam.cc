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

#include "pktparam.h"


/**
 \brief Construtor
 */
pkt_param::pkt_param(void)
{
	try {
		m_val.clear();
		m_idx = m_val.begin();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor de cópia.
 \param pkt Objeto pkt_param (origem dos dados).
 */
pkt_param::pkt_param(const pkt_param& pkt)
{
	try {
		copy(const_cast<pkt_param*>(&pkt));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor
 \param name String contendo o nome (identificador) do parâmetro.
 */
pkt_param::pkt_param(const std::string& name)
{
	try {
		m_val.clear();
		m_idx = m_val.begin();
		m_name = name;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor
 \param name String contendo o nome (identificador) do parâmetro.
 \param val String contendo o valor contido pelo parâmetro.
 */
pkt_param::pkt_param(const std::string& name, const std::string& val)
{
	try {
		m_val.clear();
		m_name = name;
		value(val);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Destruidor
 */
pkt_param::~pkt_param(void)
{
	// Nada a fazer...
}


/**
 \brief Atribui nome (identificador) ao parâmetro.
 \param txt String contendo o nome para o parâmetro.
 */
void pkt_param::name(const std::string& txt)
{
	try {
		m_name = txt;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o nome definido para o parâmetro.
 \returns String contendo o nome do parâmetro.
 */
const std::string& pkt_param::name(void) const NO_THROW
{
	return m_name;
}


/**
 \brief Atribui valor ao parâmetro.
 \details Caso já tenha sido, préviamente, definido um valor para este parâmetro,
 este será substituído. Independentemente de o valor prévio ser único, ou um
 conjunto de valores.
 \param value String contendo o valor para o parâmetro.
 */
void pkt_param::value(const std::string& txt)
{
	try {
		if (!m_val.empty()) {
			m_val.clear();
		}
		add_value(txt);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona um valor ao conteúdo do objeto.
 \details Acrescenta o argumento passado à lista de valores do objeto. Caso não
 tenha ocorrido nenhuma atribuição até então, este será o primeiro valor.
 \param txt String contendo o valor a ser adicionado ao parâmetro.
 */
void pkt_param::add_value(const std::string& txt)
{
	try {
		m_val.push_back(txt);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtem o valor contido pelo parâmetro.
 \details Retorna o valor associado ao parâmetro, caso este seja único. Caso
 se tratar de uma lista, retorna sempre o primeiro valor, e prepara as 
 propriedades do objeto para que seja percorrida a lista de valores, 
 subsequentemente.
 \returns String contendo o valor do parâmetro.
 \throws pkt_param_exp Caso não haja nenhum valor definido.
 */
const std::string& pkt_param::value(void) throw(pkt_param_exp)
{
	if (!m_val.empty()) {
		m_idx = m_val.begin();
		m_idx++;
		return m_val.front();
	}

	throw pkt_param_exp("Nenhum valor definido");
}


/**
 \brief Obtém o próximo valor (da lista) associado ao parâmetro.
 \details Obtem o valor apontado, e incrementa o apontador (interno) do objeto,
 de modo a preparar a próxima chamada deste método.
 \returns String contendo um dos valores associado ao parâmetro.
 \throws pkt_param_exp Quando for atingido o final da lista de valores.
 */
const std::string& pkt_param::next_value(void) throw(pkt_param_exp)
{
	if ((!m_val.empty()) && (m_idx != m_val.end()))	{
//		return m_val[std::distance(m_val.begin(), m_idx++)];
		return *m_idx++;
	}

	throw pkt_param_exp("Nao ha mais valores definidos");
}


/**
 \brief Obtém o número total de valores associados ao parâmetro.
 \returns Número inteiro, indicando o número de valores contidos pelo objeto.
 */
int pkt_param::num_lines(void) const NO_THROW
{
	return m_val.size();
}


/**
 \brief Atribui o conteúdo de outro objeto ao atual.
 \param obj Instância de objeto pkt_param.
 \returns Objeto pkt_param modificado.
 */
const pkt_param& pkt_param::operator=(const pkt_param& obj) NO_THROW
{
	copy(const_cast<pkt_param*>(&obj));

	return *this;
}


/**
 \brief Atribui o conteúdo de outro objeto ao atual.
 \param obj Referencia a objeto pkt_param.
 \returns Objeto pkt_param modificado.
 */
const pkt_param& pkt_param::operator=(const pkt_param* obj) NO_THROW
{
	copy(const_cast<pkt_param*>(obj));

	return *this;
}


/**
 \brief Copia o conteúdo de um parâmetro para o objeto.
 \param obj Referência a objeto pkt_param.
 \returns Objeto pkt_param modificado.
 */
const pkt_param& pkt_param::copy(const pkt_param* obj) NO_THROW
{
	int lines = obj->num_lines();
	name(obj->name());
	for (int i = 0; i < lines; i++) {
		if (i == 0) {
			try {
				value((const_cast<pkt_param*>(obj))->value());
			}
			catch (pkt_param_exp& e) {
				// Ignorar a sinalização de nenhum valor definido
			}
		}
		else	{
			try {
				add_value((const_cast<pkt_param*>(obj))->next_value());
			}
			catch (pkt_param_exp& e) {
				// Ignorar a sinalização de que não há mais valores
			}
		}
	}

	return *this;
}
