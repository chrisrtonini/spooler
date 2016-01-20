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

#include "ecfbematechcv0909.h"


ecf_bematech_cv0909::ecf_bematech_cv0909(void) : 
	ecf_bematech(BEMATECH_SIZE_FPGTO_CV0909, BEMATECH_SIZE_TOTCNF_CV0909, 
	    BEMATECH_SIZE_RELGER_CV0909)
{
	debug_log("ecf_bematech::ecf_bematech_cv0909(...)");
}


/**
 \brief Obtém o índice de um totalizador não fiscal.
 \param tot String contendo o nome/descrição do totalizador não fiscal.
 \returns Valor inteiro, representando o índice, dentro da tabela de 
 totalizadores não fiscais, correspondente à descrição passada como argumento.
 */
short ecf_bematech_cv0909::get_totalizador_cnf(const std::string& tot)
{
	debug_log(std::string("ecf_bematech_cv0909::get_totalizador_cnf(") +
	    dquote(tot.c_str()) + std::string(")"));

	char				buffer_cnf[600];
	short				idx = idx_element(tot.substr(0, m_size_totcnf), 
										&m_tot_cnf);
	short				i = 0;
	short				rec_size = BEMATECH_SIZE_TOTCNF + 1; // O "1" a mais, 
									// corresponde ao separador "," (virgula)
	short				total = (sizeof(buffer_cnf) / rec_size);
	std::string			one_cnf;
	char				buff_um[BEMATECH_SIZE_TOTCNF + 1];

	if (idx == IDX_NOT_FOUND)   {
		try {
			xdebug_log("Bematech_FI_VerificaTotalizadoresNaoFiscaisMFD(...)");
		
			memset(buffer_cnf, 0, sizeof(buffer_cnf));
			check(Bematech_FI_VerificaTotalizadoresNaoFiscaisMFD(buffer_cnf));

			m_tot_cnf.clear();

			do  {
				memset(buff_um, 0, sizeof(buff_um));
				strncpy(buff_um, static_cast<char*>(buffer_cnf+(i*rec_size)),
				    m_size_totcnf);

				one_cnf = buff_um;
				trim(one_cnf);
				if (one_cnf.empty())	{
					xdebug_log(std::string("Adicionando CNF (vazio) # ") +
					    into_string(i) + std::string(" = ") +
					    dquote(BEMATECH_TXT_AVAILABLE));
					add_element(&m_tot_cnf, BEMATECH_TXT_AVAILABLE);
				}
				else	{
					xdebug_log(std::string("Adicionando CNF # ") +
					    into_string(i) + std::string(" = ") +
					    dquote(one_cnf.c_str()));
					
					add_element(&m_tot_cnf, one_cnf.c_str());
					if (str_iequals(tot.substr(0, m_size_totcnf), one_cnf))
						idx = i;
				}
				i++;
			} while (i < total);
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech_cv0909::get_totalizador_cnf(...) = ") +
	    (idx < MIN_IDX_VALID ? std::string("Nao encontrado!") : 
			    into_string(idx + 1)));

	// Como a biblioteca da Bematech roda em "modo de compatibilidade", ela
	// implementa um incremento de 2 posições como forma de "desviar" os 
	// registros de "FUNDO DE TROCO" e "SANGRIA", que nos modelos anteriores,
	// se encontravam ao final da tabela.
	// Portanto, após o processamento da lista, estamos DECREMENTANDO em dois
	// o valor encontrado, para que o resultado seja correto ao ser passado
	// para o ECF.
	return (idx < MIN_IDX_VALID ? idx : ((idx + 1) - 2));
}


/**
 \brief Obtém o índice de uma forma de pagamento.
 \param fpgto Descrição da forma de pagamento.
 \returns Valor inteiro, iniciando em ZERO, indicando a posição, dentro da 
 tabela de formas de pagamento, onde se encontra a forma passada como argumento.
 */
short ecf_bematech_cv0909::get_id_forma_pgto(const std::string& fpgto)
{
	debug_log(std::string("ecf_bematech_cv0909::get_id_forma_pgto(") +
	    dquote(fpgto.c_str()) + std::string(")"));
	
	char					buffer_fpgto[920];
	char					buffer_reg[16+14+14+1+1];
	short					idx = idx_element(fpgto.substr(0, m_size_fpgto), 
										&m_fpgto);
	short					rec_size = sizeof(bematech_reg_fpgto_mfd);
	short					total = 920 / rec_size;
	short					i = 0;
	short					j = 0;
	short					x = 0;
	bool					found = false;
	char					descr[BEMATECH_SIZE_FPGTO+1];
	std::string				fmt_dsc;

	// Caso a forma de pagamento não tenha sido encontrada
	// no cache do objeto, solicita a carga da tabela de formas de
	// pagamento do ECF.
	if (idx == IDX_NOT_FOUND)  {
		try {
			xdebug_log("Bematech_FI_VerificaFormasPagamentoMFD(...)");
			check(Bematech_FI_VerificaFormasPagamentoMFD(buffer_fpgto));

			m_fpgto.clear();

			for (i = 0; i < total; i++)	{

				memset(buffer_reg, 0, sizeof(buffer_reg));
				x = 0;

				do  {
					if (buffer_fpgto[j] != ',')
						buffer_reg[x++] = buffer_fpgto[j];
					else
						found = true;
					
					j++;
				} while ((j < sizeof(buffer_fpgto)) && (!found));
				found = false;

				memset(descr, 0, sizeof(descr));

				xdebug_log(into_string(strlen(buffer_reg)) + std::string(": ")
				    + dquote(buffer_reg));
				xdebug_log(into_string(sizeof(buffer_reg)-1));
				
				if (strlen(buffer_reg) < (sizeof(buffer_reg)-1))
					strncpy(descr, BEMATECH_TXT_UNAVAILABLE, m_size_fpgto);
				else
					strncpy(descr, buffer_reg, m_size_fpgto);
				
				fmt_dsc = descr;
				trim(fmt_dsc);

				if (!fmt_dsc.empty())   {
					xdebug_log(std::string("Adicionando Forma de Pgto # ") +
					    into_string(i) + std::string(": ") +
					    dquote(fmt_dsc.c_str()));
					add_element(&m_fpgto, fmt_dsc);
				}
				else	{
					xdebug_log(std::string("Adicionando Forma de Pgto # ") +
					    into_string(i) + std::string(": ") +
					    dquote(BEMATECH_TXT_AVAILABLE));
					add_element(&m_fpgto, BEMATECH_TXT_AVAILABLE);
				}

				if (str_iequals(fpgto.substr(0, m_size_fpgto), fmt_dsc)) {
					idx = i;
				}
			}
		}
		catch (...) {
			throw;
		}
	}

	xdebug_log(std::string("ecf_bematech_cv0909::get_id_forma_pgto(...) = ") + 
	    (idx < MIN_IDX_VALID ? std::string("Nao encontrada!") : 
			    into_string(idx + 1)));

	return (idx < MIN_IDX_VALID ? idx : idx + 1);
}


/**
 \brief Obtém o índice para a próxima forma de pagamento (a ser cadastrada).
 \returns Valor inteiro representando a próxima posição livre na tabela.
 */
short ecf_bematech_cv0909::get_prox_forma_pgto(void)
{
	debug_log("ecf_bematech_cv0909::get_prox_forma_pgto()");

	short   idx = IDX_NOT_FOUND;
	short   i = 0;
	std::vector<std::string>::iterator it;

	// Força a carga da tabela de formas de pagamento,
	// caso esta ainda não tenha sido carregada.
	get_id_forma_pgto("");
	
	if (!m_fpgto.empty())   {
		for (it = m_fpgto.begin(); it != m_fpgto.end(); it++)   {
			if (idx < MIN_IDX_VALID)	{
				xdebug_log(std::string("Examinando # ") + into_string(i) +
				    std::string(" = ") + dquote((*it).c_str()));
				if (*it == BEMATECH_TXT_AVAILABLE)
					idx = i;
			}
			i++;
		}
	}
	else
		xdebug_log("ecf_bematech_cv0909::get_prox_forma_pgto() Tabela vazia!");
/*
	short   idx = 0;
	std::vector<std::string>::iterator it;
	 
	// Força a carga da tabela de formas de pagamento,
	// caso esta ainda não tenha sido carregada.
	get_id_forma_pgto("");

	if (!m_fpgto.empty())   {
		idx = m_fpgto.size();
		if (idx > 0)	{
			int x = 0;
			for (it = m_fpgto.begin(); it != m_fpgto.end(); it++)   {
				xdebug_log(std::string("Ocupada # ") + into_string(x++) +
				    std::string(": ") + dquote((*it).c_str()));
			}
		}
	}
*/

	xdebug_log(std::string("ecf_bematech_cv0909()::get_prox_forma_pgto() = ") +
	    into_string(idx));

	return idx;
}


/**
 \brief Grava uma nova forma de pagamento no ECF.
 \param forma Nome/descrição da nova forma de pagamento (moeda).
 \param vinc Flag que indica vinculação com CDC, para uso de TEF.
 */
void ecf_bematech_cv0909::set_forma_pgto(const std::string& forma, 
    const std::string& vinc)
{
	debug_log(std::string("ecf_bematech_cv0909::set_forma_pgto(") +
	    dquote(forma.c_str()) + std::string(", ") + dquote(vinc.c_str()) +
	    std::string(")"));

	try {
		int idx = get_id_forma_pgto(forma);

		if (idx < MIN_IDX_VALID)	{
			idx = get_prox_forma_pgto() + 1;
			
			std::string tmp = forma.substr(0, m_size_fpgto);
			trim(tmp);

			std::string flg = "0";
			if (str_iequals(vinc, std::string("true")) || (vinc[0] == '1'))
				flg = "1";
		
			xdebug_log(
				std::string("Bematech_FI_ProgramaFormaPagamentoCV0909(") +
					into_string(idx).c_str() + std::string(", ") +
					dquote(tmp.c_str()) + std::string(", ") + 
					dquote(flg.c_str()) + std::string(")"));

			if (!fake())
				check(Bematech_FI_ProgramaFormaPagamentoCV0909(
				    into_string(idx).c_str(), tmp.c_str(), atoi(flg.c_str())));
			else
				debug_log(
				    std::string(
					    "FAKE >> Bematech_FI_ProgramaFormaPagamentoCV0909"));

			m_fpgto.clear();
		}
/*
		 // Ao tentar adicionar uma forma de pagamento, se for constatada sua
		 // presença na memória do equipamento, NÃO retornamos erro, pois o
		 // resultado prático é o mesmo que tivesse sido recém adicionada,
		 // ou seja, a forma de pagamento está (ou passa a figurar) na tabela
		 // do ECF.
		else
			throw ecf_exp(ECF_FPGTO_PRESENT);
*/
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Cadastra um novo totalizador não sujeito ao ICMS
 \param descr Nome/descrição para o novo totalizador
 \param tipo  Tipo do totalizador, sendo "E" para "entrada" ou "S" para "saida"
 */
void ecf_bematech_cv0909::set_totalizador(const std::string& descr,
    const std::string& tipo)
{
	debug_log(std::string("ecf_bematech_cv0909::set_totalizador(") +
	    dquote(descr.c_str()) + std::string(", ") + dquote(tipo.c_str()) +
	    std::string(")"));

	try {
		int idx = get_totalizador_cnf(descr);

		if (idx < MIN_IDX_VALID)	{
			idx = get_prox_totalizador() + 1;

			std::string cidx = format_valor(static_cast<float>(idx), "%02.0f");
			std::string tmp = descr.substr(0, m_size_totcnf);
			trim(tmp);
	
			xdebug_log(
				std::string("Bematech_FI_NomeiaTotalizadorNaoSujeitoIcmsCV0909(")
				+ into_string(idx) + std::string(", ") + 
				dquote(tmp.c_str()) + std::string(", ") + 
				dquote(tipo.c_str()) + std::string(")"));

			if (!fake())
				check(Bematech_FI_NomeiaTotalizadorNaoSujeitoIcmsCV0909(
				    idx, tmp.c_str(), tipo.c_str()));
			else
				xdebug_log(std::string("FAKE >> ") + std::string(
				    "Bematech_FI_NomeiaTotalizadorNaoSujeitoIcmsCV0909"));

			m_tot_cnf.clear();
		}
/*
		 // Ao tentar adicionar um totalizador não fiscal, se for constatada 
		 // sua presença na memória do equipamento, NÃO retornamos erro, pois o
		 // resultado prático é o mesmo que tivesse sido recém adicionado, ou
		 // seja, o totalizador não fiscal está (ou passa a figurar) na tabela
		 // do ECF.
		else
			throw ecf_exp(ECF_TOT_CNF_PRESENT);
*/
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Adiciona um relatório gerencial.
 \param titulo Nome/descrição do novo relatório.
 */
void ecf_bematech_cv0909::set_rel_gerencial(const std::string& titulo)
{
	debug_log(std::string("ecf_bematech_cv0909::set_rel_gerencial(") +
	    dquote(titulo.c_str()) + std::string(")"));
	
	try {
		short nidx = get_id_rel_gerencial(titulo);
		if (nidx < MIN_IDX_VALID)   {
			nidx = m_relger.size() + 1;
			std::string cidx;
			cidx = format_valor(static_cast<float>(nidx), "%02.0f");
			std::string tmp = titulo.substr(0, m_size_relger);
			trim(tmp);

			xdebug_log(std::string("Bematech_FI_NomeiaRelatorioGerencialCV0909(") + 
			    dquote(cidx.c_str()) + std::string(", ") + 
				dquote(tmp.c_str()) + std::string(")"));

			if (!fake())
				check(Bematech_FI_NomeiaRelatorioGerencialCV0909(cidx.c_str(), 
				    tmp.c_str()));
			else
				debug_log(
				    std::string(
					    "FAKE >> Bematech_FI_NomeiaRelatorioGerencialCV0909"));

			m_relger.clear();
		}
/*
		 // Ao tentar adicionar um relatório gerencial, se for constatada 
		 // sua presença na memória do equipamento, NÃO retornamos erro, pois o
		 // resultado prático é o mesmo que tivesse sido recém adicionado, ou
		 // seja, o relatório gerencial está (ou passa a figurar) na tabela
		 // do ECF.
		else
			throw ecf_exp(ECF_REL_GER_PRESENT);
*/
	}
	catch (...) {
		throw;
	}
}


