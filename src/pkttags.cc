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

#include "pkttags.h"


/**
 \static Obtém a tag de inicio de pacote.
 \returns "<ecf_sgi_begin>"
 */
const std::string tags::packet_tags::begin_tag(void) NO_THROW
{
	return "<ecf_sgi_begin>";
}


/**
 \static Obtém a tag para fim de pacote.
 \returns "<ecf_sgi_end>"
 */
const std::string tags::packet_tags::end_tag(void) NO_THROW
{
	return "<ecf_sgi_end>";
}


/**
 \statis Obtém a tag de EOF
 \returns "<eof>"
 */
const std::string tags::packet_tags::eof_tag(void) NO_THROW
{
	return "<eof>";
}


/**
 \static Obtém a string de identificação de contexto de pacote.
 \details Converte o tipo enumerado em string.
 */
const std::string tags::packet_tags::context(tags::packet_context ctx) NO_THROW
{
	std::string txt;

	switch (ctx)
	{
		case ctx_request	:
			txt = "_request";
			break;
		case ctx_response   :
			txt = "_response";
			break;
		default						:
			txt = "";
	}

	return txt;
}


/**
 \static Obtém a string de identificação de tipo de pacote.
 \details Converte o tipo enumerado em string.
 */
const std::string tags::packet_tags::type(tags::packet_type tp) NO_THROW
{
	std::string txt;

	switch (tp)
	{
		case tp_begin_session   :
			txt = "ecf_begin_session";
			break;
		case tp_execut			:
			txt = "ecf_execute";
			break;
		case tp_end_session		:
			txt = "ecf_end_session";
			break;
		case tp_status			:
			txt = "ecf_status";
			break;
		case tp_reset			:
			txt = "ecf_reset";
			break;
		default					:
			txt = "";
	}

	return txt;
}
