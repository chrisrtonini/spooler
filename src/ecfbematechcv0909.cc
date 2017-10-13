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


ecf_bematech_cv0909::ecf_bematech_cv0909(const std::string& cfg_file) : 
	ecf_bematech(BEMATECH_SIZE_FPGTO_CV0909, BEMATECH_SIZE_TOTCNF_CV0909, 
	    BEMATECH_SIZE_RELGER_CV0909, cfg_file)
{
	debug_log(std::string("ecf_bematech::ecf_bematech_cv0909(...,") + 
	          dquote(cfg_file.c_str()) + std::string(")"));
	
	reset_dont_pay();

	/*try {
		load_specific();
	}
	catch (...) {
		throw;
	}*/
}


/**
 \brief Obtém o índice de uma alíquota tributária.
 \param aliq Alíquota cujo índice se deseja.
 \returns Valor inteiro, indicando a posição da alíquota no ECF, ou -1, caso
 esta não seja encontrada.
 */
short ecf_bematech_cv0909::get_idx_aliquota(const aliq_trib::aliquota& aliq)
{
	debug_log(std::string("ecf_bematech::get_idx_aliquota(") +
	    const_cast<aliq_trib::aliquota&>(aliq).get_serialized() + std::string(")"));
	
	short										idx = IDX_NOT_FOUND,
												x = 0;
	std::map<short, aliq_trib::aliquota>::iterator  i;
	char										ret_aliq[(BEMATECH_TOTAL_ALIQS_CV0909*5)+1];
	char										idx_iss[91];
	unsigned char								flag[BEMATECH_TOTAL_ALIQS_CV0909];
	std::string									one_idx;
	std::string									one_aliq;
	aliq_trib::aliquota							obj_aliq;

	// Cache de alíquotas vazio... carregar...
	if (m_aliqs.empty())	{

		// Vetor de flags de vinculação ao ISS, convertidos
		// a partir de retorno do equipamento.
		for (x = 0; x < BEMATECH_TOTAL_ALIQS_CV0909; x++)	{
			flag[x] = aliq_trib::TRIBUTADO_ICMS;
		}
		
		try {
			memset(idx_iss, 0, sizeof(idx_iss));
			
			xdebug_log("Bematech_FI_VerificaIndiceAliquotasIss(...)");
			
			// Obtenção dos flags de vinculação de ISS
			check(Bematech_FI_VerificaIndiceAliquotasIss(idx_iss));

			short cm = 0;
			short pv = 0;
			short t = strlen(idx_iss);
			short j = 0;
			char buft[BEMATECH_TOTAL_ALIQS_CV0909];
			do  {
				memset(buft, 0, sizeof(buft));
				if ((idx_iss[j] == ',') || (j == t))  {
					cm = j;
					strncpy(buft, idx_iss+pv, (cm - pv));
					pv = (cm + 1);
					one_idx = buft;
					trim(one_idx);
					// Incluído teste para evitar falsa entrada, verificada
					// quando não há nenhuma alíquota de ISS cadastrada.
					if (!one_idx.empty())   {
						flag[atoi(one_idx.c_str())+29] = aliq_trib::TRIBUTADO_ISS;
						xdebug_log(std::string("Aliquota # ") + one_idx +
							std::string(" = ISS"));
					}
				}
				j++;
			} while (j <= t);

			memset(ret_aliq, 0, sizeof(ret_aliq));

			xdebug_log("Bematech_FI_RetornoAliquotasCV0909(...)");
			
			// Obtenção da tabela de alíquotas tributárias...
			check(Bematech_FI_RetornoAliquotasCV0909(ret_aliq));

			// Atribuição da cache, associando um valor de índice,
			// a um objeto 'aliquota', contendo o tipo e o percentual.
			// TODO: encontrar uma forma de alimentar também o atributo 'cst'.
			x = 0;
			pv = 0;
			cm = 0;
			j = 0;
			t = strlen(ret_aliq);
			std::string fmt;
			do  {
				if ((ret_aliq[j] == ',') || (j == t)) {
					cm = j;
					strncpy(buft, ret_aliq+pv, (cm - pv));
					pv = (cm + 1);
					fmt = buft;
					// Tamanhos abaixo fixos em "2" (duas) posições,
					// pois as alíquotas encontram-se no formato: "99.99"
					one_aliq = fmt.substr(0, 2);
					one_aliq += ".";
					one_aliq += fmt.substr(2);
					obj_aliq.set(flag[x], 0, atof(one_aliq.c_str()));
					m_aliqs.insert(std::pair<short, aliq_trib::aliquota>
					    (x++, obj_aliq));
					xdebug_log(std::string("Aliquota # ") + into_string(x) +
						std::string(" = ") + one_aliq);
				}
				j++;
			} while (j <= t);
		}
		catch (...) {
			throw;
		}
	}

	// Varrer a lista, buscando compatibilidade de 'tipo' e 'percentual'...
	// caso não encontre, o retorno ser '-1'
	for (i = m_aliqs.begin(); i != m_aliqs.end(); i++)  {
		xdebug_log(std::string("Comparando ") + 
		    into_string(i->second.get_tipo()) + std::string(":") +
		    into_string(i->second.get_percentual()) + std::string(" e ") +
		    into_string(aliq.get_tipo()) + std::string(":") +
		    into_string(aliq.get_percentual()));
		if ((i->second.get_tipo() == aliq.get_tipo()) &&
				(i->second.get_percentual() == aliq.get_percentual()))   {
			idx = i->first;
			xdebug_log(std::string("Aliquota selecionada: ") + into_string(idx));
		}
	}

	xdebug_log(std::string("ecf_bematech::get_idx_aliquota(...) = ") +
	    (idx <MIN_IDX_VALID ? std::string("Nao encontrada!") : 
			    into_string(idx + 1)));
	
	return (idx < MIN_IDX_VALID ? idx : idx + 1);
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
	// Alterção de 19/02/2016:
	// Não é mais realizado o decréscimo, pois a versão 7.0.339-0SuSE11.3 da
	// biblioteca, não está mais realizando o deslocamento de 2 posições,
	// descrito acima. Portanto, para fins de documentação, o "- 2" foi 
	// substituído por "- 0".
	return (idx < MIN_IDX_VALID ? idx : ((idx + 1) - 0));
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
	bool					vinc_ccd = false;
	bematech_reg_fpgto_mfd* reg;
	
	// Caso a forma de pagamento não tenha sido encontrada
	// no cache do objeto, solicita a carga da tabela de formas de
	// pagamento do ECF.
	if (idx == IDX_NOT_FOUND)  {
		try {
			xdebug_log("Bematech_FI_VerificaFormasPagamentoMFD(...)");
			check(Bematech_FI_VerificaFormasPagamentoMFD(buffer_fpgto));

			m_fpgto.clear();
			m_vinc_ccd.clear();

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

				reg = reinterpret_cast<bematech_reg_fpgto_mfd*>(buffer_reg);
				memset(descr, 0, sizeof(descr));

				if (strlen(buffer_reg) < (sizeof(buffer_reg)-1))
					strncpy(descr, BEMATECH_TXT_UNAVAILABLE, m_size_fpgto);
				else
					strncpy(descr, buffer_reg, m_size_fpgto);

				vinc_ccd = (reg->flag_uso_vinc == '1' ? true : false);
				
				fmt_dsc = descr;
				trim(fmt_dsc);

				if (!fmt_dsc.empty())   {
					xdebug_log(std::string("Adicionando Forma de Pgto # ") +
					    into_string(i) + std::string(": ") +
					    dquote(fmt_dsc.c_str()) +
					    std::string(vinc_ccd ? "(V)" : ""));
					add_element(&m_fpgto, fmt_dsc);
					m_vinc_ccd.push_back(vinc_ccd);
				}
				else	{
					xdebug_log(std::string("Adicionando Forma de Pgto # ") +
					    into_string(i) + std::string(": ") +
					    dquote(BEMATECH_TXT_AVAILABLE));
					add_element(&m_fpgto, BEMATECH_TXT_AVAILABLE);
					m_vinc_ccd.push_back(false);
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


/**
 \brief Gera arquivo de texto contendo a tabela de alíquotas tributárias
 do equipamento.
 \returns Path do arquivo gerado.
 */
std::string ecf_bematech_cv0909::get_tab_aliq(void)
{
	std::string			arq;
	std::ofstream		out;
	std::map<short, aliq_trib::aliquota>::iterator 
						i;
	short				idx = 0;
	aliq_trib::aliquota	aliq("T:00:8888");
	
	try {
		// Carga da tabela de alíquotas...
		get_idx_aliquota(aliq);

		// Definição do nome do arquivo a ser gerado
		arq = std::string(BEMATECH_OUTPUT_PATH) + 
				fs::random_name("bema_aliq_");
		
		out.open(arq.c_str());

		for (i = m_aliqs.begin(); i != m_aliqs.end(); i++)  {
			std::string ln_tmp = "";
			std::string fmt_idx = format_monetary(
							static_cast<float>(++idx), "%!=0#2.0i");
			std::string fmt_pct = format_monetary(
							i->second.get_percentual(), "%!=0#2.2i");
			ln_tmp = fmt_idx + std::string(":") +
						into_string(i->second.get_tipo()) +
						std::string(":") +
						fmt_pct;
			
			//std::cerr << ln_tmp << std::endl;
			
			out << ln_tmp << std::endl;
		}
		
		out.close();
	}
	catch (...) {
		throw;
	}
	
	return arq.c_str();
}


/**
 \brief Registra um totalizador, em um documento não fiscal não vinculado.
 \param tot_cnf Nome/descrição do totalizador não fiscal.
 \param valor Valor a ser lançado no totalizador.
 */
void ecf_bematech_cv0909::reg_nao_fiscal(const std::string& tot_cnf, float valor)
{
	debug_log(std::string("ecf_bematech_cv0909::reg_nao_fiscal(") +
	    dquote(tot_cnf.c_str()) + std::string(", ") +
	    into_string(valor) + std::string(")"));

	std::string cnf_saida = m_specific.get_value_by_key(BEMATECH_SPEC_CNF_SAIDA);
	std::string arg = tot_cnf.substr(0, m_size_totcnf);

	trim(arg);

	xdebug_log(std::string("ecf_bematech_cv0909::reg_nao_fiscal(...): buscar ") +
	           dquote(arg.c_str()) + std::string(" em ") + 
	           dquote(cnf_saida.c_str()));

	if (cnf_saida.find(arg) != std::string::npos)	{
		set_dont_pay();
		debug_log("ecf_bematech_cv0909::reg_nao_fiscal(...): found!");
	}

	ecf_bematech::reg_nao_fiscal(tot_cnf, valor);
}


/**
 \brief Efetua uma forma de pagamento, no cupom aberto.
 \param fpgto Nome/descrição da forma de pagamento (15 posições).
 \param valor Valor do pagamento.
 */
void ecf_bematech_cv0909::pagamento(const std::string& fpgto, float valor)
{
	debug_log(std::string("ecf_bematech_cv0909::pagamento(") + dquote(fpgto.c_str()) +
	    std::string(", ") + into_string(valor) + std::string(")"));

	if (get_dont_pay()) {
		warn_log(std::string("ecf_bematech_cv0909::pagamento(...): ") +
		         std::string("Pagamento suprimido pelo driver."));
	}
	else	{
		try {
			short idx = get_id_forma_pgto(fpgto);

			if (idx < MIN_IDX_VALID)
				throw ecf_exp(ECF_FPGTO_MISSING);
		
			std::string cidx = format_valor(static_cast<float>(idx), "%02.0f");
			std::string cvlr = format_valor(valor, "%11.2f");

			xdebug_log(std::string("Bematech_FI_EfetuaFormaPagamentoIndice(") +
				dquote(cidx.c_str()) + std::string(", ") +
				dquote(cvlr.c_str()) + std::string(")"));

			if (!fake())
				check(Bematech_FI_EfetuaFormaPagamentoIndice(cidx.c_str(), 
					cvlr.c_str()));
			else
				debug_log("FAKE >> Bematech_FI_EfetuaFormaPagamentoIndice");

			debug_log(std::string("m_use_ccd.push_back [") +
				into_string(idx) + std::string("] = ") + 
				std::string((get_vinc_forma_pgto(idx) ? "true" : "false")));
			m_use_ccd.push_back(get_vinc_forma_pgto(idx));
		}
		catch (...) {
			throw;
		}
	}
}


/**
 \brief Abre comprovante de Crédito ou Débito.
 \param fpgto Descrição da forma de pagamento (vinculada pelo Cupom Fiscal).
 \param valor Valor da moeda no Cupom Fiscal.
 \param coo Número do cupom ao qual está vinculado.
 \param doc CPF ou CNPJ do cliente.
 \param nome Nome ou Razão Social do cliente.
 \param endr Endereço do cliente.
 \note Os argumentos "valor" e "coo" são mantidos apenas por compatibilidade,
 já que não são utilizados pelo método.
 */
void ecf_bematech_cv0909::abre_credito_debito(const std::string& fpgto, 
	float valor, unsigned int coo, const std::string& doc, 
	const std::string& nome, const std::string& endr)
{
	debug_log(std::string("ecf_bematech_cv0909::abre_credito_debito(") +
	    dquote(fpgto.c_str()) + std::string(", ") +
	    into_string(valor) + std::string(", ") + into_string(coo) +
	    std::string(", ") + dquote(doc.c_str()) + std::string(", ") +
	    dquote(nome.c_str()) + std::string(", ") + dquote(endr.c_str()) +
	    std::string(")"));
	
	try {
		short idx		   = get_id_forma_pgto(fpgto);
		short seq		   = get_next_pgto_ccd();
		std::string cidx   = format_valor(idx, "%02.0f");

		xdebug_log(
		    std::string("Bematech_FI_AbreComprovanteNaoFiscalVinculadoCV0909(") +
		           into_string(seq) + std::string(", ") +
		           dquote(cidx.c_str()) + std::string(", 1, 1 ") +
		           dquote(doc.c_str()) + std::string(", ") +
		           dquote(nome.substr(0, BEMATECH_SIZE_NOME).c_str()) +
		           std::string(", ") +
		           dquote(endr.substr(0, BEMATECH_SIZE_ENDR).c_str()) +
		           std::string(")"));

		if (!fake())
			check(Bematech_FI_AbreComprovanteNaoFiscalVinculadoCV0909(
				seq, cidx.c_str(), 1, 1, doc.c_str(),
				nome.substr(0, BEMATECH_SIZE_NOME).c_str(),
				endr.substr(0, BEMATECH_SIZE_ENDR).c_str()));
		else
			debug_log("FAKE >> Bematech_FI_AbreComprovanteNaoFiscalVinculadoCV0909");
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a seqüência do pagamento para o próximo CCD.
 \details Percorre um vetor contendo flags (booleanos) cujos valores, associados
 a cada meio de pagamento empregado no último cupom fiscal, ou comprovante não
 fiscal não vinculado, buscando pela próxima seqüência válida para emissão de
 CCD.
 \returns Valor inteiro, representando a seqüência da moeda ou -1 caso nenhuma
 moeda utilizada tenha flag de vinculação.
 */
short ecf_bematech_cv0909::get_next_pgto_ccd(void)
{
	debug_log("ecf_bematech_cv0909::get_next_pgto_ccd()");
	
	std::vector<bool>::iterator it;
	short						idx = IDX_NOT_FOUND,
								i = (get_pgto_seq() + 1),
								j = 0;
	bool						out_of_seq = true;

	xdebug_log(std::string("ecf_bematech_cv0909::get_next_pgto_ccd() ") +
	           std::string("Iniciando em ") + dquote(into_string(i).c_str()));
	try {
		for (it = m_use_ccd.begin(); it != m_use_ccd.end(); it++)   {
			out_of_seq = false;
			xdebug_log(std::string("ecf_bematech_cv0909::get_next_pgto_ccd() ") +
			           std::string("Indice: ") + into_string(j) +
			           std::string((*it) ? " True" : " False"));
			//xdebug_log(std::string("Confira i:[") + into_string(i) +
			//	std::string("] j:[") + into_string(j) + 
			//	std::string((*it ? "] true" : "] false")));
			if ((j >= i) && (*it)) {
				idx = j;
				break;
			}
			j++;
		}

		// Caso o spooler seja reiniciado, a implementacao a seguir
		// visa tentar (na sorte) a impressao do CCD.
		// Provavelmente, operacoes com multiplos cartoes NAO FUNCIONARAO!
		if (out_of_seq)	{
			idx++;
		}
		
		set_pgto_seq(idx);

		if (idx == IDX_NOT_FOUND)
			throw ecf_exp(ECF_CCD_UNAVAILABLE);
	}
	catch (...) {
		throw;
	}

	debug_log(std::string("ecf_bematech_cv0909::get_next_pgto_ccd() = ") +
	          dquote(into_string(idx+1).c_str()));
	
	return (idx != IDX_NOT_FOUND ? idx + 1 : idx);
}


/**
 \brief Subtotaliza o cupom fiscal aberto, e atribui valor de acréscimo.
 \details Limpa vetor de formas de pagamento, utilizado para controlar sua 
 sequencia, para abertura de CCDs.
 \param acresc Valor do acréscimo.
 */
void ecf_bematech_cv0909::inicia_fechamento(float acresc)
{
	debug_log(std::string("ecf_bematech_cv0909::inicia_fechamento(") +
	    into_string(acresc) + std::string(")"));

	try {
		// Subtotaliza cupom, habilitando lançamento de acréscimo e/ou
		// desconto
		// NO CASO DA *SGI*, APENAS *ACRESCIMO*
		xdebug_log("Bematech_FI_SubTotalizaCupomMFD()");

		if (!fake())	{
			check(Bematech_FI_SubTotalizaCupomMFD());
		}
		else
			debug_log("FAKE >> Bematech_FI_SubTotalizaCupomMFD");

		// O lançamento de acréscimo, somente é possível entre o comando de
		// "subtotal" e o de "total"
		const char* flag = ACRESCIMO;
		const char* tipo = VALOR;
		std::string tmp_acr = format_valor(acresc, "%11.2f");
		
		xdebug_log(std::string("Bematech_FI_AcrescimoDescontoSubtotalCV0909(" +
			dquote(flag) + std::string(", ") + dquote(tipo) + std::string(", ") +
			dquote(tmp_acr.c_str()) + std::string(")")));
		
		if (!fake())	{
			check(Bematech_FI_AcrescimoDescontoSubtotalCV0909(flag, tipo,
				tmp_acr.c_str()));
		}
		else
			debug_log("FAKE >> Bematech_FI_AcrescimoDescontoSubtotalCV0909");

		// Tendo terminado o lançamento de acréscimo, é necessário totalizar
		// o cupom, de maneira a permitir o lançamento das formas de pagamento.
		xdebug_log("Bematech_FI_TotalizaCupomMFD()");

		if (!fake())	{
			check(Bematech_FI_TotalizaCupomMFD());
		}
		else
			debug_log("FAKE >> Bematech_FI_TotalizaCupomMFD");
	}
	catch (...) {
		throw;
	}
	
	//debug_log("m_use_ccd.clear()");
	m_use_ccd.clear();
	set_pgto_seq(IDX_NOT_FOUND);
}


/**
 \brief Subtotaliza recibo (comprovante não fiscal).
 \details Limpa vetor de formas de pagamento, utilizado para controlar sua
 sequencia, para abertura de CCDs.
 \param tipo Permite que no subtotal haja:
	- ACRESCIMO_PERCENTUAL
	- DESCONTO_PERCENTUAL
	- ACRESCIMO_VALOR
	- DESCONTO_VALOR
	- ACR_DSC_PERCENTUAL
	- ACR_DSC_VALOR
 \param valor Valor correspondente ao tipo.
 */
void ecf_bematech_cv0909::inicia_fechamento_nao_fiscal(unsigned char tipo,
    float valor)
{
	debug_log(std::string("ecf_bematech_cv0909::inicia_fechamento_nao_fiscal(") +
	    dquote(into_string(tipo).c_str()) + std::string(", ") +
	    into_string(valor) + std::string(")"));
	
	ecf_bematech::inicia_fechamento_nao_fiscal(tipo, valor);

	//debug_log("m_use_ccd.clear()");
	m_use_ccd.clear();
	set_pgto_seq(IDX_NOT_FOUND);
}


/**
 \brief Efetua conversao para texto, do download da MFD
 \param tipo 0 - total, 1 - data, 2 - COO
 \param ini Parâmetro inicial, conforme o tipo.
 \param fim Parâmetro final, conforme o tipo.
 \param origem Arquivo binário, contendo o download da MFD.
 \param destino Arquivo de texto a ser gerado após a conversão.
 \param fmt Formato do texto gerado, pode ser espelho da MFD ou ATO COTEPE.
 \param usr Obrigatório quando tipo for 1 (COO).
 */
void ecf_bematech_cv0909::converte_mfd(int tipo, const std::string& ini, 
    const std::string& fim, const std::string& origem, 
    const std::string& destino, int fmt, int usr)
{
	bool	mfd3_support = false;

#ifdef BEMAMFD3_SUPPORT
	mfd3_support = true;
#endif

	debug_log(std::string("ecf_bematech_cv0909::converte_mfd(") + 
	    into_string(tipo) + std::string(", ") + dquote(ini.c_str()) + 
	    std::string(", ") + dquote(fim.c_str()) + std::string(", ") + 
	    dquote(origem.c_str()) + std::string(", ") + dquote(destino.c_str()) +
	    std::string(", ") + into_string(fmt) + std::string(", ") +
	    into_string(usr) + std::string(")"));
	
	if (!mfd3_support) {
		throw ecf_exp(BEMATECH_MFD_NOT_SUPPORTED);
	}
	
	try {
		ecf_bematech::converte_mfd(tipo, ini, fim, origem, destino, fmt, usr);
	}
	catch (...) {
		throw;
	}
}
