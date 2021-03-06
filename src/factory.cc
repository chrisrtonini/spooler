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

#include "factory.h"


/**
 \brief Instancia biblioteca de ECF.
 \param vendor Identificação do fabricante. Atua como seletor da biblioteca a
 ser instanciada.
 \param cfg_file Path do arquivo de configurações do driver, caso haja.
 \returns Ponteiro para instância da biblioteca de ECF.
 */
ecf* factory::create_ecf(ECF_VENDOR vendor, const std::string& cfg_file)
{
	if (vendor == BEMATECH) {
		return new ecf_bematech(cfg_file);
	}
	else if (vendor == BEMATECH_CV0909) {
		return new ecf_bematech_cv0909(cfg_file);
	}
}


/**
 \brief Obtém o nome do fabricante, associado à identificação.
 \param vendor Identificação do fabricante.
 \returns String com o nome do fabricante.
 */
const std::string factory::vendor_name(ECF_VENDOR vendor)
{
	if (vendor == BEMATECH) {
		return VENDOR_ID_BEMATECH;
	}
	else if (vendor == BEMATECH_CV0909)  {
		return VENDOR_ID_BEMATECH_CV0909;
	}
}


/**
 \brief Obtém a identificação, a partir do nome do fabricante.
 \param vendor Nome do fabricante.
 \returns Identificador (enumerado) do fabricante.
 */
const ECF_VENDOR factory::vendor_id(const std::string& vendor)
{
	if (vendor == VENDOR_ID_BEMATECH)   {
		return BEMATECH;
	}
	else if (vendor == VENDOR_ID_BEMATECH_CV0909)   {
		return BEMATECH_CV0909;
	}
}
