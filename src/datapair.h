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

#ifndef _DATAPAIR_H_
#define _DATAPAIR_H_


#include <map>
#include <string>

#include "excp.h"


#define UNDEF_DATAPAIR		"Chave nao definida"
#define INVALID_RAW_PAIR	"Erro de parsing"

#define DEFAULT_DELIM		"="


/**
 \typedef std::map<std::string, std::string> data_node
 \brief Elemento de dados, composto por chave e valor.
 */
typedef std::map<std::string, std::string> data_node;


/**
 \class datapair
 \brief Abstração de lista de nodos de dados (chave + valor)
 */
class datapair : public data_node
{
	public:
							datapair(void);
		const std::string&  get_value_by_key(const std::string& node_key)
								throw(datapair_exp);
		const std::string&  get_value_by_idx(int node_idx)
								throw(datapair_exp);
		const std::string&  get_key(int node_idx)
								throw(datapair_exp);
		void				set_delim(const std::string& delim) NO_THROW;
		const std::string&  get_delim(void) const NO_THROW;
		const datapair&		operator()(const std::string& delim) NO_THROW;
		int					assign(const std::string& node_key,
									const std::string& node_value)
								NO_THROW;
		int					from_raw_data(const std::string& raw_data,
									const std::string& delim = DEFAULT_DELIM)
								throw(datapair_exp);
		const datapair&		operator<<(const std::string& raw_data)
								throw(datapair_exp);
		bool				defined(const std::string& node_key)
								NO_THROW;
		const datapair&		operator=(const datapair&) NO_THROW;
		const datapair&		operator=(const datapair*) NO_THROW;
		const datapair&		copy(const datapair*) NO_THROW;

	private:
		std::string			m_delim;
};

#endif // _DATAPAIR_H_
