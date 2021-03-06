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

#include "ecfbroker.h"


/**
 \brief Construtor.
 \param obj Ponteiro para objeto ECF instanciado.
 */
ecf_broker::ecf_broker(ecf* obj)
{
	m_ecf = obj;
}


/**
 \brief Obtém um parâmetro (objeto), buscando por seu nome.
 \param name Nome do parâmetro desejado.
 \param pkt Pacote onde deve ser buscado o parâmetros.
 \returns Referência constante à instância do parâmetro.
 */
const pkt_param ecf_broker::get_param_by_name(const std::string& name,
    const request_packet& pkt)
{
	pkt_param   param;
	pkt_param   match;

	xdebug_log(std::string("ecf_broker::get_param_by_name(") +
	    name + std::string(")"));

	try {
		param = const_cast<request_packet&>(pkt).first_param();
		xdebug_log(std::string("Primeiro parametro: ") + 
		    dquote(param.name().c_str()));
	}
	catch (...) {
		warn_log(std::string("ecf_broker::get_param_by_name() ") +
		    dquote(BROKER_NO_PARAM));
		throw pkt_param_exp(BROKER_NO_PARAM);
	}

	if (str_iequals(param.name(), name))
		match = param;
	else	{
		do  {
			try {
				param = const_cast<request_packet&>(pkt).next_param();
				xdebug_log(std::string("Prox. parametro: ") +
				    dquote(param.name().c_str()));
			}
			catch (...) {
				if (!str_iequals(match.name(), name))   {
					warn_log(std::string("ecf_broker::get_param_by_name() ") +
					    dquote(BROKER_PARAM_NOT_FOUND));
					throw pkt_param_exp(BROKER_PARAM_NOT_FOUND);
				}
			}
			if (str_iequals(param.name(), name))
				match = param;
		} while (match.name().empty());
	}

	xdebug_log(
	    std::string("ecf_broker::get_param_by_name(...) Retorno [") +
	    match.name() + std::string("][") + match.value() + std::string("]"));

	return match;
}


/**
 \brief Obtem o valor de um parâmetro, identificado pelo nome passado.
 \param name Nome do parâmetro cujo valor se deseja.
 \param pkt Pacote no qual se encontra o parâmetro.
 \returns String contendo o valor do parâmetro nomeado.
 */
std::string ecf_broker::get_param_value_by_name(const std::string& name, 
    const request_packet& pkt)
{
	pkt_param   match = get_param_by_name(name, pkt);

	if (str_iequals(match.name(), name))	{
		xdebug_log(
		    std::string("ecf_broker::get_param_value_by_name(") + name +
		    std::string(") = ") + dquote(match.value().c_str()));
		return match.value();
	}
	else	{
		warn_log(std::string("Parametro ") +  dquote(name.c_str()) + 
		    std::string(" nao encontrado."));
		return "";
	}
}


/**
 \brief Processa o comando, sumetendo-o ao ECF e gerando a resposta.
 \param in Refência constante à instância do pacote de requisição.
 \param out Ponteiro para o objeto de pacote de resposta.
 */
