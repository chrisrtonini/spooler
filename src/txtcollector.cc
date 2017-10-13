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


#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <errno.h>
#include <stdlib.h>

#include "txtcollector.h"
#include "txtreader.h"
#include "ecfbroker.h"


/**
 \brief Configura o coletor de texto.
 \param cfg Objeto contendo dados estruturados de configuração.
 \param broker Ponteiro para objeto de troca de pacotes com ECF.
 \param writer Ponteiro para função de escrita de pacotes de retorno.
 */
void txt_collector::setup(const datapair& cfg,
    ecf_broker* broker, void (*writer)(const std::string& path, 
	    const response_packet* pkt))
{
	std::ifstream		dat;
	std::string			line;

	m_cfg = const_cast<datapair*>(&cfg);
	m_broker = broker;
	m_writer = writer;
	m_dir = new file_queue(m_cfg->get_value_by_key(APP_WORK_DIR).c_str(),
					m_cfg->get_value_by_key(APP_CMD_PREFIX),
					atol(m_cfg->get_value_by_key(APP_SCAN_DELAY).c_str()));
}


/**
 \brief Obtém um novo pacote e invoca callback.
 */
int txt_collector::accept(void)
{
	std::ifstream   cmd_stream;
	txt_reader*		cmd_reader;
	
	int				code = COLLECTOR_RUNNING;
	
	bool			got_one = false;
	std::string		tmp_name,
					tmp_fpath;
	std::string		prefix_name = m_cfg->get_value_by_key(APP_CMD_PREFIX);
	
	// Scan do diretório de trabalho, buscando
	// por arquivos de comando...
	try {
		debug_log(std::string("txt_collector::accept() Scanning ") +
		    m_cfg->get_value_by_key(APP_WORK_DIR));

		while ((!got_one) && (keep_running()))	{
			tmp_name = m_dir->scan_files();
			if (!tmp_name.empty())  {
				got_one = true;
				/*
				xdebug_log(std::string("txt_collector::accept() \"") +
				           tmp_name + std::string("\" selecionado"));
				*/
				break;
			}
		}

		if (!keep_running())
			code = COLLECTOR_STOP;
	}
	catch (std::exception& err) {
		error_log(std::string("txt_collector::accept() ") + err.what());
		code = COLLECTOR_ABORT;
		throw;
	}

	if (code == COLLECTOR_RUNNING)  {
		// Identificado um arquivo de comando, seu parsing deve alimentar
		// o conteúdo de um objeto de pacote de comando.
		try {
			m_failure = false;
			m_mute = false;
			tmp_fpath = m_cfg->get_value_by_key(APP_WORK_DIR);
			tmp_fpath += tmp_name;
			xdebug_log(std::string("txt_collector::accept() Processar") +
			    dquote(tmp_fpath.c_str()));
		
			cmd_stream.open(tmp_fpath.c_str(), std::ifstream::in);

			//debug_log("txt_collector::accept() Instanciar reader");
			cmd_reader = new txt_reader(&cmd_stream);

			//debug_log("txt_collector::accept() Carregar pacote");
			m_packet = new request_packet();
			cmd_reader->read_packet(m_packet);
		
			cmd_stream.close();
			fs::sys_unlink(tmp_fpath);
			m_cmdfile = tmp_fpath;
		}
		catch (cortar_papel_exp& err_cut) {
			m_failure = true;
			// Processamento indicado como 'mute', pois não deve
			// produzir nenhuma resposta.
			// TODO: criar parâmetro de configuração para determinar este
			//			comportamento.
			m_mute = true;
		}
		catch (...) {
			bool fim = cmd_stream.eof();
			if (cmd_stream.is_open())
				cmd_stream.close();

			fs::sys_unlink(tmp_fpath);
			m_cmdfile = tmp_fpath;

			if (!fim)   {
				m_failure = true;
			}
		}

		// Tendo carregado o pacote de comando, deve ser encaminhado
		// para o callback, onde a comunicação com o dispositivo (ECF) será
		// realizada, e o pacote de retorno produzido adequadamente.
		//debug_log("txt_collector::accept() Invocar 'callback()'");
		code = callback();
		delete m_packet;
	}

	m_cmdfile = "";

	debug_log(std::string("txt_collector::accept() Retornar ") +
	    dquote(into_string(code).c_str()));
	return code;
}


