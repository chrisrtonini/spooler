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

#include "aliquota.h"


/**
 \brief Construtor padrão.
 */
aliq_trib::aliquota::aliquota(void)
{
	m_tipo = aliq_trib::TRIBUTADO_ICMS;
	m_cst = 0;
	m_aliq = 0.0;
}


/**
 \brief Construtor.
 \param str String contendo aliquota 'serializada' no format "T:00:1700". Onde:
	- T		= Caracter indicando a distinção do totalizador fiscal no ECF.
	- 00	= Código de Situação Tributária (com apenas 2 dígitos)
	- 1700  = Percentual (17,00%) de imposto.
 */
aliq_trib::aliquota::aliquota(const std::string& str)
{
	try {
		set(str);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor.
 \param tipo Caracter de identificação de tributo no ECF.
 \param cst Código de Situação Tributária (valor numérico com 2 dígitos).
 \param pcent Percentual de imposto.
 */
aliq_trib::aliquota::aliquota(unsigned char tipo, unsigned short cst, 
    float pcent)
{
	try {
		set(tipo, cst, pcent);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Construtor de cópia
 \param obj Instância de objeto aliquota.
 */
aliq_trib::aliquota::aliquota(const aliquota& obj)
{
	m_tipo = obj.get_tipo();
	m_cst = obj.get_cst();
	m_aliq = obj.get_percentual();
}


/**
 \brief Atribui valor à instância de objeto.
 \param obj Outra instância de objeto do mesmo tipo.
 \returns Referência para o objeto devidamente atribuído.
 */
aliq_trib::aliquota& aliq_trib::aliquota::operator=(const aliquota& obj)
{
	m_tipo = obj.get_tipo();
	m_cst = obj.get_cst();
	m_aliq = obj.get_percentual();

	return *this;
}


/**
 \brief Obtém o tipo de tributo.
 \returns Caractere 'T' para ICMS ou 'S' para ISS.
 */
unsigned char aliq_trib::aliquota::get_tipo(void) const
{
	return m_tipo;
}


/**
 \brief Obme o CST.
 \returns Valor numérico correspondente ao Código de Situação Tributária (CST).
 */
unsigned short aliq_trib::aliquota::get_cst(void) const
{
	return m_cst;
}


/**
 \brief Obtém o percentual de incidência.
 \returns Número de ponto flutuante, com valor na faixa entre 0.00 e 99.99.
 */
float aliq_trib::aliquota::get_percentual(void) const
{
	return m_aliq;
}


/**
 \brief Obtém alíquota em formato string.
 \returns String contendo alíquota no formato "99.99" (incluindo o PONTO
     como separador de decimal).
 */
std::string aliq_trib::aliquota::get_raw(void) const
{
	char	buff[12];
	char	final[12];
	size_t  len,
			i;
	size_t  pos = 0;

	if (strfmon(buff, 12, FMT_ALIQ_INTL, m_aliq) == -1) {
		throw aliquota_exp(MSG_ALIQ_FMT_ERR);
	}

	len = strlen(buff);
	for (i = 0; i < len; i++) {
		if (!isspace(buff[i]))	{
			final[pos++] = buff[i];
		}
	}
	final[pos] = '\0';

	return final;
}


/**
 \brief Obtém alíquota em formato string, sem separadores.
 \returns String contendo alíquota no formato "9999" (sem ESPAÇOS em branco e
     sem SEPARADOR decimal).
 */
std::string aliq_trib::aliquota::get_raw_numbers(void) const
{
	try {
		std::string obj = get_raw();

		size_t pos = obj.find(".");
		if (pos != std::string::npos)   {
			std::string final = obj.substr(0, pos);
			final += obj.substr(pos+1);

			return final;
		}
		
		return obj;
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Obtém a alíquota serializada em formato string
 \returns String no formato "T:00:9999"
 */
std::string aliq_trib::aliquota::get_serialized(void)
{
	char		buffer[32];

	memset(buffer, 0, sizeof(buffer));

	sprintf(buffer, "%c:%02d:%s", m_tipo, m_cst, get_raw_numbers().c_str());

	return buffer;
}


/**
 \brief Atribui propriedades do objeto.
 \param tipo Identificação de tipo de tributo: ISS/ICMS.
 \param cst Código de Situação Tributária.
 \param pcent Alíquota tributária (percentual)
 */
void aliq_trib::aliquota::set(unsigned char tipo, unsigned short cst, 
    float pcent) throw(aliquota_exp)
{
	if ((tipo != TRIBUTADO_ISS) && (tipo != TRIBUTADO_ICMS) &&
		    (tipo != SUBST_TRIBUTARIA) && (tipo != NAO_INCIDENTE) &&
			(tipo != ISENTO))   {
		throw aliquota_exp(MSG_UNKNOWN_TYPE);
	}

	m_tipo = tipo;
	m_cst = cst;
	
	if ((pcent < 0.0) || (pcent > 99.99))   {
		throw aliquota_exp(MSG_PCENT_OUT_RANGE);
	};

	m_aliq = pcent;
}


/**
 \brief Atribui propriedades a partir de string serializada.
 \param str Cadeia de caracteres no formato T:C:99.99. Onde:
	- T: ICMS ou ISS
	- C: CST
	- 99.99: percentual da alíquota tributária.
 */
void aliq_trib::aliquota::set(const std::string& str)
{
	size_t  sep1;
	size_t  sep2;

	unsigned char   tipo;
	unsigned short  cst;
	float			pcent;

	sep1 = str.find(":");
	sep2 = str.rfind(":");

	if (((sep1 == std::string::npos) || (sep2 == std::string::npos)) ||
	    (sep1 == sep2))  {
		throw aliquota_exp(MSG_MALFORMED_ALIQ);
	}

	try {
		tipo = str.at(sep1 - 1);
		cst = atoi(str.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
		std::string tmp = str.substr(sep2 + 1, 2);
		tmp += ".";
		tmp += str.substr(sep2 + 3);
		pcent = atof(tmp.c_str());

		set(tipo, cst, pcent);
	}
	catch (...) {
		throw;
	}
}


/**
 \brief Desmembra e ecoa conteúdo do objeto.
 */
void aliq_trib::aliquota::dump(void)
{
	char	tp[2];

	memset(tp, 0, sizeof(tp));
	tp[0] = m_tipo;
	
	log::logger::info_msg("Inicio - aliq_trib::aliquota::dump");
	log::logger::info_msg(std::string("Tributo: [") + std::string(tp) +
	    std::string("]"));
	log::logger::info_msg(std::string("C.S.T. : [") + into_string(m_cst) +
	    std::string("]"));
	log::logger::info_msg(std::string("Aliq.  : [") + into_string(m_aliq) +
	    std::string("]"));
	log::logger::info_msg("Fim - aliq_trib::aliquota::dump");
}
