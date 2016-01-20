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

#ifndef _LOOPCTRL_H_
#define _LOOPCTRL_H_


#include <string>
#include <exception>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


#include "excp.h"


/**
 \typedef lp_struct
 \brief Estrutura para conter dados compartilhados, para controle de loops
 */
typedef struct  {
	int		handler;
	bool	keep_running;
	key_t   proc_id;
} lp_struct;


/**
 \class loopctrl
 \brief Classe estática para manipulação de estrutura de controle de loops.
 */
class loopctrl
{
	public:
		static void			init(lp_struct* ctx)
								throw(loopctrl_exp);
		static void			to_true(lp_struct ctx) 
								throw(loopctrl_exp);
		static void			to_false(lp_struct ctx) 
								throw(loopctrl_exp);
		static bool			running(lp_struct ctx) 
								throw(loopctrl_exp);
		static void			destroy(lp_struct ctx) 
								throw(loopctrl_exp);
		static std::string	err_shmget(int code) NO_THROW;
		static std::string	err_shmat(int code) NO_THROW;
		static std::string	err_shmdt(int code) NO_THROW;
		static std::string	err_shmctl(int code) NO_THROW;

	private:
		static const int	m_perm = 0666;
		static const size_t m_size = sizeof(lp_struct);
};

#endif // _LOOPCTRL_H_
