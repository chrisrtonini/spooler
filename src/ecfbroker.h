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

#ifndef _ECFBROKER_H_
#define _ECFBROKER_H_

#include <iostream>
#include <fstream>

#include "excp.h"
#include "ecf.h"
#include "pktrequest.h"
#include "pktresponse.h"
#include "rspwriter.hpp"
#include "cmdtags.h"
#include "aliquota.h"
#include "log.h"

#define BROKER_NO_PARAM			"Nenhum parametro encontrado."
#define BROKER_PARAM_NOT_FOUND  "Parametro nao encontrado."


/**
 \class ecf_broker
 \brief Processa as requisições ao ECF, submetendo-as apropriadamente.
 */
class ecf_broker
{
	public:
				ecf_broker(ecf* obj);
		void	trade(const request_packet& in, response_packet* out) NO_THROW;

	protected:
		void	get_nr_loja(const request_packet& in, response_packet* out);
		void	get_nr_serie(const request_packet& in, response_packet* out);
		void	get_marca(const request_packet& in, response_packet* out);
		void	get_modelo(const request_packet& in, response_packet* out);
		void	get_nr_usuario(const request_packet& in, response_packet* out);
		void	get_nr_serie_cript(const request_packet& in,
										response_packet* out);
		void	get_nr_caixa(const request_packet& in, response_packet* out);
		void	get_firmware(const request_packet& in, response_packet* out);
		void	get_data_atual(const request_packet& in, response_packet* out);
		void	get_hora_atual(const request_packet& in, response_packet* out);
		void	get_data_mvto(const request_packet& in, response_packet* out);
		void	get_data_ult_red(const request_packet& in, 
										response_packet* out);
		void	get_hora_ult_red(const request_packet& in, 
										response_packet* out);
		void	get_status_ecf(const request_packet& in, response_packet* out);
		void	get_nr_cupom(const request_packet& in, response_packet* out);
		void	get_id_forma_pgto(const request_packet& in, 
										response_packet* out);
		void	get_totalizador_cnf(const request_packet& in, 
										response_packet* out);
		void	get_prox_totalizador(const request_packet& in, 
										response_packet* out);
		void	set_forma_pgto(const request_packet& in, response_packet* out);
		void	set_aliquota(const request_packet& in, response_packet* out);
		void	set_totalizador(const request_packet& in, response_packet* out);
		void	snd_abre_cupom(const request_packet& in, response_packet* out);
		void	snd_cancela_cupom(const request_packet& in, 
										response_packet* out);
		void	snd_vende_item(const request_packet& in, response_packet* out);
		void	snd_inicia_fechamento(const request_packet& in, 
										response_packet* out);
		void	snd_pagamento(const request_packet& in, response_packet* out);
		void	snd_fecha_cupom(const request_packet& in, response_packet* out);
		void	snd_nfiscal_vinc(const request_packet& in, 
										response_packet* out);
		void	snd_msg_vinc(const request_packet& in, response_packet* out);
		void	snd_fecha_vinc(const request_packet& in, response_packet* out);
		void	snd_nfiscal_nvinc(const request_packet& in, 
										response_packet* out);
		void	get_max_col(const request_packet& in, response_packet* out);
		void	snd_abregaveta(const request_packet& in, response_packet* out);
		void	reducao_z(const request_packet& in, response_packet* out);
		void	get_cupom_aberto(const request_packet& in, 
										response_packet* out);
		void	set_arredondamento(const request_packet& in, 
										response_packet* out);
		void	get_imprime_cheques(const request_packet& in, 
									response_packet* out);
		void	get_nr_reducoes(const request_packet& in, response_packet* out);
		void	dados_ult_reducao(const request_packet& in, 
										response_packet* out);
		void	get_cnt_reinicio(const request_packet& in, 
										response_packet* out);
		void	memoria_fiscal(const request_packet& in, response_packet* out);
		void	leitura_x(const request_packet& in, response_packet* out);
		void	set_moeda_singular(const request_packet& in, 
										response_packet* out);
		void	set_moeda_plural(const request_packet& in, 
										response_packet* out);
		void	snd_imprime_cheque(const request_packet& in, 
										response_packet* out);
		void	snd_abre_rel_gerencial(const request_packet& in, 
										response_packet* out);
		void	snd_txt_gerencial(const request_packet& in, 
										response_packet* out);
		void	snd_fecha_rel_gerencial(const request_packet& in, 
										response_packet* out);
		void	set_rel_gerencial(const request_packet& in, 
										response_packet* out);
		void	snd_aciona_guilhotina(const request_packet& in, 
										response_packet* out);
		void	get_id_rel_gerencial(const request_packet& in, 
										response_packet* out);
		void	snd_corte_auto(const request_packet& in, response_packet* out);
		void	get_istermica(const request_packet& in, response_packet* out);
		void	get_ispafcompliant(const request_packet& in, 
										response_packet* out);
		void	set_tipo_corte(const request_packet& in, response_packet* out);
		void	grandetotal(const request_packet& in, response_packet* out);
		void	matchconf(const request_packet& in, response_packet* out);
		void	snd_sangria(const request_packet& in, response_packet* out);
		void	snd_suprimento(const request_packet& in, response_packet* out);
		void	set_horarioverao(const request_packet& in, 
										response_packet* out);
		void	get_ishorarioverao(const request_packet& in, 
										response_packet* out);
		void	downloadmfd(const request_packet& in, response_packet* out);
		void	get_poucopapel(const request_packet& in, response_packet* out);
		void	get_cnt_cupomfiscal(const request_packet& in, 
										response_packet* out);
		void	get_cnt_naofiscal(const request_packet& in, 
										response_packet* out);
		void	get_cnt_creddeb(const request_packet& in, response_packet* out);
		void	get_cnt_relgerencial(const request_packet& in, 
										response_packet* out);
		void	snd_cancela_item(const request_packet& in, 
										response_packet* out);
		void	snd_abrerecibo(const request_packet& in, response_packet* out);
		void	snd_abreccd(const request_packet& in, response_packet* out);
		void	snd_segundaviaccd(const request_packet& in, 
										response_packet* out);
		void	snd_reimprimeccd(const request_packet& in, 
										response_packet* out);
		void	snd_cancelarecibo(const request_packet& in, 
										response_packet* out);
		void	snd_cancelaccd(const request_packet& in, response_packet* out);
		void	snd_regnaofiscal(const request_packet& in, 
										response_packet* out);
		void	snd_lctonaofiscal(const request_packet& in, 
										response_packet* out);
		void	snd_iniciafechamentonaofiscal(const request_packet& in,
										response_packet* out);
		void	snd_fecharecibo(const request_packet& in, response_packet* out);
		void	snd_fechaccd(const request_packet& in, response_packet* out);
		void	snd_cancnaofiscalpos(const request_packet& in, 
										response_packet* out);
		void	snd_abre_gerencial(const request_packet& in, 
										response_packet* out);
		void	get_naofisc_aberto(const request_packet& in, 
										response_packet* out);
		void	get_tab_aliq(const request_packet& in,
										response_packet* out);
		void	get_tab_relger(const request_packet& in,
										response_packet* out);
		void	get_tab_totcnf(const request_packet& in,
										response_packet* out);
		void	get_tab_fpgto(const request_packet& in,
										response_packet* out);
		void	convertemfd(const request_packet& in, response_packet* out);
		
	private:
		ecf*			 m_ecf;
		request_packet	 m_cmd;
		response_packet	 m_rsp;
		const pkt_param  get_param_by_name(const std::string& name,
										const request_packet& pkt);

		std::string		 get_param_value_by_name(const std::string& name,
										const request_packet& pkt);
};

#endif // _ECFBROKER_H_
