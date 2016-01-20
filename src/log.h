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

#ifndef _LOG_H_
#define _LOG_H_


#include <iostream>
#include <ctime>
#include <ios>
#include <string>

#include <string.h>

#include "excp.h"


/**
 \namespace log
 \brief Engloba definições para geração e formatação de mensagens de log.
 */
namespace log
{
	/**
	 \typedef level
	 \brief Nível de gravidade de erro. Sendo:
		- 0 = error : Identifica mensagem de erro ocorrido.
		- 1 = warn  : Identifica mensagem de advertência.
		- 2 = info  : Mensagem contendo informações da execução.
		- 3 = debug : Mensagem contendo informações para depuração.
		- 4 = xdbg  : Mensagem contendo dados de contexto de depuração.
	 */
	typedef enum
	{
		error = 0,
		warn,
		info,
		debug,
		xdbg
	} level;


	/**
	 \static log_level Define nível mínimo de log para registro.
	 */
	static level			log_level = log::debug;

	/**
	 \static verbose Especifica se as mensagens de log devem ecoar também 
	 na tela.
	 */
	static bool				verbose = false;

	/**
	 \static output_stream Canal para escrita de log.
	 */
	static std::ostream*	output_stream = NULL;

	/**
	 \class logger Classe contendo métodos para registro de logs.
	 */
	class logger
	{
		public:
			
			static void			setup(std::ostream* out, level min_log = debug,
										bool verb = false) NO_THROW;
			static void			write(const std::string& msg,
										level lvl = error);
			static void			err_msg(const std::string& msg);
			static void			warn_msg(const std::string& msg);
			static void			info_msg(const std::string& msg);
			static void			debug_msg(const std::string& msg);
			static void			xdbg_msg(const std::string& msg);

		protected:
			static std::string  fmt_date(void) NO_THROW;
			static std::string	fmt_level(level lvl) NO_THROW;
	};

}

#endif // _LOG_H_
