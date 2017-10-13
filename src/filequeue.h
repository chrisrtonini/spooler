/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * spooler
 * Copyright (C) Chris Robert Tonini 2016 <chrisrtonini@gmail.com>
 * 
 * spooler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * spooler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FILEQUEUE_H_
#define _FILEQUEUE_H_


#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>

#include <queue>

#include "fs.h"
#include "excp.h"
#include "log.h"
#include "utils.h"


#define NOTIFY_EVENT_SIZE			sizeof(struct inotify_event)
#define NOTIFY_EVENT_QUEUE_LEN		NOTIFY_EVENT_SIZE*2048


/**
 \enum scan_type
 \brief Define modos de esquadrinhamento de diretório.
 */
typedef enum
{
	dir_scan = 0,
	event_scan
} scan_type;


/**
 \class file_queue
 \brief Classe para monitorar um diretório, montando uma fila de arquivos.
 \details Os arquivos são selecionados de acordo com o prefixo informado. E o
 método de coleta é incremental, iniciando por varredura de diretório, de modo
 a enfileirar os já existentes, e posteriormente, através de monitoramento dos
 eventos de gravação.
 */
class file_queue : public std::queue<std::string>
{
	public:
								file_queue(const std::string& path,
								           const std::string& prefix,
								           const long timeout = 0l);
								~file_queue(void);
		std::string				scan_files(void);
		void					set_mask(uint32_t new_mask);
		uint32_t				get_mask(void);
		void					set_timeout(const long timeout);
		long					get_timeout(void);

	protected:
		bool					directory_scan(void);
		bool					kernel_scan(void);
		void					stop_monitoring(void);
		void					start_monitoring(void);
		void					reset_monitoring(void);

	private:
		scan_type				m_scan_type;
		std::string				m_path;
		std::string				m_prefix;  // TODO: no futuro, testar 'regex'
		long					m_timeout;
		int						m_fd;
		int						m_wd;
		uint32_t				m_mask;
};

#endif // _FILEQUEUE_H_
