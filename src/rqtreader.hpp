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

#ifndef _RQTREADER_HPP_
#define _RQTREADER_HPP_


#include "pktrequest.h"


/**
 \class rqt_reader
 \brief Template de coleta de pacote, a partir de um descritor ou stream.
 \tparam T Identificador de tipo (ou classe) do objeto de input (ostream, etc)
 */
template <class T>
class rqt_reader
{
	public:
						rqt_reader(T in);
		virtual void	read_packet(const request_packet* pkt) {};

	protected:
		T				m_in;
};


/**
 \brief Construtor.
 \tparam T Classe da instância de objeto utilizada coletar o input.
 \param out Instância de objeto para input.
 */
template <class T>
rqt_reader<T>::rqt_reader(T in)
{
	try {
		m_in = in;
	}
	catch (...) {
		throw;
	}
}

#endif // _RQTREADER_HPP_
