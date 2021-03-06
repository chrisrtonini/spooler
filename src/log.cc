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

#include "log.h"


/**
 \brief Define parâmetros para registro de log.
 \param out Ponteiro para stream de output.
 \param min_log Nível mínimo de logs a registrar. Padrão: 'debug'.
 \param verb Valor lógico indicando se as mensagens devem também ser enviadas
 para 'std::cerr'.
 */
void log::logger::setup(std::ostream* out, level min_log, bool verb) NO_THROW
{
	output_stream = out;
	log_level = min_log;
	verbose = verb;
}


/**
 \brief Registra mensagem de log.
 \param msg Mensagem a ser registrada.
 \param lvl Nível de correspondente à mensagem. Padrão: 'error'.
 */
void log::logger::write(const std::string& msg, level lvl)
{
	if (!output_stream)
		return;

	if (lvl > log_level)
		return;
	
	try {
		std::string full_msg = fmt_date();
		full_msg += fmt_level(lvl);
		full_msg += " : ";
		full_msg += msg;

		if (verbose)
			std::cerr << full_msg << std::endl;

		(*output_stream) << full_msg << std::endl;
		output_stream->flush();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Registra mensagem de erro no log.
 \param msg Mensagem de erro.
 */
void log::logger::err_msg(const std::string& msg)
{
	write(msg, log::error);
}


/**
 \brief Registra mensagem de advertência no log.
 \param msg Mensagem de advertência.
 */
void log::logger::warn_msg(const std::string& msg)
{
	write(msg, log::warn);
}


/**
 \brief Registra mensagem de informação no log.
 \param msg Mensagem contendo informações.
 */
void log::logger::info_msg(const std::string& msg)
{
	write(msg, log::info);
}


/**
 \brief Registra mensagem de depuração no log.
 \param msg Mensagem de depuração.
 */
void log::logger::debug_msg(const std::string& msg)
{
	write(msg, log::debug);
}


/**
 \brief Registra dados de depuração no log.
 \param msg Mensagem contendo dados contextualizados para depuração.
 */
void log::logger::xdbg_msg(const std::string& msg)
{
	write(msg, log::xdbg);
}


/**
 \brief Formata data e hora para incorporar à mensagem.
 \returns Data e hora, no formato "[aaaa/mm/dd hh:mm:ss] "
 */
std::string log::logger::fmt_date(void) NO_THROW
{
	struct tm   rec_time;
	time_t		hw_time;
	char		time_buffer[30];
	std::string ret_str;

	hw_time = time(NULL);
	if (hw_time != -1)  {
		localtime_r(&hw_time, &rec_time);
		memset(time_buffer, 0, sizeof(time_buffer));
		strftime(time_buffer, sizeof(time_buffer), "[%Y/%m/%d %H:%M:%S] ",
		    &rec_time);
	}
	else	{
		strncpy(time_buffer, "[] ", sizeof(time_buffer));
	}

	ret_str = time_buffer;

	return ret_str;
}


/**
 \brief Converte nível de log em string.
 \param lvl Valor enumerado.
 \returns String que descreve o valor enumerado.
 */
std::string log::logger::fmt_level(level lvl) NO_THROW
{
	std::string fmt = "(";

	switch (lvl)
	{
		case error  :
			fmt += "ERRO";
			break;
		case warn   :
			fmt += "WARN";
			break;
		case info   :
			fmt += "INFO";
			break;
		case debug  :
			fmt += "DEBUG";
			break;
		case xdbg   :
			fmt += "DATA";
			break;
		default		:
			fmt += "UNDEF";
	};
	
	fmt += ")";
	
	return fmt;
}
