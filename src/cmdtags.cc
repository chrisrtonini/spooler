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

#include "cmdtags.h"

/**
 \static Converte um identificador enumerado em uma sequência de caracteres.
 \returns Vetor constante de caracteres, contendo a tag desejada.
 \throws cmdtag_exp Quando o indice de tag estiver fora da faixa.
 */
const char* cmd::command_tags::tag_string(cmd_idx id)
	throw(cmdtag_exp)
{
	int idx = static_cast<int>(id);
	
	if ((idx > NONE) && (idx < OUT_OF_BOUND))   {
		return cmd_id[idx];
	}

	throw cmdtag_exp("Indice de tag invalido");
}


/**
 \static Converte tag em identificador enumerado.
 \returns Valor enumerado correspondente à tag passada como argumento.
 \throws cmdtag_exp Quando não for possível realizar a conversão.
 */
cmd::cmd_idx cmd::command_tags::tag_enum(const std::string& tag_str)
	throw(cmdtag_exp)
{
	int i;
	cmd_idx ret = NONE;

	for (i = NONE; i < OUT_OF_BOUND; i++)   {
		std::string tmp = cmd_id[i];
		if (tmp == tag_str) {
			ret = static_cast<cmd_idx>(i);
		}
	}

	if (ret == NONE)	{
		std::string msg = "Tag invalida \"";
		msg += tag_str;
		msg += "\"!";
		throw cmdtag_exp(msg);
	}

	return ret;
}
