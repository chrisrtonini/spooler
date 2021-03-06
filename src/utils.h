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

#ifndef _UTILS_H_
#define	_UTILS_H_


#include <algorithm>
#include <string>
#include <sstream>

#include "log.h"


/**
 \fn ltrim
 \brief Elimina os espaços em branco à esquerda numa string.
 \param s String a ser verificada/modificada.
 \returns Instância modificada da string.
 */
static inline std::string& ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
	    std::not1(std::ptr_fun<int, int>(std::isspace))));
	
	return s;
}


/**
 \fn rtrim
 \brief Elimina os espaços em branco à direita numa string.
 \param s String a ser verificada/modificada.
 \returns Instância modificada da string.
 */
static inline std::string& rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), 
	    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	
	return s;
}


/**
 \fn trim
 \brief Elimina espaços em branco à direita e à esquerda numa string.
 \param s String a ser verificada/modificada.
 \returns Instância modificada da string.
 */
static inline std::string& trim(std::string& s)
{
	return ltrim(rtrim(s));
}


/**
 \fn upper_string
 \brief Converte caracteres de string para maiúsculos.
 \param str Referência de string a ser convertido.
 */
static inline void upper_string(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}


/**
 \template into_string
 \brief Converte dados em std::string.
 \param vlr Valor a ser convertido.
 \returns String contendo o valor convertido.
 */
template<class T>
std::string into_string(T vlr)
{
	std::stringstream   str;
	std::string			txt;

	str << vlr;
	txt = str.str();

	return txt;
}


/**
 \fn dquote
 \brief Envolve string em aspas duplas
 \param txt String.
 \returns String.
 */
static inline std::string dquote(const char* txt)
{
	return (std::string("\"") + txt + std::string("\""));
}


/**
 \fn error_log
 \brief Registra mensagem de erro no log.
 */
static inline void error_log(const std::string& msg)
{
	log::logger::err_msg(msg);
}


/**
 \fn warn_log
 \brief Registra mensagem de advertência no log.
 */
static inline void warn_log(const std::string& msg)
{
	log::logger::warn_msg(msg);
}


/**
 \fn info_log
 \brief Registra mensagem de informação no log.
 */
static inline void info_log(const std::string& msg)
{
	log::logger::info_msg(msg);
}


/**
 \fn debug_log
 \brief Registra mensagem de depuração no log.
 */
static inline void debug_log(const std::string& msg)
{
	log::logger::debug_msg(msg);
}


/**
 \fn xdebug_log
 \brief Registra mensagem de dados (para depuração) no log.
 */
static inline void xdebug_log(const std::string& msg)
{
	log::logger::xdbg_msg(msg);
}


/**
 \fn make_datafile
 \brief Cria nome para arquivo auxiliar, no diretório '/tmp/',
 com um prefixo determinado, e tendo o nome composto pelo PID.
 */
static inline std::string make_datafile(const std::string& prefix)
{
	std::string fpath = "/tmp/";

	fpath += prefix;
	fpath += into_string(getpid());

	return fpath;
}


/**
 \fn str_iequals
 \brief Verifica se duas strings são iguais. Comparação não sensível ao caso.
 \param str1 String a ser comparada.
 \param str2 Outra string a ser comparada.
 \returns Indicador lógico do resultado do teste.
 */
static inline bool str_iequals(const std::string str1, const std::string str2)
{
	std::string	cpy1 = str1,
				cpy2 = str2;
	int			len1 = 0,
				len2 = 0;

	upper_string(trim(cpy1));
	len1 = cpy1.length();
	upper_string(trim(cpy2));
	len2 = cpy2.length();

	if ((len1 != len2) && ((len1 == 0) || (len2 == 0)))
		return false;

	bool test = (cpy1 == cpy2);

	xdebug_log(std::string("str_iequals(\"") + cpy1 + 
	    std::string("\", \"") + cpy2 + std::string("\") = ") +
	    std::string((test ? "TRUE" : "FALSE")));

	return (test);
}


#endif
