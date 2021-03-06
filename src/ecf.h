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

#ifndef _ECF_H_
#define _ECF_H_


#include <exception>
#include <string>
#include <time.h>

#include "excp.h"
#include "aliquota.h"

#define SPOOLER_FAKE			"SPOOLER_FAKE"

#define ECF_UNEXPECTED_MSG		"Impossivel identificar erro."
#define ECF_DLL_CALL_ERR		"Erro ao acionar biblioteca do fabricante."
#define ECF_ALIQ_NOT_DEFINED	"Aliquota tributaria nao definida."
#define ECF_ALIQ_DEFINED		"Aliquota tributaria ja definida."
#define ECF_TOT_CNF_MISSING		"Totalizador nao fiscal nao encontrado."
#define ECF_TOT_CNF_PRESENT		"Totalizador nao fiscal ja cadastrado."
#define ECF_NUM_FORMAT_ERR		"Erro ao formatar valor."
#define ECF_TIPO_LEITURA_ERR	"Tipo de leitura nao suportado."
#define ECF_REL_GER_MISSING		"Relatorio Gerencial nao encontrado."
#define ECF_REL_GER_PRESENT		"Relatorio Gerencial ja cadastrado."
#define ECF_TIPO_DOWNLOAD_ERR   "Tipo de download da MFD nao suportado."
#define ECF_FMT_CONV_ERR		"Formato de conversao de MFD nao suportado."
#define ECF_TIPO_CORTE_ERR		"Modo de corte nao reconhecido."
#define ECF_FPGTO_MISSING		"Forma de pagamento nao encontrada."
#define ECF_FPGTO_PRESENT		"Forma de pagamento ja cadastrada."
#define ECF_SUBTOTAL_MODE_ERR   "Modo invalido ao subtotalizar."
#define ECF_CCD_UNAVAILABLE		"Nao ha moedas vinculadas para emitir CCD."

#define ECF_SAIDA_ECF			0
#define ECF_SAIDA_SERIAL		1

#define ECF_LEITURA_DATA		0
#define ECF_LEITURA_REDUCAO		1
#define ECF_LEITURA_COO			2

#define ECF_LEITURA_SIMPLES		0
#define ECF_LEITURA_COMPLETA	1

#define ECF_CORTE_PARCIAL		0
#define ECF_CORTE_TOTAL			1

#define ECF_DOWNLOAD_MFD_TOTAL	0
#define ECF_DOWNLOAD_MFD_DATA   1
#define ECF_DOWNLOAD_MFD_COO	2

#define ECF_CONV_MFD_TEXTO		0	// 'espelho' da MFD
#define ECF_CONV_MFD_COTEPE		1	// Formato para homologacao PAF-ECF


/**
 \class ecf_status
 \brief Mantém, de modo coeso e organizado, conjunto de dados que definem
 o estado do Emissor de Cupons Fiscais.
 */
class ecf_status
{
	public:
		bool				pouco_papel;
		bool				sem_papel;
		bool				cupom_fiscal_aberto;
		bool				z_pendente;
		bool				memoria_cheia;
		bool				nao_fiscal_aberto;
		
							ecf_status(void);
							ecf_status(const ecf_status& obj);
		ecf_status&			operator=(const ecf_status& obj);
};


/**
 \interface ecf
 \brief Define conjunto de qualquer equipamento Emissor de Cupons Fiscais.
 */
class ecf
{
	public:
		static const char   ACRESCIMO_PERCENTUAL;
		static const char   DESCONTO_PERCENTUAL;
		static const char   ACRESCIMO_VALOR;
		static const char   DESCONTO_VALOR;
		static const char   ACR_DSC_PERCENTUAL;
		static const char   ACR_DSC_VALOR;
		static const char   NO_OP;

							ecf(const std::string& cfg_file = "");
		
