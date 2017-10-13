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

#ifndef _EXCP_H_
#define _EXCP_H_


#ifndef NO_THROW
	#if __cplusplus > 199711L
		#define NO_THROW	noexcept
	#else
		#define NO_THROW	throw()
	#endif
#endif


#include <exception>
#include <string>

/**
 \class spooler_exp
 \brief Classe base para definição de todos os objetos de exceção do projeto
 */
class spooler_exp : public std::exception
{
	public:
					spooler_exp(const std::string& msg) : m_msg(msg)		{}
					~spooler_exp(void) NO_THROW								{}
		
		const char* what(void) const NO_THROW								{
			return m_msg.c_str();
		}

	private:
		std::string m_msg;
};


/**
 \class cmdtag_exp
 \brief Exceção lançada quando da validação (estrutural) sintática de tags
 para pacotes de comando (ou resposta).
 */
class cmdtag_exp : public spooler_exp
{
	public:
					cmdtag_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class collector_exp
 \brief Exceção lançada por falha reportada pelo collector.
 */
class collector_exp : public spooler_exp
{
	public:
					collector_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class datapair_exp
 \brief Exceção lançada quando não houver uma referida chave + valor.
 */
class datapair_exp : public spooler_exp
{
	public:
					datapair_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class ecf_exp
 \brief Exceções lançadas por objetos encapsulando ECFs.
 */
class ecf_exp : public spooler_exp
{
	public:
					ecf_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class fs_exp
 \brief Exceção lançada por falha reportada pelo kernel/filesystem.
 */
class fs_exp : public spooler_exp
{
	public:
					fs_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class loopctrl_exp
 \brief Exceção lançada quando não for possível criar o block de memória
 compartilhada.
 */
class loopctrl_exp : public spooler_exp
{
	public:
					loopctrl_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class packet_exp
 \brief Exceção lançada quando da validação (estrutural) de pacotes de comando
 (ou resposta).
 */
class packet_exp : public spooler_exp
{
	public:
					packet_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class pkt_param_exp
 \brief Exceção lançada quando da validação (hierárquica) de parâmetros de
 comando (ou resposta).
 */
class pkt_param_exp : public spooler_exp
{
	public:
					pkt_param_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class rqt_reader_exp
 \brief Exceção lançada quando da validação estrutural de pacotes de comando.
 */
class rqt_reader_exp : public spooler_exp
{
	public:
					rqt_reader_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class cortar_papel_exp
 \brief Execção lançada para comando de corte de papel mal formatado.
 \details Apenas para fins de compatibilidade, pois o Solidus PDV, produz, em
 determinadas circunstancias, pacote de comando sem cabeçalho ou rodapé. 
 Evidenciado apenas em casos de sinalização para acionamento de guilhotna, dentro
 de texto não fiscal (relatório gerencial ou comprovante vinculado).
 */
class cortar_papel_exp : public rqt_reader_exp
{
	public:
					cortar_papel_exp(const std::string& msg) : rqt_reader_exp(msg) {}
};


/**
 \class aliquota_exp
 \brief Exceção lançado quando do tratamento de alíquotas tributárias.
 */
class aliquota_exp : public spooler_exp
{
	public:
					aliquota_exp(const std::string& msg) : spooler_exp(msg) {}
};


/**
 \class ctrl_exp
 \brief Exceção lançado no controle de execução, sinalizando parada do serviço.
 */
class ctrl_exp : public spooler_exp
{
	public:
					ctrl_exp(const std::string& msg) : spooler_exp(msg) {}
};


#endif
