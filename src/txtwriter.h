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

#ifndef _TXTWRITER_H_
#define _TXTWRITER_H_

#include <iostream>
#include <fstream>

#include "rspwriter.hpp"
#include "txttags.h"
#include "vrstags.h"


/**
 \class txt_writer
 \brief Serializador de pacotes de resposta, para formato texto.
 */
class txt_writer: public rsp_writer<std::ostream*>
{
	public:
				txt_writer(std::ostream* out);
		void	write_packet(const response_packet& pkt);
};

#endif // _TXTWRITER_H_
