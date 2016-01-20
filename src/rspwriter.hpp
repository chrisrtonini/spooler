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

#ifndef _RSPWRITER_HPP_
#define _RSPWRITER_HPP_


#include "pktresponse.h"


/**
 \class rsp_writer
 \brief Template para escrita de pacote de comando para um objeto e/ou descritor
 de destino, especificado.
 \tparam T Identificador de tipo (ou classe) do objeto de output (ostream, etc)
 */
template <class T>
class rsp_writer
{
	public:
						rsp_writer(T out);
		virtual void	write_packet(const response_packet& pkt) {};

	protected:
		T				m_out;
};


/**
 \brief Construtor.
 \tparam T Classe da instância de objeto utilizada para enviar o output.
 \param out Instância de objeto para output.
 */
template <class T>
rsp_writer<T>::rsp_writer(T out)
{
	try {
		m_out = out;
	}
	catch (...) {
		throw;
	}
}

#endif // _RSPWRITER_HPP_
