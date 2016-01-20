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

#include "datapair.h"


/**
 \brief Construtor.
 */
datapair::datapair(void) : data_node()
{
	m_delim = DEFAULT_DELIM;
}


/**
 \brief Obtém o valor associado a uma chave.
 \param node_key String contendo a chave de identificação do nodo.
 \returns String contendo o valor associado.
 \throws datapair_exp Caso não haja nenhum nodo identificado pela chave passada.
 */
const std::string& datapair::get_value_by_key(const std::string& node_key)
	throw(datapair_exp)
{
	data_node::iterator i;

	for (i = begin(); i != end(); i++)  {
		if (i->first == node_key) {
			return i->second;
			break;
		}
	}

	throw datapair_exp(UNDEF_DATAPAIR);
}


/**
 \brief Obtém o valor associado a um índice.
 \param node_idx Valor inteiro correspondente ao índice do valor desejado.
 \returns String contendo o valor associado.
 \throws datapair_exp Caso não haja nenhum nodo correspondente ao índice 
 passado.
 */
const std::string& datapair::get_value_by_idx(int node_idx)
	throw(datapair_exp)
{
	int idx = 0;
	data_node::iterator i;

	for (i = begin(); i != end(); i++)  {
		if (node_idx == ++idx)  {
			return i->second;
			break;
		}
	}

	throw datapair_exp(UNDEF_DATAPAIR);
}


/**
 \brief Obtém o conteúdo da chave correspondente a um índice.
 \param node_idx Valor inteiro correspondente a um índice.
 \returns String contendo a chave identificadora.
 \throws datapair_exp Caso não haja nenhum nodo correspodente ao índice
 passado.
 */
const std::string& datapair::get_key(int node_idx)
	throw(datapair_exp)
{
	int idx = 0;
	data_node::iterator i;

	for (i = begin(); i != end(); i++)  {
		if (node_idx == ++idx)  {
			return i->first;
			break;
		}
	}

	throw datapair_exp(UNDEF_DATAPAIR);
}


/**
 \brief Define o delimitador padrão.
 \param delim String contendo o delimitador.
 */
void datapair::set_delim(const std::string& delim) NO_THROW
{
	m_delim = delim;
}


/**
 \brief Obtém o delimitador padrão.
 \returns String contendo o delimitador.
 */
const std::string& datapair::get_delim(void) const NO_THROW
{
	return m_delim;
}


/**
 \brief Define o delimitador padrão.
 \param delim String contendo o delimitador.
 \returns Instância do objeto datapair modificada.
 */
const datapair& datapair::operator()(const std::string& delim) NO_THROW
{
	set_delim(delim);

	return *this;
}


/**
 \brief Atribui valor a um nodo de dados.
 \details Caso já exista um nodo identificado pelo chave passada, seu valor
 correspondente será substituído pelo argumento passado. Caso contrário, um
 novo nodo será adicionado, contendo respectivamente, a chave e valor passados.
 \param node_key String contendo a chave identificadora.
 \param node_value String contendo o valor associado.
 \returns Valor inteiro correspondente ao índice do nodo alterado/inserido.
 */
int datapair::assign(const std::string& node_key, const std::string& node_value)
	NO_THROW
{
	int idx = 0;
	data_node::iterator i;

	if (!defined(node_key)) {
		insert(std::pair<std::string, std::string>(node_key, node_value));
		idx = size();
	}
	else	{
		for (i = begin(); i!= end(); i++)   {
			idx++;
			if (node_key == i->first)   {
				i->second = node_value;
				break;
			}
		}
	}

	return idx;
}


/**
 \brief Importa um nodo a partir de um string delimitado.
 \details A string deve conter os tokens correspodentes à chave e ao valor,
 separados por um delimitador, possibilitando ao parser distinguí-los.
 Uma vez obtidos os tokens, a atribuição é feita através da invocação do
 método assign.
 \param raw_data String a ser processado.
 \param delim String separador (entre "chave" e "valor")
 \returns Valor inteiro correspondente ao índice do nodo inserido ou modificado.
 \throws datapair_exp Caso ocorra erro de parsing do argumento passado.
 */
int datapair::from_raw_data(const std::string& raw_data, 
    const std::string& delim) throw(datapair_exp)
{
	int idx = -1;
	unsigned int cut = raw_data.find(delim);

	if (cut != std::string::npos)   {
		std::string node_key = raw_data.substr(0, cut);
		std::string node_value = raw_data.substr(cut+1);

		idx = assign(node_key, node_value);
	}
	else	{
		throw datapair_exp(INVALID_RAW_PAIR);
	}

	return idx;
}


/**
 \brief Importa um nodo a partir de um string delimitado.
 \details Utiliza o delimitador padrão, necessitando portanto, receber um
 único argumento.
 \param raw_data String contendo os dados de "chave" e "valor" devidamente
 delimitados.
 \returns Instância modificada do objeto.
 \throws datapair_exp Caso não seja possível realizar o parsing da string.
 */
const datapair& datapair::operator<<(const std::string& raw_data) 
	throw(datapair_exp)
{
	try {
		assign(raw_data, m_delim);
	}
	catch (...) {
		throw;
	}

	return *this;
}


/**
 \brief Verifica a presença de um nodo, através de sua chave.
 \param node_key String contendo a chave identificadora.
 \returns Valor lógico indicando se já existe (ou não) a referida chave.
 */
bool datapair::defined(const std::string& node_key) NO_THROW
{
	data_node::iterator i;

	for (i = begin(); i != end(); i++)  {
		if (node_key == i->first)   {
			return true;
			break;
		}
	}

	return false;
}


/**
 \brief Atribui ao objeto, o conteúdo de outro.
 \details O conteúdo do objeto de destino, caso contenha dados, será perdido.
 \param obj Objeto datapair, fonte dos dados para cópia.
 */
const datapair& datapair::operator=(const datapair& obj) NO_THROW
{
	return copy(&obj);
}


/**
 \brief Atribui ao objeto, o conteúdo da referência passada.
 \details O conteúdo do objeto de destino, caso contenha dados, será perdido.
 \param obj Referência a um objeto datapair, fonte dos dados para cópia.
 */
const datapair& datapair::operator=(const datapair* obj) NO_THROW
{
	return copy(obj);
}


/**
 \brief Atribui ao objeto, o conteúdo da referência passada.
 \details O conteúdo do objeto de destino, caso contenha dados, será perdido.
 \param obj Referência a um objeto datapair, fonte dos dados para cópia.
 */
const datapair& datapair::copy(const datapair* obj) NO_THROW
{
	data_node::iterator i;

	clear();

	for (i = const_cast<datapair*>(obj)->begin(); i != obj->end(); i++)	{
		assign(i->first, i->second);
	}

	return *this;
}