void ecf_broker::trade(const request_packet& in, response_packet* out) NO_THROW
{
	try {
		xdebug_log(std::string("ecf_broker::trade() Comando ") +
		    dquote(in.id().c_str()));
		
		out->type(in.type());
		out->response(in.id());
		out->ret_code(PKT_RESPONSE_OK);
		
		cmd::cmd_idx one_cmd = cmd::command_tags::tag_enum(in.id());

		out->ret_code(PKT_RESPONSE_OK);

		switch (one_cmd)
		{
			case cmd::GET_NR_LOJA					:
				get_nr_loja(in, out);
				break;
			case cmd::GET_NR_SERIE					:
				get_nr_serie(in, out);
				break;
			case cmd::GET_MARCA						:
				get_marca(in, out);
				break;
			case cmd::GET_MODELO					:
				get_modelo(in, out);
				break;
			case cmd::GET_NR_USUARIO				:
				get_nr_usuario(in, out);
				break;
			case cmd::GET_NR_SERIE_CRIPT			:
				get_nr_serie_cript(in, out);
				break;
			case cmd::GET_NR_CAIXA					:
				get_nr_caixa(in, out);
				break;
			case cmd::GET_FIRMWARE					:
				get_firmware(in, out);
				break;
			case cmd::GET_DATA_ATUAL				:
				get_data_atual(in, out);
				break;
			case cmd::GET_HORA_ATUAL				:
				get_hora_atual(in, out);
				break;
			case cmd::LEITURA_X						:
				leitura_x(in, out);
				break;
			case cmd::GET_DATA_MVTO					:
				get_data_mvto(in, out);
				break;
			case cmd::GET_DATA_ULT_RED				:
				get_data_ult_red(in, out);
				break;
			case cmd::GET_HORA_ULT_RED				:
				get_hora_ult_red(in, out);
				break;
			case cmd::GET_STATUS_ECF				:
				get_status_ecf(in, out);
				break;
			case cmd::GET_NR_CUPOM					:
				get_nr_cupom(in, out);
				break;
			case cmd::GET_ID_FORMA_PGTO				:
				get_id_forma_pgto(in, out);
				break;
			case cmd::GET_TOTALIZADOR_CNF			:
				get_totalizador_cnf(in, out);
				break;
			case cmd::GET_PROX_TOTALIZADOR			:
				get_prox_totalizador(in, out);
				break;
			case cmd::SET_FORMA_PGTO				:
				set_forma_pgto(in, out);
				break;
			case cmd::SET_ALIQUOTA					:
				set_aliquota(in, out);
				break;
			case cmd::SET_TOTALIZADOR				:
				set_totalizador(in, out);
				break;
			case cmd::SND_ABRE_CUPOM				:
				snd_abre_cupom(in, out);
				break;
			case cmd::SND_CANCELA_CUPOM				:
				snd_cancela_cupom(in, out);
				break;
			case cmd::SND_VENDE_ITEM				:
				snd_vende_item(in, out);
				break;
			case cmd::SND_INICIA_FECHAMENTO			:
				snd_inicia_fechamento(in, out);
				break;
			case cmd::SND_PAGAMENTO					:
				snd_pagamento(in, out);
				break;
			case cmd::SND_FECHA_CUPOM				:
				snd_fecha_cupom(in, out);
				break;
			case cmd::SND_NFISCAL_VINC				:
				snd_nfiscal_vinc(in, out);
				break;
			case cmd::SND_MSG_VINC					:
				snd_msg_vinc(in, out);
				break;
			case cmd::SND_FECHA_VINC				:
				snd_fecha_vinc(in, out);
				break;
			case cmd::SND_NFISCAL_NVINC				:
				snd_nfiscal_nvinc(in, out);
				break;
			case cmd::GET_MAX_COL					:
				get_max_col(in, out);
				break;
			case cmd::SND_ABREGAVETA				:
				snd_abregaveta(in, out);
				break;
			case cmd::REDUCAO_Z						:
				reducao_z(in, out);
				break;
			case cmd::GET_CUPOM_ABERTO				:
				get_cupom_aberto(in, out);
				break;
			case cmd::SET_ARREDONDAMENTO			:
				set_arredondamento(in, out);
				break;
			case cmd::GET_IMPRIME_CHEQUES			:
				get_imprime_cheques(in, out);
				break;
			case cmd::GET_NR_REDUCOES				:
				get_nr_reducoes(in, out);
				break;
			case cmd::DADOS_ULT_REDUCAO				:
				dados_ult_reducao(in, out);
				break;
			case cmd::GET_CNT_REINICIO				:
				get_cnt_reinicio(in, out);
				break;
			case cmd::MEMORIA_FISCAL				:
				memoria_fiscal(in, out);
				break;
			case cmd::SET_MOEDA_SINGULAR			:
				set_moeda_singular(in, out);
				break;
			case cmd::SET_MOEDA_PLURAL				:
				set_moeda_plural(in, out);
				break;
			case cmd::SND_IMPRIME_CHEQUE			:
				snd_imprime_cheque(in, out);
				break;
			case cmd::SND_ABRE_REL_GERENCIAL		:
				snd_abre_rel_gerencial(in, out);
				break;
			case cmd::SND_TXT_GERENCIAL				:
				snd_txt_gerencial(in, out);
				break;
			case cmd::SND_FECHA_REL_GERENCIAL		:
				snd_fecha_rel_gerencial(in, out);
				break;
			case cmd::SET_REL_GERENCIAL				:
				set_rel_gerencial(in, out);
				break;
			case cmd::SND_ACIONA_GUILHOTINA			:
				snd_aciona_guilhotina(in, out);
				break;
			case cmd::GET_ID_REL_GERENCIAL			:
				get_id_rel_gerencial(in, out);
				break;
			case cmd::SND_CORTE_AUTO				:
				snd_corte_auto(in, out);
				break;
			case cmd::GET_ISTERMICA					:
				get_istermica(in, out);
				break;
			case cmd::GET_ISPAFCOMPLIANT			:
				get_ispafcompliant(in, out);
				break;
			case cmd::SET_TIPO_CORTE				:
				set_tipo_corte(in, out);
				break;
			case cmd::GRANDETOTAL					:
				grandetotal(in, out);
				break;
			case cmd::MATCHCONF						:
				matchconf(in, out);
				break;
			case cmd::SND_SANGRIA					:
				snd_sangria(in, out);
				break;
			case cmd::SND_SUPRIMENTO				:
				snd_suprimento(in, out);
				break;
			case cmd::SET_HORARIOVERAO				:
				set_horarioverao(in, out);
				break;
			case cmd::GET_ISHORARIOVERAO			:
				get_ishorarioverao(in, out);
				break;
			case cmd::DOWNLOADMFD					:
				downloadmfd(in, out);
				break;
			case cmd::GET_POUCOPAPEL				:
				get_poucopapel(in, out);
				break;
			case cmd::GET_CNT_CUPOMFISCAL			:
				get_cnt_cupomfiscal(in, out);
				break;
			case cmd::GET_CNT_NAOFISCAL				:
				get_cnt_naofiscal(in, out);
				break;
			case cmd::GET_CNT_CREDDEB				:
				get_cnt_creddeb(in, out);
				break;
			case cmd::GET_CNT_RELGERENCIAL			:
				get_cnt_relgerencial(in, out);
				break;
			case cmd::SND_CANCELA_ITEM				:
				snd_cancela_item(in, out);
				break;
			case cmd::SND_ABRERECIBO				:
				snd_abrerecibo(in, out);
				break;
			case cmd::SND_ABRECCD					:
				snd_abreccd(in, out);
				break;
			case cmd::SND_SEGUNDAVIACCD				:
				snd_segundaviaccd(in, out);
				break;
			case cmd::SND_REIMPRIMECCD				:
				snd_reimprimeccd(in, out);
				break;
			case cmd::SND_CANCELARECIBO				:
				snd_cancelarecibo(in, out);
				break;
			case cmd::SND_CANCELACCD				:
				snd_cancelaccd(in, out);
				break;
			case cmd::SND_REGNAOFISCAL				:
				snd_regnaofiscal(in, out);
				break;
			case cmd::SND_LCTONAOFISCAL				:
				snd_lctonaofiscal(in, out);
				break;
			case cmd::SND_INICIAFECHAMENTONAOFISCAL :
				snd_iniciafechamentonaofiscal(in, out);
				break;
			case cmd::SND_FECHARECIBO				:
				snd_fecharecibo(in, out);
				break;
			case cmd::SND_FECHACCD					:
				snd_fechaccd(in, out);
				break;
			case cmd::SND_CANCNAOFISCALPOS			:
				snd_cancnaofiscalpos(in, out);
				break;
			case cmd::SND_ABRE_GERENCIAL			:
				snd_abre_gerencial(in, out);
				break;
			case cmd::GET_NAOFISC_ABERTO			:
				get_naofisc_aberto(in, out);
				break;
			case cmd::GET_TAB_ALIQ				:
				get_tab_aliq(in, out);
				break;
			case cmd::GET_TAB_RELGER			:
				get_tab_relger(in, out);
				break;
			case cmd::GET_TAB_TOTCNF			:
				get_tab_totcnf(in, out);
				break;
			case cmd::GET_TAB_FPGTO				:
				get_tab_fpgto(in, out);
				break;
			case cmd::CONVERTEMFD					:
				convertemfd(in, out);
				break;
			default									:
				error_log(
				    std::string("ecf_broker::trade() Erro ") +
				    dquote(CMDTAG_UNKNOWN_CMD));
				throw cmdtag_exp(CMDTAG_UNKNOWN_CMD);
		};
	}
	catch (spooler_exp& e) {
		error_log(std::string("ecf_broker::trade() ") + dquote(e.what()));
		out->ret_code(PKT_RESPONSE_ERR);
		out->add_param("msg", e.what());

		// Nenhuma exceção é (re)lançada daqui, pois qualquer
		// mensagem de erro deve ir para o arquivo de retorno.
	}
}


/**
 \brief Obtém o número de loja programado no ECF.
 \details Adiciona ao pacote "out" o parâmetro "valor", contendo o número da
 loja.
 */
