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

#ifndef _ALIQUOTA_H_
#define _ALIQUOTA_H_


#include <iostream>
#include <string>

#include <stdlib.h>
#include <monetary.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "excp.h"
#include "utils.h"
#include "log.h"

#define MSG_UNKNOWN_TYPE	"Tipo de aliquota nao reconhecido."
#define MSG_UNKNOWN_CST		"CST nao reconhecido."
#define MSG_PCENT_OUT_RANGE "Percentual fora da faixa."
#define MSG_MALFORMED_ALIQ  "Aliquota formatada incorretamente."
#define MSG_ALIQ_FMT_ERR	"Erro ao formatar aliquota."

#define FMT_ALIQ_INTL		"%!=0-#2.2i"


/**
 \namespace aliq_trib
 \brief Agrupa elementos para formatação/tratamento de aliquotas tributárias.
 */
namespace aliq_trib
{
	const unsigned char		TRIBUTADO_ISS		= 'S';
	const unsigned char		TRIBUTADO_ICMS		= 'T';
	const unsigned char		SUBST_TRIBUTARIA	= 'F';
	const unsigned char		NAO_INCIDENTE		= 'N';
	const unsigned char		ISENTO				= 'I';

	const int				FLAG_ISS = 1;
	const int				FLAG_ICMS = 0;


	/**
	 \typedef aliq_item
	 \brief Tipo de dado para alíquota tributária.
	 \details Compreende:
		- o seu índice na tabela de alíquotas do ECF
		- a sua definição de tipo (ICMS/ISS)
		- uma representação interna para CST
		- e o percentual da alíquota, propriamente dito.
	 */
	typedef struct  {
		short			indice;
		unsigned char   tipo;
		unsigned short  cst;
		float			percentual;
	} aliq_item;


	/**
	 \class aliquota
	 \brief Define operações sobre aliquotas tributárias.
	 */
	class aliquota
	{
		public:
							aliquota(void);
							aliquota(const std::string& str);
							aliquota(unsigned char tipo, unsigned short cst,
										float pcent);
							aliquota(const aliquota& obj);
			unsigned char   get_tipo(void) const;
			unsigned short  get_cst(void) const;
			float			get_percentual(void) const;
			std::string		get_raw(void) const;
			std::string		get_raw_numbers(void) const;
			std::string		get_serialized(void);
			void			set(const std::string& str);
			void			set(unsigned char tipo, unsigned short cst,
									float pcent)
								throw(aliquota_exp);
			aliquota&		operator=(const aliquota& obj);
			
		private:
			void			dump(void);
			unsigned char   m_tipo;
			unsigned short  m_cst;
			float			m_aliq;
	};

}

#endif // _ALIQUOTA_H_
