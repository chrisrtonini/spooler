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

#ifndef _COLLECTOR_H_
#define _COLLECTOR_H_


#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "fs.h"
#include "loopctrl.h"
#include "excp.h"
#include "cmdtags.h"
#include "datapair.h"
#include "log.h"
#include "utils.h"

#define COLLECTOR_RUNNING   0
#define COLLECTOR_STOP		1
#define COLLECTOR_ABORT		2

#define MSG_COLLECTOR_ABORT "Coletor interrompido."


/**
 \class collector
 \brief Executa o laço de coleta de pacotes de comando.
 */
class collector
{
	public:
						collector(const lp_struct* ctx);
		virtual			~collector(void) throw(collector_exp)   {};
		virtual void	setup(const datapair& cfg)		{};
		int				run(bool bckground = true);
		bool			keep_running(void);
		virtual int		accept(void)							{};
		virtual int		callback(void)							{};

	private:
		bool			m_running;
		lp_struct*		m_ctx;
};

#endif // _COLLECTOR_H_
