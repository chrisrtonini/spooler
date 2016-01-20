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

#ifndef _PKTTAGS_H_
#define _PKTTAGS_H_


#include <string>

#include "excp.h"


/**
 \namespace tags
 \brief Definições específicas para manipulação de tags
 de pacotes
 */
namespace tags
{

	/**
	 \enum tags::packet_type
	 \brief Identificador de tipo de pacote.
	 */
	typedef enum
	{
		not_typed = 0,
		tp_begin_session,
		tp_execut,
		tp_end_session,
		tp_status,
		tp_reset,
		tp_dead_end
	} packet_type;


	/**
	 \enum tags::packet_context
	 \brief Identificador de contexto de pacote (request/response)
	 */
	typedef enum
	{
		no_context = 0,
		ctx_request,
		ctx_response
	} packet_context;


	/**
	 \class packet_tags
	 \brief Classe contendo métodos estáticos para tratamento de tags.
	 \details Os métodos cobrem as tarefas de conversão dos tipos enumerados
	 em strings, para uso em pacotes.
	 */
	class packet_tags
	{
		public:
			static const std::string  begin_tag(void) NO_THROW;
			static const std::string  end_tag(void) NO_THROW;
			static const std::string  eof_tag(void) NO_THROW;
			static const std::string  context(packet_context ctx) NO_THROW;
			static const std::string  type(packet_type tp) NO_THROW;
	};

};


#endif // _PKTTAGS_H_
