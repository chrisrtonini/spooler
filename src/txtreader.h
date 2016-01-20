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

#ifndef _TXTREADER_H_
#define _TXTREADER_H_



#include <iostream>
#include <fstream>
#include <string>

#include "rqtreader.hpp"
#include "txttags.h"

#define MSG_UNKNOWN_TOKEN		"Token nao reconhecido"
#define MSG_INVALID_CONTEXT		"Contexto invalido"
#define MSG_UNEXPECTED_TOKEN	"Token nao esperado"
#define MSG_EXPECTED_START		"Esperada tag de inicio"
#define MSG_EXPECTED_STOP		"Esperada tag de fim"
#define MSG_EXPECTED_REQUEST	"Esperada tag de tipo"
#define MSG_EXPECTED_SSID		"Esperada identificacao de sessao"
#define MSG_EXPECTED_CMDID		"Esperada identificacao de comando"
#define MSG_REQUEST_NOT_FOUND   "Sufixo _request nao encontrado"
#define MSG_PARAM_MALFORMED		"Definicao invalida de parametro"
#define MSG_MESSAGE_MALFORMED   "Mensagem mal formada"


/*
 \enum in_phase
 \brief Estados para o motor de coleta e parsing de pacotes de comando.
 */
enum in_phase   {
	in_unknown = -1,
	in_idle,			// 0
	in_start,			// 1
	in_request,			// 2
	in_ssid,			// 3
	in_cmdid,			// 4
	in_param,			// 5
	in_text,			// 6
	in_stop				// 7
};


/**
 \class txt_reader
 \brief Loader de pacotes de comando, em formato texto
 */
class txt_reader : public rqt_reader<std::istream*>
{
	public:
							txt_reader(std::istream* in);
		void				read_packet(request_packet* pkt);

	protected:
		in_phase			qualify_token(const std::string& line,
										in_phase dfl_phase = in_unknown) const;
		bool				valid_packet_type(const std::string& token) const;
		tags::packet_type   type_from_string(const std::string& token) const;
};

#endif // _TXTREADER_H_