		virtual short		get_nr_loja(void) = 0;
		virtual std::string get_nr_serie(void) = 0;
		virtual std::string get_marca(void) = 0;
		virtual std::string get_modelo(void) = 0;
		virtual short		get_nr_usuario(void) = 0;
		virtual std::string get_nr_serie_cript(void) = 0;
		virtual short		get_nr_caixa(void) = 0;
		virtual std::string get_firmware(void) = 0;
		virtual std::string get_data_atual(void) = 0;
		virtual std::string get_hora_atual(void) = 0;
		virtual std::string get_data_mvto(void) = 0;
		virtual std::string get_data_ult_red(void) = 0;
		virtual std::string get_hora_ult_red(void) = 0;
		virtual ecf_status  get_status_ecf(void) = 0;
		virtual unsigned 
				int			get_nr_cupom(void) = 0;
		virtual short		get_id_forma_pgto(const std::string& fpgto) = 0;
		virtual bool		get_vinc_forma_pgto(const short idx) = 0;
		virtual short		get_totalizador_cnf(const std::string& tot) = 0;
		virtual short		get_prox_totalizador(void) = 0;
		virtual void		set_forma_pgto(const std::string& forma,
											const std::string& vinc) = 0;
		virtual void		set_aliquota(const aliq_trib::aliquota& aliq) = 0;
		virtual void		set_totalizador(const std::string& descr,
										const std::string& tipo) = 0;
		virtual void		abre_cupom(const std::string& dcto, 
										const std::string& nome,
										const std::string& endr) = 0;
		virtual void		cancela_cupom(void) = 0;
		virtual void		vende_item(const std::string& codprod,
										const std::string& nomeprod,
										const std::string& straliq,
										const std::string& unidade,
										float qtde,
										float valor, 
										float desc) = 0;
		virtual void		inicia_fechamento(float acresc) = 0;
		virtual void		pagamento(const std::string& fpgto, float valor) = 0;
		virtual void		fecha_cupom(const std::string& msg) = 0;
		virtual void		abre_nfiscal_vinc(const std::string& fpgto,
												float valor, unsigned int coo) = 0;
		virtual void		msg_nfiscal_vinc(const std::string& msg) = 0;
		virtual void		fecha_nfiscal_vinc(void) = 0;
		virtual void		abre_nfiscal_nvinc(const std::string& nome_tot,
												float valor,
												const std::string& moeda) = 0;
		virtual short		get_max_col(void) = 0;
		virtual void		abre_gaveta(void) = 0;
		virtual void		reducao_z(void) = 0;
		virtual bool		get_cupom_aberto(void) = 0;
		virtual void		set_arredondamento(void) = 0;
		virtual bool		get_imprime_cheques(void) = 0;
		virtual short		get_nr_reducoes(void) = 0;
		virtual std::string	dados_ult_reducao(void) = 0;
		virtual short		get_nr_reinicio(void) = 0;
		virtual std::string memoria_fiscal(int tipo, const std::string& inicio,
											const std::string& fim,
											bool completa, 
											int saida = ECF_SAIDA_ECF) = 0;
		virtual std::string leitura_x(int saida) = 0;
		virtual void		set_moeda_singular(const std::string& moeda) = 0;
		virtual void		set_moeda_plural(const std::string& moeda) = 0;
		virtual void		imprime_cheque(int valor_x, int valor_y, float valor,
											int ext1_x, int ext1_y, int ext2_x,
											int ext2_y, int fav_x, int fav_y,
											const std::string& favorecido,
											int local_x, int local_y, const
											std::string& localidade, int dia_x,
											int dia, int mes_x, int mes, 
											int ano_x, int ano, 
											const std::string& msg) = 0;
		virtual void		abre_rel_gerencial(const std::string& titulo) = 0;
		virtual short		get_id_rel_gerencial(const std::string& titulo) = 0;
		virtual void		txt_rel_gerencial(const std::string& msg) = 0;
		virtual void		txt_rel_gerencial(const std::string& msg, 
											const char* tag) = 0;
		virtual void		fecha_rel_gerencial(void) = 0;
		virtual void		set_rel_gerencial(const std::string& titulo) = 0;
		virtual void		aciona_guilhotina(int modo) = 0;
		virtual void		corte_auto(void) = 0;
		virtual bool		is_termica(void) = 0;
		virtual bool		is_paf_compliant(void) = 0;
		virtual void		set_tipo_corte(bool total) = 0;
		virtual float		get_grande_total(void) = 0;
		virtual bool		match_conf(void) = 0;
		virtual void		sangria(float valor) = 0;
		virtual void		suprimento(float valor, 
									const std::string& moeda = "Dinheiro") = 0;
		virtual void		set_horario_verao(void) = 0;
		virtual bool		is_horario_verao(void) = 0;
		virtual std::string	download_mfd(int tipo, const std::string& ini,
											const std::string& fim,
											int usr = 1) = 0;
		virtual void		converte_mfd(int tipo, const std::string& ini,
											const std::string& fim,
											const std::string& origem,
											const std::string& destino,
											int fmt = ECF_CONV_MFD_TEXTO,
											int usr = 1) = 0;
		virtual bool		pouco_papel(void) = 0;
		virtual long		contador_cupom_fiscal(void) = 0;
		virtual long		contador_tot_naofiscal(void) = 0;
		virtual long		contador_credito_debito(void) = 0;
		virtual long		contador_rel_gerenciais(void) = 0;
		virtual void		cancela_item(int idx = 0) = 0;
		virtual void		abre_recibo(const std::string& doc, 
										const std::string& nome,
										const std::string& endr) = 0;
		virtual void		abre_credito_debito(const std::string& fpgto, 
												float valor, unsigned int coo,
												const std::string& doc,
												const std::string& nome,
												const std::string& endr) = 0;
		virtual void		segunda_via_ccd(void) = 0;
		virtual void		reimprime_ccd(void) = 0;
		virtual void		cancela_recibo(const std::string& doc,
											const std::string& nome,
											const std::string& endr) = 0;
		virtual void		estorna_ccd(const std::string& doc,
										const std::string& nome,
										const std::string& endr) = 0;
		virtual void		reg_nao_fiscal(const std::string& tot_cnf, 
											float valor) = 0;
		virtual void		lcto_nao_fiscal(const std::string& moeda,
											float valor) = 0;
		virtual void		inicia_fechamento_nao_fiscal(unsigned char tipo,
															float valor) = 0;
		virtual void		fecha_recibo(const std::string& msg) = 0;
		virtual void		fecha_ccd(void) = 0;
		virtual void		cancela_nfiscal_pos(const std::string& moeda,
												float valor, unsigned int coo,
												unsigned int ccd,
												const std::string& doc,
												const std::string& nome,
												const std::string& endr) = 0;
		virtual bool		nao_fiscal_aberto(void) = 0;
		virtual std::string	get_tab_aliq(void) = 0;
		virtual std::string	get_tab_relger(void) = 0;
		virtual std::string	get_tab_totcnf(void) = 0;
		virtual std::string	get_tab_fpgto(void) = 0;
		
		virtual std::string	today_rec_path(std::string dir, 
											std::string prefix,
											int seq,
											std::string ext);
		
	protected:
		std::string			m_cfg_file;
};


#endif // _ECF_H_
