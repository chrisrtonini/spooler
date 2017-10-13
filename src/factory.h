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

#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "ecf.h"
#include "ecfbematech.h"
#include "ecfbematechcv0909.h"


/**
 \typedef ECF_VENDOR
 */
typedef enum	{
	NONE = 0,
	BEMATECH,
	BEMATECH_CV0909
} ECF_VENDOR;


#define VENDOR_ID_BEMATECH			"BEMATECH"
#define VENDOR_ID_BEMATECH_CV0909   "BEMATECH_CV0909"


/**
 \class factory
 \brief Instancia classes de 'wrappers' para bibliotecas de fabricantes de ECF.
 */
class factory
{
	public:
		static ecf*					create_ecf(ECF_VENDOR vendor, 
							                      const std::string& cfg_file);
		static const std::string	vendor_name(ECF_VENDOR vendor);
		static const ECF_VENDOR		vendor_id(const std::string& vendor);
};


#endif // _FACTORY_H_
