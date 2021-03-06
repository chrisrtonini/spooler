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

#ifndef _TXTCOLLECTOR_H_
#define _TXTCOLLECTOR_H_


#include "collector.h"
#include "datapair.h"
#include "pktrequest.h"
#include "pktresponse.h"
#include "ecfbroker.h"
#include "session.h"
#include "filequeue.h"


#define APP_ECF_DRIVER		"ecf"
#define APP_ECF_SPECIFIC	"ecf_settings"

#define APP_LOG_FILE		"log_file"
#define APP_LOG_LEVEL		"log_level"
#define APP_LOG_VERBOSE		"log_verbose"
#define APP_RUN_BCKGROUND   "run_bckground"

#define APP_WORK_DIR		"work_dir"
#define APP_CMD_PREFIX		"cmd_id"

#define APP_RET_PREFIX		"ret_id"
#define APP_LCK_SUFFIX		"lck_suffix"
#define APP_TMP_RET_PREFIX  "tmp_ret"

#define APP_SCAN_DELAY		"scan_delay"
#define APP_DELIVER_DELAY   "deliver_delay"


/**
 \class txt_collector
 \brief Coletor de pacotes de comando, a partir de arquivos de texto,
 de acordo com o formato legado, utilizado pela SGI.
 */
class txt_collector: public collector 
{
	public:
							txt_collector(const lp_struct* ctx)
								: collector(ctx)						{};
							~txt_collector(void) throw(collector_exp)   {};
		void				setup(const datapair& cfg,
									ecf_broker* broker,
									void (*writer)(const std::string& path,
												const response_packet* pkt));
		int					accept(void);
		int					callback(void);

	private:
		datapair*			m_cfg;
		request_packet*		m_packet;
		ecf_broker*			m_broker;
		std::string			m_cmdfile;
		session				m_session;
		bool				m_failure;
		bool				m_mute;
		file_queue*			m_dir;
		void				(*m_writer)(const std::string& path,
										const response_packet* pkt);
};


#endif // _TXTCOLLECTOR_H_
