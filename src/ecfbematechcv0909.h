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

#ifndef _ECFBEMATECHCV0909_H_
#define _ECFBEMATECHCV0909_H_

#include <stdlib.h>

#include "ecfbematech.h"
#include "bemafi_cv0909.h"

#define BEMATECH_TOTAL_TOTCNF_CV0909	30

#define BEMATECH_SIZE_FPGTO_CV0909		15
#define BEMATECH_SIZE_TOTCNF_CV0909		15
#define BEMATECH_SIZE_RELGER_CV0909		15

#define BEMATECH_TXT_SANGRIA_CV0909		"SANGRIA"
#define BEMATECH_TXT_SUPRIMENTO_CV0909	"FUNDO DE TROCO"


class ecf_bematech_cv0909: public ecf_bematech 
{
	public:
				ecf_bematech_cv0909(void);
		
		short	get_totalizador_cnf(const std::string& tot);
		short	get_id_forma_pgto(const std::string& fpgto);
		short   get_prox_forma_pgto(void);
		
		void	set_forma_pgto(const std::string& forma,
								const std::string& vinc);
		void	set_totalizador(const std::string& descr, 
								const std::string& tipo);
		void	set_rel_gerencial(const std::string& titulo);

	protected:

	private:
};

#endif // _ECFBEMATECHCV0909_H_
