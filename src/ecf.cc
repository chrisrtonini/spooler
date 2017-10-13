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

#include "ecf.h"


/**
 \brief Construtor padrão
 */
ecf_status::ecf_status(void)
{
	pouco_papel = false;
	sem_papel = false;
	cupom_fiscal_aberto = false;
	z_pendente = false;
	memoria_cheia = false;
	nao_fiscal_aberto = false;
}


/**
 \brief Constrói e inicializa o conteúdo do objeto.
 \param obj Instância de objeto ecf_status.
 */
ecf_status::ecf_status(const ecf_status& obj)
{
	pouco_papel = obj.pouco_papel;
	sem_papel = obj.sem_papel;
	cupom_fiscal_aberto = obj.cupom_fiscal_aberto;
	z_pendente = obj.z_pendente;
	memoria_cheia = obj.memoria_cheia;
	nao_fiscal_aberto = obj.nao_fiscal_aberto;
}


/**
 \brief Atribui valores ao conteúdo da instância de objeto.
 \param obj Instância de objeto ecf_status.
 */
ecf_status& ecf_status::operator=(const ecf_status& obj)
{
	pouco_papel = obj.pouco_papel;
	sem_papel = obj.sem_papel;
	cupom_fiscal_aberto = obj.cupom_fiscal_aberto;
	z_pendente = obj.z_pendente;
	memoria_cheia = obj.memoria_cheia;
	nao_fiscal_aberto = obj.nao_fiscal_aberto;

	return *this;
}


const char  ecf::ACRESCIMO_PERCENTUAL   = 'A';
const char  ecf::DESCONTO_PERCENTUAL	= 'D';
const char  ecf::ACRESCIMO_VALOR		= 'a';
const char  ecf::DESCONTO_VALOR			= 'd';
const char  ecf::ACR_DSC_PERCENTUAL		= 'X';
const char  ecf::ACR_DSC_VALOR			= 'x';
const char  ecf::NO_OP					= '\0';


/**
 \brief Construtor
 \details
 \param cfg_file Parâmetro opcional (NULL por padrão), informando o path de
 arquivo contendo configurações específicas para o driver.
 */
ecf::ecf(const std::string& cfg_file) : m_cfg_file(cfg_file)
{
	xdebug_log(std::string("ecf(") + dquote(cfg_file.c_str()) + std::string(")"));
}


/**
 \brief Produz nomes para arquivos de registro, contendo data e hora.
 \param dir Diretório base para o arquivo.
 \param prefix Identificação para o conteúdo do arquivo.
 \param seq Valor sequencial, para o caso de ser preciso mais do que um na 
 mesma data. Caso seja '0' (ZERO), não será adicionado ao nome.
 \param ext Extensão (sufixo) para o arquivo).
 \returns Path absoluto para o arquivo de registro.
 */
 std::string ecf::today_rec_path(std::string dir, std::string prefix,
									int seq, std::string ext)
{
	time_t now = time(0);
	struct tm data = *localtime(&now);
	char buff[80];
	std::string fpath = dir;

	fpath += prefix;
	fpath += "_";
	
	strftime(buff, sizeof(buff), "%Y-%m-%d.%X", &data);
	fpath += buff;

	if (seq > 0)	{
		fpath += "_";
		fpath += into_string(seq);
	}
	
	fpath += ".";
	fpath += ext;

	return fpath;
}
