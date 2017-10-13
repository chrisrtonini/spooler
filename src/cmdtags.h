/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef _CMDTAGS_H_
#define _CMDTAGS_H_


#include <string>
#include <exception>


#include "excp.h"

#define CMDTAG_UNKNOWN_CMD	"Comando nao reconhecido."


/**
 \namespace cmd
 \brief Definições específicas para manipulação de identificadores de comandos.
 */
namespace cmd
{

	/**
	 \enum cmd::cmd_idx
	 \brief Identificadores dos comandos da interface do spooler.
	 */
	enum cmd_idx
	{
		NONE = 0,
		GET_NR_LOJA,
		GET_NR_SERIE,
		GET_MARCA,
		GET_MODELO,
		GET_NR_USUARIO,
		GET_NR_SERIE_CRIPT,
		GET_NR_CAIXA,
		GET_FIRMWARE,
		GET_DATA_ATUAL,
		GET_HORA_ATUAL,
		GET_DATA_MVTO,
		GET_DATA_ULT_RED,
		GET_HORA_ULT_RED,
		GET_STATUS_ECF,
		GET_NR_CUPOM,
		GET_ID_FORMA_PGTO,
		GET_TOTALIZADOR_CNF,
		GET_PROX_TOTALIZADOR,
		SET_FORMA_PGTO,
		SET_ALIQUOTA,
		SET_TOTALIZADOR,
		SND_ABRE_CUPOM,
		SND_CANCELA_CUPOM,
		SND_VENDE_ITEM,
		SND_INICIA_FECHAMENTO,
		SND_PAGAMENTO,
		SND_FECHA_CUPOM,
		SND_NFISCAL_VINC,
		SND_MSG_VINC,
		SND_FECHA_VINC,
		SND_NFISCAL_NVINC,
		GET_MAX_COL,
		SND_ABREGAVETA,
		REDUCAO_Z,
		GET_CUPOM_ABERTO,
		SET_ARREDONDAMENTO,
		GET_IMPRIME_CHEQUES,
		GET_NR_REDUCOES,
		DADOS_ULT_REDUCAO,
		GET_CNT_REINICIO,
		MEMORIA_FISCAL,
		LEITURA_X,
		SET_MOEDA_SINGULAR,
		SET_MOEDA_PLURAL,
		SND_IMPRIME_CHEQUE,
		SND_ABRE_REL_GERENCIAL,
		SND_TXT_GERENCIAL,
		SND_FECHA_REL_GERENCIAL,
		SET_REL_GERENCIAL,
		SND_ACIONA_GUILHOTINA,
		GET_ID_REL_GERENCIAL,
		SND_CORTE_AUTO,
		GET_ISTERMICA,
		GET_ISPAFCOMPLIANT,
		SET_TIPO_CORTE,
		GRANDETOTAL,
		MATCHCONF,
		SND_SANGRIA,
		SND_SUPRIMENTO,
		SET_HORARIOVERAO,
		GET_ISHORARIOVERAO,
		DOWNLOADMFD,
		GET_POUCOPAPEL,
		GET_CNT_CUPOMFISCAL,
		GET_CNT_NAOFISCAL,
		GET_CNT_CREDDEB,
		GET_CNT_RELGERENCIAL,
		SND_CANCELA_ITEM,
		SND_ABRERECIBO,
		SND_ABRECCD,
		SND_SEGUNDAVIACCD,
		SND_REIMPRIMECCD,
		SND_CANCELARECIBO,
		SND_CANCELACCD,
		SND_REGNAOFISCAL,
		SND_LCTONAOFISCAL,
		SND_INICIAFECHAMENTONAOFISCAL,
		SND_FECHARECIBO,
		SND_FECHACCD,
		SND_CANCNAOFISCALPOS,
		SND_ABRE_GERENCIAL,
		GET_NAOFISC_ABERTO,
		GET_TAB_ALIQ,
		GET_TAB_RELGER,
		GET_TAB_TOTCNF,
		GET_TAB_FPGTO,
		CONVERTEMFD,
		OUT_OF_BOUND
	};


	/**
	 \static cmd_id[]
	 \brief Tags para formatação de pacotes de comando e resposta.
	 */
	static const char* cmd_id[] = {
		"",
		"get_nr_loja",
		"get_nr_serie",
		"get_marca",
		"get_modelo",
		"get_numusuario",
		"get_nr_serie_cript",
		"get_nr_caixa",
		"get_firmware",
		"get_data_atual",
		"get_hora_atual",
		"get_data_mvto",
		"get_data_ult_red",
		"get_hora_ult_red",
		"get_status_ecf",
		"get_nr_cupom",
		"get_id_forma_pgto",
		"get_totalizador_cnf",
		"get_prox_totalizador",
		"set_forma_pgto",
		"set_aliquota",
		"set_totalizador",
		"snd_abre_cupom",
		"snd_cancela_cupom",
		"snd_vende_item",
		"snd_inicia_fechamento",
		"snd_pagamento",
		"snd_fecha_cupom",
		"snd_nfiscal_vinc",
		"snd_msg_vinc",
		"snd_fecha_vinc",
		"snd_nfiscal_nvinc",
		"get_max_col",
		"snd_abregaveta",
		"reducao_z",
		"get_cupom_aberto",
		"set_arredondamento",
		"get_imprime_cheques",
		"get_nr_reducoes",
		"dados_ult_reducao",
		"get_cnt_reinicio",
		"memoria_fiscal",
		"leitura_x",
		"set_moeda_singular",
		"set_moeda_plural",
		"snd_imprime_cheque",
		"snd_abre_rel_gerencial",
		"snd_txt_gerencial",
		"snd_fecha_rel_gerencial",
		"set_rel_gerencial",
		"snd_aciona_guilhotina",
		"get_id_rel_gerencial",
		"snd_corte_auto",
		"get_istermica",
		"get_ispafcompliant",
		"set_tipo_corte",
		"grandetotal",
		"matchconf",
		"snd_sangria",
		"snd_suprimento",
		"set_horarioverao",
		"get_ishorarioverao",
		"downloadmfd",
		"get_poucopapel",
		"get_cnt_cupomfiscal",
		"get_cnt_naofiscal",
		"get_cnt_creddeb",
		"get_cnt_relgerencial",
		"snd_cancela_item",
		"snd_abrerecibo",
		"snd_abreccd",
		"snd_segundaviaccd",
		"snd_reimprimeccd",
		"snd_cancelarecibo",
		"snd_cancelaccd",
		"snd_regnaofiscal",
		"snd_lctonaofiscal",
		"snd_iniciafechamentonaofiscal",
		"snd_fecharecibo",
		"snd_fechaccd",
		"snd_cancnaofiscalpos",
		"snd_abre_gerencial",
		"get_naofisc_aberto",
		"get_tab_aliquotas",
		"get_tab_relgerenciais",
		"get_tab_totcnf",
		"get_tab_formaspgto",
		"convertemfd",
		""
	};


	/**
	 \class command_tags
	 \brief Classe estática para realizar conversões entre identificadores
	 numéricos (enumerados) e tags de pacotes.
	 */
	class command_tags	{
		public:
			static const char* 	tag_string(cmd_idx id)
										throw(cmdtag_exp);
			static cmd_idx		tag_enum(const std::string& tag_str)
										throw(cmdtag_exp);
	};

};


#endif