void ecf_broker::get_nr_loja(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_nr_loja()");
	
	try {
		short valor = m_ecf->get_nr_loja();
		
		xdebug_log(std::string("ecf_broker::get_nr_log() = ") + 
		    dquote(into_string(valor).c_str()));
		
		out->add_param("valor", into_string(valor));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o número de série do ECF.
 \details Adiciona ao pacote "out" o parâmetro "valor", contendo o número de
 série.
 */
void ecf_broker::get_nr_serie(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_nr_serie()");
	
	try {
		std::string valor = m_ecf->get_nr_serie();

		xdebug_log(std::string("ecf_broker::get_nr_serie() = ") + 
		    dquote(valor.c_str()));
		
		out->add_param("valor", valor);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a marca do ECF.
 \details Adiciona ao pacote "out" o parâmetro "valor" contendo a marca.
 */
void ecf_broker::get_marca(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_marca()");
	
	try {
		std::string valor = m_ecf->get_marca();
		
		xdebug_log(std::string("ecf_broker::get_marca() = ") +  
		    dquote(valor.c_str()));
		
		out->add_param("valor", valor);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o modelo do ECF.
 \details Adiciona ao pacote "out" o parâmetro "valor" contendo o modelo.
 */
void ecf_broker::get_modelo(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_modelo()");
	
	try {
		std::string valor = m_ecf->get_modelo();
		
		xdebug_log(std::string("ecf_broker::get_modelo() = ") +
		    dquote(valor.c_str()));
		
		out->add_param("valor", valor);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o número de usuário programado.
 */
void ecf_broker::get_nr_usuario(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_nr_usuario()");
	
	try {
		short valor = m_ecf->get_nr_usuario();
		
		xdebug_log(std::string("ecf_broker::get_nr_usuario() = ") +
		    dquote(into_string(valor).c_str()));
		
		out->add_param("valor", into_string(valor));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o número de série do ECF, criptografado.
 */
void ecf_broker::get_nr_serie_cript(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_nr_serie_cript()");
	
	try {
		std::string valor = m_ecf->get_nr_serie_cript();
		
		xdebug_log(std::string("ecf_broker::get_nr_serie_cript() = ") +
		    dquote(valor.c_str()));
		
		out->add_param("valor", valor);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o numéro de caixa programado.
 */
void ecf_broker::get_nr_caixa(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_nr_caixa()");
	
	try {
		short valor = m_ecf->get_nr_caixa();
		
		xdebug_log(std::string("ecf_broker::get_nr_caixa() = ") + 
		    dquote(into_string(valor).c_str()));
		
		out->add_param("valor", into_string(valor));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a versão do firware do ECF.
 */
void ecf_broker::get_firmware(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_firmware()");
	
	try {
		std::string versao = m_ecf->get_firmware();
		
		xdebug_log(std::string("ecf_broker::get_firmware() = ") + 
		    dquote(versao.c_str()));
		
		out->add_param("versao", versao);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a data atual do ECF.
 */
void ecf_broker::get_data_atual(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_data_atual()");
	
	try {
		std::string data = m_ecf->get_data_atual();
		
		xdebug_log(std::string("ecf_broker::get_data_atual() = ") + 
		    dquote(data.c_str()));
		
		out->add_param("data", data);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a hora atual do ECF.
 */
void ecf_broker::get_hora_atual(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_hora_atual()");
	
	try {
		std::string hora = m_ecf->get_hora_atual();
		
		xdebug_log(std::string("ecf_broker::get_hora_atual() = ") + 
		    dquote(hora.c_str()));
		
		out->add_param("hora", hora);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a data de movimento do ECF.
 */
void ecf_broker::get_data_mvto(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_data_mvto()");
	
	try {
		std::string data = m_ecf->get_data_mvto();
		
		xdebug_log(std::string("ecf_broker::get_data_mvto() = ") + 
		    dquote(data.c_str()));
		
		out->add_param("data", data);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a data da última Redução Z.
 */
void ecf_broker::get_data_ult_red(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_data_ult_red()");
	
	try {
		std::string data = m_ecf->get_data_ult_red();
		
		xdebug_log(std::string("ecf_broker::get_data_ult_red() = ") + 
		    dquote(data.c_str()));
		
		out->add_param("data", data);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a hora da última Redução Z.
 */
void ecf_broker::get_hora_ult_red(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_hora_ult_red()");
	
	try {
		std::string hora = m_ecf->get_hora_ult_red();
		
		xdebug_log(std::string("ecf_broker::get_hora_ult_red() = ") +
		    dquote(hora.c_str()));
		
		out->add_param("hora", hora);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o estado do ECF.
 */
void ecf_broker::get_status_ecf(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_status_ecf()");
	
	try {
		ecf_status  sts = m_ecf->get_status_ecf();
		short		count = 0;

		if (sts.pouco_papel)			count++;
		if (sts.sem_papel)				count++;
		if (sts.cupom_fiscal_aberto)	count++;
		if (sts.z_pendente)				count++;
		if (sts.memoria_cheia)			count++;
		if (sts.nao_fiscal_aberto)		count++;

		debug_log(std::string("ecf_broker::get_status_ecf() = ") +
		    dquote(into_string(count).c_str()));
		
		out->add_param("contador", into_string(count));

		if (sts.pouco_papel)	{
			warn_log("ecf_broker::get_status_ecf() Pouco papel");
			out->add_param("msg", "Pouco papel");
		}

		if (sts.sem_papel)  {
			error_log("ecf_broker::get_status_ecf() Sem papel");
			out->add_param("msg", "Sem papel");
		}

		if (sts.cupom_fiscal_aberto)	{
			info_log("ecf_broker::get_status_ecf() Cupom Fiscal aberto");
			out->add_param("msg", "Cupom Fiscal aberto");
		}

		if (sts.z_pendente) {
			info_log("ecf_broker::get_status_ecf() Reducao Z pendente");
			out->add_param("msg", "Reducao Z pendente");
		}

		if (sts.memoria_cheia)  {
			error_log("ecf_broker::get_status_ecf() Memoria Fiscal cheia");
			out->add_param("msg", "Memoria Fiscal cheia");
		}

		if (sts.nao_fiscal_aberto)  {
			info_log(
			    "ecf_broker::get_status_ecf() Comprovante Nao Fiscal aberto");
			out->add_param("msg", "Comprovante Nao Fiscal aberto");
		}
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o COO do último cupom fiscal.
 */
void ecf_broker::get_nr_cupom(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_nr_cupom()");
	
	try {
		unsigned int coo = m_ecf->get_nr_cupom();
		
		xdebug_log(std::string("ecf_broker::get_nr_cupom() = ") + 
		    dquote(into_string(coo).c_str()));
		
		out->add_param("coo", into_string(coo));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o índice de uma forma de pagamento.
 */
void ecf_broker::get_id_forma_pgto(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_id_forma_pgto(...)");

	try {
		std::string descr = get_param_value_by_name("descricao", in);
		
		xdebug_log(
		    std::string("ecf_broker::get_id_forma_pgto(...) descricao = ") + 
		    dquote(descr.c_str()));
		    
		short indice = m_ecf->get_id_forma_pgto(descr);

		xdebug_log(std::string("ecf_broker::get_id_forma_pgto(...) = ") + 
		    dquote(into_string(indice).c_str()));

		// O índice indicando NOT FOUND, é -1 internamente, mas no código
		// do sessao_ecf.pkg, está sendo verificado 0 (zero), assim, caso
		// seja identificada uma forma NÃO CADASTRADA, o valor de retorno
		// é corrigido para 0 (zero).
		// Todos os demais casos, já estão sendo tratados corretamente.
		// TODO: acertar bibliotecas Dataflex + C++ para tratar de maneira
		//			padronizada esta situação (e demais eventualmente
		//			identificadas).
		out->add_param("indice", into_string((indice < 0 ? 0 : indice)));
		
	}
	catch (...) {
		error_log(std::string("ecf_broker::get_id_forma_pgto(...) ..."));
		throw;
	}
}


/**
 \brief Obtém o índice de um totalizador não fiscal.
 */
void ecf_broker::get_totalizador_cnf(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_totalizador_cnf(...)");
	
	try {
		std::string descr = get_param_value_by_name("descricao", in);

		xdebug_log(
		    std::string("ecf_broker::get_totalizador_cnf(...) descricao = ") + 
		    dquote(descr.c_str()));

		short indice = m_ecf->get_totalizador_cnf(descr);

		xdebug_log(std::string("ecf_broker::get_totalizador_cnf(...) = ") +
		    dquote(into_string(indice).c_str()));
		
		out->add_param("indice", into_string(indice+1));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o próximo índice livre para cadastramento de totalização 
 não fiscal.
 */
void ecf_broker::get_prox_totalizador(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_prox_totalizador()");
	
	try {
		short indice = m_ecf->get_prox_totalizador();

		xdebug_log(std::string("ecf_broker::get_prox_totalizador() = ") +
		    dquote(into_string(indice).c_str()));
		
		out->add_param("indice", into_string(indice));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Registra uma forma de pagamento
 */
void ecf_broker::set_forma_pgto(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::set_forma_pagto(...)");

	try {
		std::string descr = get_param_value_by_name("descricao", in);
		xdebug_log(std::string("ecf_broker::set_forma_pgto(...) descricao = ") + 
		    dquote(descr.c_str()));

		std::string vinc;
		try {
			vinc = get_param_value_by_name("vinculado", in);
			xdebug_log(
			    std::string("ecf_broker::set_forma_pgto(...) vinculado = ") +
				dquote(vinc.c_str()));
		}
		catch (pkt_param_exp& e)   {
			vinc = "false";
		}

		m_ecf->set_forma_pgto(descr, vinc);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Registra uma alíquota tributária
 */
void ecf_broker::set_aliquota(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::set_aliquota(...)");
	
	try {
		aliq_trib::aliquota	data(get_param_value_by_name("aliquota", in));

		xdebug_log(std::string("ecf_broker::set_aliquota(...) aliquota = ") + 
		    dquote(data.get_serialized().c_str()));

		m_ecf->set_aliquota(data);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona totalizador CNF.
 */
void ecf_broker::set_totalizador(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::set_totalizador(...)");
	
	try {
		std::string valor = get_param_value_by_name("valor", in);
		std::string tipo;
		xdebug_log(std::string("ecf_broker::set_totalizador(...) valor = ") + 
		    dquote(valor.c_str()));

		try {
			tipo = get_param_value_by_name("tipo", in);
		}
		catch (pkt_param_exp& e)	{
			tipo = "E";
		}
		
		m_ecf->set_totalizador(valor, tipo);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre cupom fiscal.
 */
void ecf_broker::snd_abre_cupom(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_abre_cupom(...)");
	
	try {
		std::string nr_doc = get_param_value_by_name("nr_documento", in);
		xdebug_log(std::string("ecf_broker::snd_abre_cupom(...) documento = ") + 
		    dquote(nr_doc.c_str()));
		
		std::string nome = get_param_value_by_name("nome", in);
		xdebug_log(std::string("ecf_broker::snd_abre_cupom(...) nome = ") +
		    dquote(nome.c_str()));
		
		std::string endr = get_param_value_by_name("endereco", in);
		xdebug_log(std::string("ecf_broker::snd_abre_cupom(...) endereco = ") + 
		    dquote(endr.c_str()));

		m_ecf->abre_cupom(nr_doc, nome, endr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cancela cupom fiscal.
 */
void ecf_broker::snd_cancela_cupom(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_cancela_cupom()");
	
	try {
		m_ecf->cancela_cupom();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona um item de venda a um cupom fiscal aberto
 */
void ecf_broker::snd_vende_item(const request_packet& in, response_packet* out)
{
	std::string my_cod,
				my_descr,
				my_aliq,
				my_un;
	float		my_qtde,
				my_vlr,
				my_dsc;

	debug_log("ecf_broker::snd_vende_item(...)");
	
	try {
		my_un = get_param_value_by_name("unidade", in);
	}
	catch (...) {
		my_un = "";
	}

	try {
		my_cod = get_param_value_by_name("codigo", in);
		xdebug_log(std::string("ecf_broker::snd_vende_item(...) codigo = ") +
		    dquote(my_cod.c_str()));
		
		my_descr = get_param_value_by_name("descricao", in);
		xdebug_log(std::string("ecf_broker::snd_vende_item(...) descricao = ") +
		    dquote(my_descr.c_str()));
		
		my_aliq = get_param_value_by_name("aliquota", in);
		xdebug_log(std::string("ecf_broker::snd_vende_item(...) aliquota = ") +
		    dquote(my_aliq.c_str()));
		
		my_qtde = atof(get_param_value_by_name("quantidade", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_vende_item(...) quantidade = ") +
		    dquote(into_string(my_qtde).c_str()));
		
		my_vlr = atof(get_param_value_by_name("valor", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_vende_item(...) valor = ") +
		    dquote(into_string(my_vlr).c_str()));
		
		my_dsc = atof(get_param_value_by_name("desconto", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_vende_item(...) desconto = ") +
		    dquote(into_string(my_dsc).c_str()));
		
		m_ecf->vende_item(my_cod, my_descr, my_aliq, my_un, my_qtde, 
		    my_vlr, my_dsc);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Encerra o lançamento de itens em cupom fiscal, e subtotaliza-o.
 */
void ecf_broker::snd_inicia_fechamento(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_inicia_fechamento(...)");
	
	try {
		float   my_vlr;

		my_vlr = atof(get_param_value_by_name("valor", in).c_str());
		xdebug_log(
		    std::string("ecf_broker::snd_inicia_fechamento(...) valor = ") +
		    dquote(into_string(my_vlr).c_str()));
		
		m_ecf->inicia_fechamento(my_vlr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona uma moeda e valor, como pagamento em um cupom fiscal.
 */
void ecf_broker::snd_pagamento(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_pagamento(...)");
	
	try {
		std::string my_moeda = get_param_value_by_name("indice", in);
		xdebug_log(std::string("ecf_broker::snd_pagamento(...) indice = ") +
		    dquote(my_moeda.c_str()));
		
		float		my_vlr = atof(get_param_value_by_name("valor", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_pagamento(...) valor = ") +
		    dquote(into_string(my_vlr).c_str()));
		
		m_ecf->pagamento(my_moeda, my_vlr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha cupom fiscal.
 */
void ecf_broker::snd_fecha_cupom(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_fecha_cupom(...)");
	
	try {
		std::string msg = get_param_value_by_name("mensagem", in);
		xdebug_log(std::string("ecf_broker::snd_fecha_cupom(...) mensagem = ") +
		    dquote(msg.c_str()));
		
		m_ecf->fecha_cupom(msg);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre comprovante não fiscal vinculado.
 */
void ecf_broker::snd_nfiscal_vinc(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_nfiscal_vinc(...)");
	
	try {
		std::string my_descr = get_param_value_by_name("descricao", in);
			xdebug_log(
			    std::string("ecf_broker::snd_nfiscal_vinc(...) descricao = ") +
			    dquote(my_descr.c_str()));

		float		my_vlr = atof(get_param_value_by_name("valor", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_nfiscal_vinc(...) valor = ") +
		    dquote(into_string(my_vlr).c_str()));

		int			my_coo = atoi(get_param_value_by_name("coo", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_nfiscal_vinc(...) coo = ") +
		    dquote(into_string(my_coo).c_str()));
		
		m_ecf->abre_nfiscal_vinc(my_descr, my_vlr, my_coo);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona mensagem de texto a comprovante não fiscal vinculado aberto.
 */
void ecf_broker::snd_msg_vinc(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_msg_vinc(...)");
	
	try {
		std::string msg = get_param_value_by_name("mensagem", in);
		xdebug_log(std::string("ecf_broker::snd_msg_vinc(...) mensagem = ") +
		    dquote(msg.c_str()));
		
		m_ecf->msg_nfiscal_vinc(msg);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha comprovante não fiscal vinculado.
 */
void ecf_broker::snd_fecha_vinc(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_fecha_vinc()");
	
	try {
		m_ecf->fecha_nfiscal_vinc();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre comprovante não fiscal não vinculado.
 */
void ecf_broker::snd_nfiscal_nvinc(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_nfiscal_nvinc(...)");
	
	try {
		std::string codigo = get_param_value_by_name("codigo", in);
		xdebug_log(std::string("ecf_broker::snd_nfiscal_nvinc(...) codigo = ")
		    + dquote(codigo.c_str()));
		
		float valor = atof(get_param_value_by_name("valor", in).c_str());
		xdebug_log(std::string("ecf_broker::snd_nfiscal_nvinc(...) valor = ")
		    + dquote(into_string(valor).c_str()));
		
		std::string moeda = get_param_value_by_name("moeda", in);
		xdebug_log(std::string("ecf_broker::snd_nfiscal_nvinc(...) moeda = ")
		    + dquote(moeda.c_str()));
		
		m_ecf->abre_nfiscal_nvinc(codigo, valor, moeda);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o número máximo de colunas em um cupom.
 */
void ecf_broker::get_max_col(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_max_col()");
	
	try {
		out->add_param("resposta", into_string(m_ecf->get_max_col()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Comando a abertura de gaveta de dinheiro conectada ao ECF.
 */
void ecf_broker::snd_abregaveta(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_abregaveta()");
	
	try {
		m_ecf->abre_gaveta();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Comando o encerramento do movimento no ECF.
 */
void ecf_broker::reducao_z(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::reducao_z()");
	
	try {
		m_ecf->reducao_z();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém indicador informando se há ou não cupom fiscal aberto.
 */
void ecf_broker::get_cupom_aberto(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_cupom_aberto()");
	
	try {
		out->add_param("resposta", (m_ecf->get_cupom_aberto() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Define a flag de arredondamento do ECF.
 */
void ecf_broker::set_arredondamento(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::set_arredondamento()");
	
	try {
		m_ecf->set_arredondamento();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém indicador informando se o equipamento oferece ou não impressão de
 cheques.
 */
void ecf_broker::get_imprime_cheques(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_imprime_cheques()");
	
	try {
		out->add_param("resposta", 
		    (m_ecf->get_imprime_cheques() ? "SIM" : "NAO"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o contador de reduções Z.
 */
void ecf_broker::get_nr_reducoes(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_nr_reducoes(...)");
	
	try {
		out->add_param("valor", into_string(m_ecf->get_nr_reducoes()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém os dados da última Redução Z.
 */
void ecf_broker::dados_ult_reducao(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::dados_ult_reducao()");
	
	try {
		std::string data_file = make_datafile("mapaecf_");
		xdebug_log(std::string("ecf_broker::dados_ult_reducao() Arq. dados = ")
		    + dquote(data_file.c_str()));

		std::ofstream out_mapa(data_file.c_str());
		out_mapa << m_ecf->dados_ult_reducao();
		out_mapa.close();
		
		out->add_param("dados", data_file);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o contador de vezes que a memória fiscal foi reiniciada.
 */
void ecf_broker::get_cnt_reinicio(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_cnt_reinicio()");
	
	try {
		out->add_param("valor", into_string(m_ecf->get_nr_reinicio()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém leitura da memória fiscal
 */
void ecf_broker::memoria_fiscal(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::memoria_fiscal(...)");
	
	try {
		int	my_tipo = (str_iequals(get_param_value_by_name("tipo", in), 
						"data") ? ECF_LEITURA_DATA : ECF_LEITURA_REDUCAO);
		xdebug_log(std::string("ecf_broker::memoria_fiscal(...) tipo = ") +
		    dquote(into_string(my_tipo).c_str()));
		
		std::string my_ini = get_param_value_by_name("data_inicio", in);
		xdebug_log(std::string("ecf_broker::memoria_fiscal(...) data_inicio = ")
		    + dquote(my_ini.c_str()));
		
		std::string my_fim = get_param_value_by_name("data_fim", in);
		xdebug_log(std::string("ecf_broker::memoria_fiscal(...) data_fim = ") +
		    dquote(my_fim.c_str()));
		
		bool my_compl = (str_iequals(get_param_value_by_name("completa", in), 
							"false") ? false : true);
		xdebug_log(std::string("ecf_broker::memoria_fiscal(...) completa = ") +
		    std::string((my_compl ? "sim" : "nao")));
		
		int my_saida = (str_iequals(get_param_value_by_name("saida", in),
						"serial") ? ECF_SAIDA_SERIAL : ECF_SAIDA_ECF);
		xdebug_log(std::string("ecf_broker::memoria_fiscal(...) saida = ") +
		    std::string((my_saida == ECF_SAIDA_SERIAL ? "serial" : "ecf")));
		
		out->add_param("dados", m_ecf->memoria_fiscal(my_tipo, my_ini, my_fim, 
		    my_compl, my_saida));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Emite Leitura X
 */
void ecf_broker::leitura_x(const request_packet& in, response_packet* out)
{
	std::string arg;

	debug_log("ecf_broker::leitura_x(...)");
	
	// Parâmetro opcional...
	try {
		arg = get_param_value_by_name("saida", in);
	}
	catch (pkt_param_exp& e)	{
		arg = "ecf";
	}
	
	try {
		xdebug_log(std::string("ecf_broker::leitura_x(...) saida = ") +
		    dquote(arg.c_str()));
		
		int saida = (
		    str_iequals(arg, "serial") ? ECF_SAIDA_SERIAL : ECF_SAIDA_ECF
		);
		std::string arq = m_ecf->leitura_x(saida);
		if (!arq.empty())
			out->add_param("dados", arq);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Define nome de moeda no singular.
 */
void ecf_broker::set_moeda_singular(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::set_moeda_singular(...)");
	
	try {
		std::string moeda = get_param_value_by_name("descricao", in);
		xdebug_log(
		    std::string("ecf_broker::set_moeda_singular(...) descricao = ")
		    + dquote(moeda.c_str()));
		
		m_ecf->set_moeda_singular(moeda);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Define nome da moeda no plural.
 */
void ecf_broker::set_moeda_plural(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::set_moeda_plural(...)");
	
	try {
		std::string moeda = get_param_value_by_name("descricao", in);
		xdebug_log(
		    std::string("ecf_broker::set_moeda_plural(...) descricao = ")
		    + dquote(moeda.c_str()));
		
		m_ecf->set_moeda_plural(moeda);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Comanda a impressão de cheque.
 */
void ecf_broker::snd_imprime_cheque(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_imprime_cheque(...)");
	try {
		m_ecf->imprime_cheque(
				atoi(get_param_value_by_name("valor_x", in).c_str()),
				atoi(get_param_value_by_name("valor_y", in).c_str()),
				atof(get_param_value_by_name("valor", in).c_str()),
				atoi(get_param_value_by_name("ext1_x", in).c_str()),
				atoi(get_param_value_by_name("ext1_y", in).c_str()),
				atoi(get_param_value_by_name("ext2_x", in).c_str()),
				atoi(get_param_value_by_name("ext2_y", in).c_str()),
				atoi(get_param_value_by_name("fav_x", in).c_str()),
				atoi(get_param_value_by_name("fav_y", in).c_str()),
				get_param_value_by_name("favorecido", in),
				atoi(get_param_value_by_name("local_x", in).c_str()),
				atoi(get_param_value_by_name("local_y", in).c_str()),
				get_param_value_by_name("localidade", in),
				atoi(get_param_value_by_name("dia_x", in).c_str()),
				atoi(get_param_value_by_name("dia", in).c_str()),
				atoi(get_param_value_by_name("mes_x", in).c_str()),
				atoi(get_param_value_by_name("mes", in).c_str()),
				atoi(get_param_value_by_name("ano_x", in).c_str()),
				atoi(get_param_value_by_name("ano", in).c_str()),
				get_param_value_by_name("mensagem", in)
		    );
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre relatório gerencial
 */
void ecf_broker::snd_abre_rel_gerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_abre_rel_gerencial(...)");
	
	try {
		std::string titulo = get_param_value_by_name("titulo", in);
		xdebug_log(
		    std::string("ecf_broker::snd_abre_rel_gerencial(...) titulo = ")
		    + dquote(titulo.c_str()));
		
		m_ecf->abre_rel_gerencial(titulo);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Imprime texto em relatório gerencial
 */
void ecf_broker::snd_txt_gerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_txt_gerencial(...)");
	try {
		std::string msg = get_param_value_by_name("mensagem", in);
		xdebug_log(
		    std::string("ecf_broker::snd_txt_gerencial(...) mensagem = ")
		    + dquote(msg.c_str()));
		
		m_ecf->txt_rel_gerencial(msg);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha relatório gerencial
 */
void ecf_broker::snd_fecha_rel_gerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_fecha_rel_gerencial()");
	
	try {
		m_ecf->fecha_rel_gerencial();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona novo relatório gerencial ao ECF.
 */
void ecf_broker::set_rel_gerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::set_rel_gerencial(...)");
	
	try {
		std::string descr = get_param_value_by_name("descricao", in);
		xdebug_log(
		    std::string("ecf_broker::set_rel_gerencial(...) descricao = ")
		    + dquote(descr.c_str()));
		
		m_ecf->set_rel_gerencial(descr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Aciona guilhotina.
 */
void ecf_broker::snd_aciona_guilhotina(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_aciona_guilhotina(...)");
	
	try {
		int modo = (
		    str_iequals(get_param_value_by_name("cortetotal", in), "true") ? 
		    ECF_CORTE_TOTAL :
			ECF_CORTE_PARCIAL
		);
		xdebug_log(
		    std::string("ecf_broker::snd_aciona_guilhotina(...) cortetotal = ")
		    + std::string((modo == ECF_CORTE_TOTAL ? "sim" : "nao")));
		m_ecf->aciona_guilhotina(modo);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém índice de relatório gerencial.
 */
void ecf_broker::get_id_rel_gerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_id_rel_gerencial(...)");
	
	try {
		std::string descr = get_param_value_by_name("descricao", in);
		xdebug_log(
		    std::string("ecf_broker::get_id_rel_gerencial(...) descricao = ")
		    + dquote(descr.c_str()));
		
		out->add_param("valor", 
		    into_string(m_ecf->get_id_rel_gerencial(descr)));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Desabilita o próximo acionamento automático da guilhotina.
 */
void ecf_broker::snd_corte_auto(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_corte_auto()");
	
	try {
		m_ecf->corte_auto();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém flag indicando se ECF é ou não equipado com mecanismo de 
 impressão térmica.
 */
void ecf_broker::get_istermica(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_istermica()");
	
	try {
		out->add_param("valor", (m_ecf->is_termica() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém flag indicando se o equipamento é compatível com convênio ICMS
 que determina exigências para PAF-EF.
 */
void ecf_broker::get_ispafcompliant(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_ispafcompliant()");
	
	try {
		out->add_param("valor", (m_ecf->is_paf_compliant() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Determina o tipo de corte a ser executado pela guilotina.
 */
void ecf_broker::set_tipo_corte(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::set_tipo_corte(...)");
	
	try {
		bool total = (str_iequals(
		    get_param_value_by_name("cortetotal", in), "true") ? true : false);
		xdebug_log(std::string("ecf_broker::set_tipo_corte(...) cortetotal = ")
		    + std::string((total ? "sim" : "nao")));
		
		m_ecf->set_tipo_corte(total);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o GT do ECF.
 */
void ecf_broker::grandetotal(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::grandetotal()");
	
	try {
		out->add_param("valor", into_string(m_ecf->get_grande_total()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Verifica se o equipamento está devidamente configurado.
 */
void ecf_broker::matchconf(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::matchconf()");
	
	try {
		out->add_param("resposta", (m_ecf->match_conf() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Efetua sangria.
 */
void ecf_broker::snd_sangria(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_sangria(...)");
	
	try {
		std::string valor = get_param_value_by_name("valor", in);
		xdebug_log(std::string("ecf_broker::snd_sangria(...) valor = ") +
		    dquote(valor.c_str()));
		
		m_ecf->sangria(atof(valor.c_str()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Efetua suprimento de caixa no equipamento.
 */
void ecf_broker::snd_suprimento(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_suprimento(...)");
	
	try {
		std::string valor = get_param_value_by_name("valor", in);
		xdebug_log(std::string("ecf_broker::snd_suprimento(...) valor = ") +
		    dquote(valor.c_str()));
		
		m_ecf->suprimento(atof(valor.c_str()));
	}
	catch (...) {
	    throw;
    }
}


/**
 \brief Ativa/desativa horário de verão.
 */
void ecf_broker::set_horarioverao(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::set_horarioverao()");
	
	try {
		m_ecf->set_horario_verao();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém flag indicando se horário de verão está ou não ativo no ECF.
 */
void ecf_broker::get_ishorarioverao(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_ishorarioverao()");
	
	try {
		out->add_param("resposta", (m_ecf->is_horario_verao() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Efetua o download da MFD.
 */
void ecf_broker::downloadmfd(const request_packet& in, response_packet* out)
{
	try {
		out->add_param("dados", m_ecf->download_mfd(
			    (str_iequals(get_param_value_by_name("tipo", in), "data") ?
				    ECF_DOWNLOAD_MFD_DATA : ECF_DOWNLOAD_MFD_COO),
			    get_param_value_by_name("inicial", in),
			    get_param_value_by_name("final", in)
			));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Verifica o estado do sensor de pouco papel do ECF.
 */
void ecf_broker::get_poucopapel(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_poucopapel()");
	
	try {
		out->add_param("resposta", (m_ecf->pouco_papel() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o contador de cupons fiscais emitidos.
 */
void ecf_broker::get_cnt_cupomfiscal(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_cnt_cupomfiscal()");
	
	try {
		out->add_param("valor", into_string(m_ecf->contador_cupom_fiscal()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o contador de comprovantes não fiscais emitidos.
 */
void ecf_broker::get_cnt_naofiscal(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_cnt_naofiscal()");
	
	try {
		out->add_param("valor", into_string(m_ecf->contador_tot_naofiscal()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém o contador de CCD emitidos.
 */
void ecf_broker::get_cnt_creddeb(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::get_cnt_creddeb()");
	
	try {
		out->add_param("valor", into_string(m_ecf->contador_credito_debito()));
	}
	catch (...) {
		throw;
	}
}


/*
 \brief Obtém o contador de relatórios gerenciais emitidos.
 */
void ecf_broker::get_cnt_relgerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_cnt_relgerencial()");
	
	try {
		out->add_param("valor", into_string(m_ecf->contador_rel_gerenciais()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cancela o último ítem de venda registrado no ECF.
 */
void ecf_broker::snd_cancela_item(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_cancela_item()");
	
	try {
		m_ecf->cancela_item();
	}
	catch (...) {
		throw;
	}
}


/*
 \brief Abre recibo
 */
void ecf_broker::snd_abrerecibo(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_abrerecibo(...)");
	    
	try {
		std::string dcto = get_param_value_by_name("nr_documento", in);
		xdebug_log(std::string("ecf_broker::snd_abrerecibo(...) documento = ") +
		    dquote(dcto.c_str()));
		
		std::string nome = get_param_value_by_name("nome", in);
		xdebug_log(std::string("ecf_broker::snd_abrerecibo(...) nome = ") +
		    dquote(nome.c_str()));
		
		std::string endr = get_param_value_by_name("endereco", in);
		xdebug_log(std::string("ecf_broker::snd_abrerecibo(...) endereco = ") +
		    dquote(endr.c_str()));
		
		m_ecf->abre_recibo(dcto, nome, endr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre CCD
 */
void ecf_broker::snd_abreccd(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_abreccd(...)");
	try {
		std::string fpgto = get_param_value_by_name("formapgto", in);
		xdebug_log(std::string("ecf_broker::snd_abreccd(...) formapgto = ") +
			dquote(fpgto.c_str()));

		std::string valor = get_param_value_by_name("valor", in);
		xdebug_log(std::string("ecf_broker::snd_abreccd(...) valor = ") +
			dquote(fpgto.c_str()));

		std::string coo = get_param_value_by_name("coo", in);
		xdebug_log(std::string("ecf_broker::snd_abreccd(...) coo = ") +
		    dquote(coo.c_str()));

		std::string dcto = get_param_value_by_name("nr_documento", in);
		xdebug_log(std::string("ecf_broker::snd_abreccd(...) documento = ") +
		    dquote(dcto.c_str()));

		std::string nome = get_param_value_by_name("nome", in);
		xdebug_log(std::string("ecf_broker::snd_abreccd(...) nome = ") +
		    dquote(nome.c_str()));

		std::string endr = get_param_value_by_name("endereco", in);
		xdebug_log(std::string("ecf_broker::snd_abreccd(...) endereco = ") +
		    dquote(endr.c_str()));
		
		m_ecf->abre_credito_debito(fpgto, atof(valor.c_str()),
		    atoi(coo.c_str()), dcto, nome, endr);
	}
	catch (...) {
		throw;
	}
}


/*
 \brief Emite 2a via de CCD.
 */
void ecf_broker::snd_segundaviaccd(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_segundaviaccd()");
	
	try {
		m_ecf->segunda_via_ccd();
	}
	catch (...) {
		throw;
	}
}


/*
 \brief Reimpŕime CCD.
 */
void ecf_broker::snd_reimprimeccd(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::reimprime_ccd()");
	
	try {
		m_ecf->reimprime_ccd();
	}
	catch (...) {
		throw;
	}
}


/*
 \brief Cancela recibo.
 */
void ecf_broker::snd_cancelarecibo(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_cancelarecibo()");
	
	std::string doc,
				nome,
				endr;

	// Coleta de parâmetros é opcional, pois a implementação
	// do Sólidus não os provê.
	try {
		doc = get_param_value_by_name("nr_documento", in);
		nome = get_param_value_by_name("nome", in);
		endr = get_param_value_by_name("endereco", in);
	}
	catch (pkt_param_exp& e)	{
		doc = "";
		nome = "";
		endr = "";
	}

	try {
		m_ecf->cancela_recibo(doc, nome, endr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Estorna CCD.
 */
void ecf_broker::snd_cancelaccd(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_cancelaccd(...)");
	
	try {
		std::string dcto = get_param_value_by_name("nr_documento", in);
		xdebug_log(std::string("ecf_broker::snd_cancelaccd(...) documento = )")
			+ dquote(dcto.c_str()));
		
		std::string nome = get_param_value_by_name("nome", in);
		xdebug_log(std::string("ecf_broker::snd_cancelaccd(...) nome = )") +
			dquote(nome.c_str()));
		
		std::string endr = get_param_value_by_name("endereco", in);
		xdebug_log(std::string("ecf_broker::snd_cancelaccd(...) endereco = )") +
			dquote(endr.c_str()));
		
		m_ecf->estorna_ccd(dcto, nome, endr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Lança um item não fiscal.
 */
void ecf_broker::snd_regnaofiscal(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_regnaofiscal(...)");
	
	try {
		std::string idx = get_param_value_by_name("indice", in);
		xdebug_log(std::string("ecf_broker::snd_regnaofiscal(...) indice =") +
		    dquote(idx.c_str()));
		
		std::string vlr = get_param_value_by_name("valor", in);
		xdebug_log(std::string("ecf_broker::snd_regnaofiscal(...) valor =") +
		    dquote(vlr.c_str()));
		
		m_ecf->reg_nao_fiscal(idx, atof(vlr.c_str()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Efetua pagamento não fiscal
 */
void ecf_broker::snd_lctonaofiscal(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_lctonaofiscal(...)");
	
	try {
		std::string fpgto = get_param_value_by_name("formapgto", in);
		xdebug_log(
		    std::string("ecf_broker::snd_lctonaofiscal(...) formapgto = ")
		    + dquote(fpgto.c_str()));

		std::string valor = get_param_value_by_name("valor", in);
		xdebug_log(
		    std::string("ecf_broker::snd_lctonaofiscal(...) valor = ") +
		    dquote(valor.c_str()));
		
		m_ecf->lcto_nao_fiscal(fpgto, atof(valor.c_str()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Subtotaliza documento não fiscal.
 */
void ecf_broker::snd_iniciafechamentonaofiscal(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_iniciafechamentonaofiscal(...)");
	
	try {
		std::string tipo = get_param_value_by_name("tipo", in);
		xdebug_log(
		    std::string("ecf_broker::snd_iniciafechamentonaofiscal(...) tipo =")
		    + dquote(tipo.c_str()));
		
		std::string valor = get_param_value_by_name("valor", in);
		xdebug_log(
		    std::string("ecf_broker::snd_iniciafechamentonaofiscal(...) valor =")
		    + dquote(valor.c_str()));
		
		m_ecf->inicia_fechamento_nao_fiscal(tipo.c_str()[0], 
		    atof(valor.c_str()));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fechamento comprovante não fiscal (recibo).
 */
void ecf_broker::snd_fecharecibo(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_fecharecibo(...)");
	
	try {
		std::string msg = get_param_value_by_name("mensagem", in);
		xdebug_log(std::string("ecf_broker::snd_fecharecibo(...) mensagem = ") +
		    dquote(msg.c_str()));
		
		m_ecf->fecha_recibo(msg);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Fecha CCD.
 */
void ecf_broker::snd_fechaccd(const request_packet& in, response_packet* out)
{
	debug_log("ecf_broker::snd_fechaccd()");
	
	try {
		m_ecf->fecha_ccd();
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cancela documento não fiscal posterior.
 */
void ecf_broker::snd_cancnaofiscalpos(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_cancnaofiscalpos(...)");
	
	try {
		std::string fpgto = get_param_value_by_name("formapgto", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) formagpto =") + 
		    dquote(fpgto.c_str()));
		
		std::string valor = get_param_value_by_name("valor", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) valor =") + 
		    dquote(valor.c_str()));
		
		std::string coo = get_param_value_by_name("coo", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) coo =") + 
		    dquote(coo.c_str()));
		
		std::string cdc = get_param_value_by_name("cdc", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) cdc =") + 
		    dquote(cdc.c_str()));
		
		std::string dcto = get_param_value_by_name("nr_documento", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) documento =") + 
		    dquote(dcto.c_str()));
		
		std::string nome = get_param_value_by_name("nome", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) nome =") + 
		    dquote(nome.c_str()));
		
		std::string endr = get_param_value_by_name("endereco", in);
		xdebug_log(
		    std::string("ecf_broker::snd_cancnaofiscalpos(...) endereco =") + 
		    dquote(endr.c_str()));
		
		m_ecf->cancela_nfiscal_pos(fpgto, atof(valor.c_str()),
		    atoi(coo.c_str()), atoi(cdc.c_str()), dcto, nome, endr);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Abre relatório gerencial, imprime texto e fecha-o.
 */
void ecf_broker::snd_abre_gerencial(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::snd_abre_gerencial(...)");
	
	try {
		snd_abre_rel_gerencial(in, out);

		std::string texto = "";
		pkt_param   msg;
		
		try {
			msg = get_param_by_name("mensagem", in);
			std::string slice = "";
			try {
				slice = msg.value();

				xdebug_log(
				    std::string(
					    "ecf_broker::snd_abre_gerencial(...) 1o slice =") +
					dquote(slice.c_str()));
					    
				texto = slice;
				do  {
					try {
						slice = msg.next_value();

						xdebug_log(
						    std::string("ecf_broker::snd_abre_gerencial") +
						    std::string("(...) 'n' slice =") +
						    dquote(slice.c_str()));
						
						texto += slice;
					}
					catch (pkt_param_exp& e)	{
						// Não há mais valor para o parâmetro
						slice = "";
					}
				} while (!slice.empty());
			}
			catch (pkt_param_exp& e)	{
				// Parâmetro sem valor, a mensagem é vazia.
				texto = "";
			}
		}
		catch (packet_exp& e)	{
			// O parâmetro não existe, a mensagem é vazia.
			texto = "";
		}

		if (!texto.empty())
			m_ecf->txt_rel_gerencial(texto, "cortar_papel_ecf");

		snd_fecha_rel_gerencial(in, out);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém indicador de documento não fiscal aberto.
 */
void ecf_broker::get_naofisc_aberto(const request_packet& in, 
    response_packet* out)
{
	debug_log("ecf_broker::get_naofisc_aberto()");
	
	try {
		out->add_param("valor", (m_ecf->nao_fiscal_aberto() ? "1" : "0"));
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém tabela de alíquotas do equipamento
 */
void ecf_broker::get_tab_aliq(const request_packet& in,
	response_packet* out)
{
	debug_log("ecf_broker::get_tab_aliq()");

	try {
		std::string arq = m_ecf->get_tab_aliq();
		if (!arq.empty())   {
			out->add_param("dados", arq);
			xdebug_log(std::string("ecf_brocker::get_tab_aliq() = \"") +
			           arq + std::string("\""));
		}
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém tabela de relatórios gerenciais do equipamento
 */
void ecf_broker::get_tab_relger(const request_packet& in,
	response_packet* out)
{
	debug_log("ecf_broker::get_tab_relger()");

	try {
		std::string arq = m_ecf->get_tab_relger();
		if (!arq.empty())
			out->add_param("dados", arq);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém tabela de totalizadores não sujeitos ao ICMS do equipamento
 */
void ecf_broker::get_tab_totcnf(const request_packet& in,
	response_packet* out)
{
	debug_log("ecf_broker::get_tab_totcnf()");

	try {
		std::string arq = m_ecf->get_tab_totcnf();
		if (!arq.empty())
			out->add_param("dados", arq);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém tabela de formas/meios de pagamento do equipamento
 */
void ecf_broker::get_tab_fpgto(const request_packet& in,
	response_packet* out)
{
	debug_log("ecf_broker::get_tab_fpgto()");

	try {
		std::string arq = m_ecf->get_tab_fpgto();
		if (!arq.empty())
			out->add_param("dados", arq);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Converte para texto, arquivo binário contendo download da MFD.
 */
void ecf_broker::convertemfd(const request_packet& in, response_packet* out)
{
	int			tipo;
	std::string	inicial;
	std::string	final;
	std::string	origem;
	std::string	destino;
	int			fmt;
	int			usr;

	debug_log("ecf_broker::convertemfd(...)");
	try {
		tipo	= (str_iequals(get_param_value_by_name("tipo", in), "data") ?
				    ECF_DOWNLOAD_MFD_DATA : ECF_DOWNLOAD_MFD_COO);
		xdebug_log(std::string("ecf_broker::convertemfd(...) tipo = ") +
					into_string(tipo));
				    
		inicial = get_param_value_by_name("inicial", in);
		xdebug_log(std::string("ecf_broker::convertemfd(...) inicial = ") +
					dquote(inicial.c_str()));
					
		final	= get_param_value_by_name("final", in);
		xdebug_log(std::string("ecf_broker::convertemfd(...) final = ") +
					dquote(final.c_str()));
					
		origem	= get_param_value_by_name("origem", in);
		xdebug_log(std::string("ecf_broker::convertemfd(...) origem = ") +
					dquote(origem.c_str()));
					
		destino	= get_param_value_by_name("destino", in);
		xdebug_log(std::string("ecf_broker::convertemfd(...) destino = ") +
					dquote(destino.c_str()));
		
		// A menos que seja requisitada explicitamente a geracao no
		// formato do ATO COTEPE, assume por padrao, geracao do espelho
		// da MFD.
		try {
			fmt = (str_iequals(get_param_value_by_name("formato", in), 
					"mfd") ? ECF_CONV_MFD_TEXTO : ECF_CONV_MFD_COTEPE);
			xdebug_log(std::string("ecf_broker::convertemfd(...) formato = ") +
						std::string(
							(fmt == ECF_CONV_MFD_TEXTO ? "mfd" : "cotepe")));
		}
		catch (pkt_param_exp& e) {
			fmt = ECF_CONV_MFD_TEXTO;
			xdebug_log(
				"ecf_broker::convertemfd(...) formato = \"mfd\" (default)");
		}
		
		// Se nenhum índice de usuário do ECF for informado, 
		// assume-se que seja um único cadastrado.
		try {
			usr = atoi(get_param_value_by_name("usuario", in).c_str());
			xdebug_log(std::string("ecf_broker::convertemfd(...) usuario = ") +
						into_string(usr));
		}
		catch (pkt_param_exp& e) {
			usr = 1;
			xdebug_log("ecf_broker::convertemfd(...) usuario = 1 (default)");
		}
		
		m_ecf->converte_mfd(tipo, inicial, final, origem, destino, fmt, usr);
	}
	catch (...) {
		throw;
	}
}

