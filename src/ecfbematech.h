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

#ifndef _ECFBEMATECH_H_
#define _ECFBEMATECH_H_


#include <iomanip>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdio.h>
#include <monetary.h>
#include <ctype.h>
#include <string.h>

#include "datapair.h"
#include "ecf.h"
#include "bemafi.h"
#include "utils.h"
#include "fs.h"

#include "../config.h"


#define BEMATECH_OK					 1
#define BEMATECH_COMM_ERR			 0
#define BEMATECH_EXEC_FAILURE	    -1
#define BEMATECH_INVALID_PARAM	    -2
#define BEMATECH_MISSING_ALIQ	    -3
#define BEMATECH_MISSING_INI_FILE	-4
#define BEMATECH_SERIAL_OPEN_ERR	-5
#define BEMATECH_UNPLUGGED_ECF		-6
#define BEMATECH_TXT_WRITE_ERR		-8
#define BEMATECH_MISSING_FPGTO		-24
#define BEMATECH_NOT_OK_STATUS		-27

#define BEMATECH_NAO_FISCAL_ABERTO  48

#define BEMATECH_SIZE_FPGTO			16
#define BEMATECH_SIZE_TOTCNF		19
#define BEMATECH_SIZE_RELGER		17
#define BEMATECH_SIZE_NOME			30
#define BEMATECH_SIZE_ENDR			80
#define BEMATECH_MAX_COL			48
#define BEMATECH_TOTAL_TOTCNF		28
#define BEMATECH_TOTAL_ALIQS		16

#define BEMATECH_OUTPUT_PATH		"/tmp/"
#define BEMATECH_RETURN_FILE		"Retorno.txt"
#define BEMATECH_OUTFILE_NOT_FOUND  "Nao encontrado arquivo de retorno."
#define BEMATECH_MFD_NOT_SUPPORTED	"Download da MFD nao suportado."

#define IDX_NOT_FOUND				-1
#define MIN_IDX_VALID				0

#define BEMATECH_TXT_AVAILABLE		"<available>"
#define BEMATECH_TXT_UNAVAILABLE	"<unavailable>"
#define BEMATECH_TXT_SANGRIA		"Sangria"
#define BEMATECH_TXT_SUPRIMENTO		"Suprimento"

#define BEMATECH_SENSOR_POUCO_PAPEL		"sensor_pouco_papel"
#define BEMATECH_IGNORAR_POUCO_PAPEL	"ignorar_pouco_papel"
#define BEMATECH_REC_FOLDER				"diretorio_registro"


class ecf_bematech : public ecf
{
	public:
							ecf_bematech(const std::string& cfg_file = "");
							ecf_bematech(short size_fpgto, short size_totcnf,
							    short size_relger, 
							    const std::string& cfg_file = "");
		
		void				check(int retcode);
		bool				eval_status(bematech_status_mfd sts);
		bool				eval_retcode(int retcode);
		