/**
 \brief Processa o pacote de comando recebido.
 */
int txt_collector::callback(void)
{
	int				code = COLLECTOR_RUNNING;
	response_packet pkt;

	pkt.type(m_packet->type());

	if (!m_failure) {
		switch (m_packet->type())
		{
			case tags::tp_execut		:
				//debug_log("txt_collector::callback() Comando de ECF");
				pkt.session(m_packet->session());
				pkt.id(m_packet->id());
				m_broker->trade(*m_packet, &pkt);
				break;
			case tags::tp_begin_session :
				//debug_log("txt_collector::callback() Abertura de sessao");
				if (!m_session.created())   {
					pkt.session(m_session.generate());
					xdebug_log(
					    std::string("txt_collector::callback() Nova sessao: ") +
					    dquote(pkt.session().c_str()));
					pkt.ret_code(PKT_RESPONSE_OK);
				}
				else	{
					warn_log("txt_collector::callback() ECF ja esta em uso");
					pkt.ret_code(-1);
					pkt.add_param("msg", "ECF ja esta em uso");
				}
				break;
			case tags::tp_end_session   :
				//debug_log("txt_collector::callback() Encerramento de sessao");
				pkt.session(m_packet->session());
				if (!m_session.created())  {
					warn_log(
					    "txt_collector::callback() Nao ha uma sessao aberta");
					pkt.ret_code(-4);
					pkt.add_param("msg", "Nao ha uma sessao aberta");
				}
				else	{
					if (m_session.match(m_packet->session()))   {
						//debug_log("txt_collector::callback() Eliminar sessao");
						m_session.reset();
						pkt.ret_code(PKT_RESPONSE_OK);
					}
					else	{
						warn_log(std::string("txt_collector::callback() ") +
						    std::string("Identificador de sessao invalido"));
						pkt.ret_code(-2);
						pkt.add_param("msg",
						    "Identificador de sessao invalido");
					}
				}
				break;
			case tags::tp_status		:
				//debug_log("txt_collector::callback() Status de sessao");
				if (m_session.created())	{
					warn_log("txt_collector::callback() Sessao ja esta aberta");
					pkt.session(m_session.session_id());
					pkt.ret_code(500);
					pkt.add_param("msg", "Sessao ja esta aberta");
				}
				else	{
					warn_log(
					    "txt_collector::callback() Nao ha uma sessao aberta");
					pkt.ret_code(501);
					pkt.add_param("msg", "Nao ha uma sessao aberta");
				}
				break;
			case tags::tp_reset			:
				//debug_log("txt_collector::callback() Reset de sessao");
				m_session.reset();
				pkt.session(m_packet->session());
				pkt.ret_code(PKT_RESPONSE_OK);
				break;
			default						:
				error_log("txt_collector::callback() Tipo de pacote invalido");
				pkt.ret_code(PKT_RESPONSE_ERR);
				pkt.add_param("msg", "Tipo de pacote invalido!");
		}
	}
	else	{
		error_log(
		    "txt_collector::callback() Erro aoprocessar pacote de comando.");
		pkt.session(m_session.session_id());
		pkt.ret_code(1);
		pkt.add_param("msg", "Erro ao processar pacote de comando.");
	}
	
	
	try {
		if (!m_mute)	{
			//debug_log("txt_collector::callback() Invocar 'writer()'");
			m_writer(m_cmdfile, const_cast<response_packet*>(&pkt));
		}
	}
	catch (...) {
		error_log(
		    "txt_collector::callback() Erro ao serializar pacote de resposta");
		code = COLLECTOR_ABORT;
	}

	xdebug_log(std::string("txt_collector::callback() Return: ") +
	    dquote(into_string(code).c_str()));
	return code;
}
