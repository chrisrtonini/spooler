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

#ifndef _SESSION_H_
#define _SESSION_H_

#include <iostream>
#include <string>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "utils.h"


/**
 \class session
 \brief Implementa a lógica de geração e testes de identificadores de sessão.
 */
class session
{
	public:
							session(void);
							session(const std::string& ssid);
		const std::string&	session_id(void) const;
		const std::string&	generate(void);
		bool				match(const std::string& ssid) const;
		bool				operator==(const std::string& ssid);
		void				reset(void);
		bool				created(void);
		
	private:
		std::string		m_session_id;
};

#endif // _SESSION_H_