		short				get_nr_loja(void);
		std::string			get_nr_serie(void);
		std::string			get_marca(void);
		std::string			get_modelo(void);
		short				get_nr_usuario(void);
		std::string			get_nr_serie_cript(void);
		short				get_nr_caixa(void);
		std::string			get_firmware(void);
		std::string			get_data_atual(void);
		std::string			get_hora_atual(void);
		std::string			get_data_mvto(void);
		std::string			get_data_ult_red(void);
		std::string			get_hora_ult_red(void);
		ecf_status			get_status_ecf(void);
		unsigned int		get_nr_cupom(void);
		short				get_id_forma_pgto(const std::string& fpgto);
		bool				get_vinc_forma_pgto(const short idx);
		short				get_totalizador_cnf(const std::string& tot);
		short				get_prox_totalizador(void);
		void				set_forma_pgto(const std::string& forma,
											const std::string& vinc);
		void				set_aliquota(const aliq_trib::aliquota& aliq);
		void				set_totalizador(const std::string& descr,
										const std::string& tipo);
		void				abre_cupom(const std::string& dcto,
										const std::string& nome,
										const std::string& endr);
		void				cancela_cupom(void);
		void				vende_item(const std::string& codprod,
										const std::string& nomeprod,
										const std::string& straliq,
										const std::string& unidade,
										float qtde,
										float valor, 
										float desc);
		virtual void		inicia_fechamento(float acresc);
		void				pagamento(const std::string& fpgto, float valor);
		void				fecha_cupom(const std::string& msg);
		void				abre_nfiscal_vinc(const std::string& fpgto,
												float valor, unsigned int coo);
		void				msg_nfiscal_vinc(const std::string& msg);
		void				fecha_nfiscal_vinc(void);
		void				abre_nfiscal_nvinc(const std::string& nome_tot,
												float valor,
												const std::string& moeda);
		short				get_max_col(void);
		void				abre_gaveta(void);
		void				reducao_z(void);
		bool				get_cupom_aberto(void);
		void				set_arredondamento(void);
		bool				get_imprime_cheques(void);
		short				get_nr_reducoes(void);
		std::string			dados_ult_reducao(void);
		short				get_nr_reinicio(void);
		std::string			memoria_fiscal(int tipo, const std::string& inicio,
											const std::string& fim,
											bool completa,
											int saida = ECF_SAIDA_ECF);
		std::string			leitura_x(int saida = ECF_SAIDA_ECF);
		void				set_moeda_singular(const std::string& moeda);
		void				set_moeda_plural(const std::string& moeda);
		void				imprime_cheque(int valor_x, int valor_y, float valor,
											int ext1_x, int ext1_y, int ext2_x,
											int ext2_y, int fav_x, int fav_y,
											const std::string& favorecido,
											int local_x, int local_y, const
											std::string& localidade, int dia_x,
											int dia, int mes_x, int mes, 
											int ano_x, int ano, 
											const std::string& msg);
		void				abre_rel_gerencial(const std::string& titulo);
		short				get_id_rel_gerencial(const std::string& titulo);
		void				txt_rel_gerencial(const std::string& msg);
		void				txt_rel_gerencial(const std::string& msg,
											const char* tag);
		void				fecha_rel_gerencial(void);
		void				set_rel_gerencial(const std::string& titulo);
		void				aciona_guilhotina(int modo = ECF_CORTE_PARCIAL);
		void				corte_auto(void);
		bool				is_termica(void);
		bool				is_paf_compliant(void);
		void				set_tipo_corte(bool total = false);
		float				get_grande_total(void);
		bool				match_conf(void);
		void				sangria(float valor);
		void				suprimento(float valor, 
										const std::string& moeda);
		void				set_horario_verao(void);
		bool				is_horario_verao(void);
		std::string			download_mfd(int tipo, const std::string& ini,
											const std::string& fim, int usr = 1);
		void				converte_mfd(int tipo, const std::string& ini,
											const std::string& fim,
											const std::string& origem,
											const std::string& destino,
											int fmt = ECF_CONV_MFD_TEXTO,
											int usr = 1);
		bool				pouco_papel(void);
		long				contador_cupom_fiscal(void);
		long				contador_tot_naofiscal(void);
		long				contador_credito_debito(void);
		long				contador_rel_gerenciais(void);
		void				cancela_item(int idx = 0);
		void				abre_recibo(const std::string& doc,
										const std::string& nome,
										const std::string& endr);
		void				abre_credito_debito(const std::string& fpgto, 
												float valor, unsigned int coo,
												const std::string& doc,
												const std::string& nome,
												const std::string& endr);
		void				segunda_via_ccd(void);
		void				reimprime_ccd(void);
		void				cancela_recibo(const std::string& doc = "",
											const std::string& nome = "",
											const std::string& endr = "");
		void				estorna_ccd(const std::string& doc,
										const std::string& nome,
										const std::string& endr);
		void				reg_nao_fiscal(const std::string& tot_cnf,
											float valor);
		void				lcto_nao_fiscal(const std::string& moeda,
											float valor);
		virtual void		inicia_fechamento_nao_fiscal(unsigned char tipo,
															float valor);
		void				fecha_recibo(const std::string& msg);
		void				fecha_ccd(void);
		void				cancela_nfiscal_pos(const std::string& moeda,
												float valor, unsigned int coo, 
												unsigned int ccd,
												const std::string& doc,
												const std::string& nome,
												const std::string& endr);
		bool				nao_fiscal_aberto(void);
		std::string			get_tab_aliq(void);
		std::string			get_tab_relger(void);
		std::string			get_tab_totcnf(void);
		std::string			get_tab_fpgto(void);

	protected:
		void				set_dont_pay(void);
		void				reset_dont_pay(void);
		bool				get_dont_pay(void);
		void				get_data_hora_atual(char* data, char* hora);
		void				get_data_hora_ult_red(char* data, char* hora);
		void				get_data_hora_ult_red(void);
		void				set_status(const bematech_status_mfd sts);

		short				idx_element(const std::string& item,
											std::vector<std::string>* vct) 
											NO_THROW;
		void				add_element(std::vector<std::string>* vct,
											const std::string& item);
		
		short				get_idx_aliquota(const aliq_trib::aliquota& aliq);

		std::string			format_valor(float valor, const char* fmt);
		bool				aliq_is_icms(short idx);
		bool				aliq_is_iss(short idx);
		std::string			format_date(const std::string& valor);
		std::string			m_retmsg;
		std::string			m_stsmsg;
		ecf_status			m_sts;
		bematech_status_mfd	m_status;
		std::string			m_marca;
		std::string			m_modelo;
		std::string			m_tipo;
		std::string			m_dt_ult_red;
		std::string			m_hr_ult_red;
		std::string			format_monetary(float valor, const char* fmt);

		std::vector<std::string>				m_fpgto;
		std::vector<bool>						m_vinc_ccd;
		std::vector<std::string>				m_tot_cnf;
		std::map<short, aliq_trib::aliquota>	m_aliqs;
		std::vector<std::string>				m_relger;
		short									m_size_fpgto;
		short									m_size_totcnf;
		short									m_size_relger;

		bool				fake(void);
		void				set_pgto_seq(short val);
		short				get_pgto_seq(void);
		bool				ignorar_pouco_papel(void);
		datapair			m_specific;

	private:
		void				load_specific(void);
		bool				m_dont_pay;
		bool				m_fake;
		bool				m_ignorar_pouco_papel;
		std::string			m_rec_folder;
		short				m_pgto_seq;
};


#endif // _ECFBEMATECH_H_
