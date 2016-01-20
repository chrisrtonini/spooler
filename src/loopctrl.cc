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

#include "loopctrl.h"


/**
 \brief Obtém mensagens de erro da chamada shmget.
 \param code Inteiro indicando o código de erro (errno).
 \returns String contendo a mensagem descritiva de erro.
 */
std::string loopctrl::err_shmget(int code) NO_THROW
{
	std::string msg;
	
	switch (code)
	{
		case EACCES		:
			msg = "Acesso negado. Chave ja associada a um bloco de memoria.";
			break;
		case EEXIST		:
			msg = "Chave ja associada a um bloco de memoria exclusivo.";
			break;
		case EINVAL		:
			msg = "Tamanho do bloco de memoria invalido ou fora da faixa.";
			break;
		case ENOENT		:
			msg = "Chave nao associada a um bloco de memoria.";
			break;
		case ENOMEM		:
			msg = "Memoria insuficiente.";
			break;
		case ENOSPC		:
			msg = "Muitos blocos de memoria compartilhada ja definidos.";
			break;
		default			:
			msg = "Erro desconhecido (shmget(...)).";
	}

	return msg;
}


/**
 \brief Obtém mensagens de erro da chamada shmat.
 \param code Inteiro indicando o código de erro (errno).
 \returns String contendo a mensagem descritiva de erro.
 */
std::string loopctrl::err_shmat(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EACCES		:
			msg = "Acesso ao bloco de memoria negado.";
			break;
		case EINVAL		:
			msg = "Argumentos invalidos para acesso ao bloco de memoria.";
			break;
		case EMFILE		:
			msg = "Processo excedeu qtde. de blocos compartilhados acessados.";
			break;
		case ENOMEM		:
			msg = "Memoria insuficiente.";
			break;
		default			:
			msg = "Erro desconhecido (shmat(...)).";
	}

	return msg;
}


/**
 \brief Obtém mensagens de erro da chamada shmat.
 \param code Inteiro indicando o código de erro (errno).
 \returns String contendo a mensagem descritiva de erro.
 */
std::string loopctrl::err_shmdt(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EINVAL		:
			msg = "Memoria enderecada incorretamente.";
			break;
		default			:
			msg = "Erro desconhecido (shmdt(...)).";
	}

	return msg;
}


/**
 \brief Obtém mensagens de erro da chamada shmctl.
 \param code Inteiro indicando o código de erro (errno).
 \returns String contendo a mensagem descritiva de erro.
 */
std::string loopctrl::err_shmctl(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EACCES		:
			msg = "Acesso negado ao bloco de memoria.";
			break;
		case EINVAL		:
			msg = "Chave do bloco de memoria ou comando invalido.";
			break;
		case EPERM		:
			msg = "Operacao nao permitida para este usuario.";
			break;
		case EOVERFLOW  :
			msg = "Dados de controle excedem buffer.";
			break;
		default			:
			msg = "Erro desconhecido (shmctl(...)).";
	}

	return msg;
}


/**
 \static init
 \brief Define bloco compartilhado de memória, com base na estrutura de dados
 para controle de laços.
 \param ctx Referência para estrutura de dados de controle.
 \throws loopctrl_exp Caso ocorra algum problema ao definir a memória 
 compartilhada.
 */
void loopctrl::init(lp_struct* ctx) throw(loopctrl_exp)
{
	ctx->handler = shmget(ctx->proc_id, m_size, IPC_CREAT|m_perm);
	if (ctx->handler == -1) {
		throw loopctrl_exp(err_shmget(errno));
	}
}


/**
 \static to_true
 \brief Atribui TRUE à flag de controle de execução.
 \param ctx Objeto contendo identificador de memória compartilhada.
 \throws loopctrl_exp Caso ocorra problema ao acessar memória compartilhada.
 */
void loopctrl::to_true(lp_struct ctx) throw(loopctrl_exp)
{
	lp_struct*  ptr = static_cast<lp_struct*>(shmat(ctx.handler, NULL, 0));
	if (*(reinterpret_cast<int*>(ptr)) == -1) {
		throw loopctrl_exp(err_shmat(errno));
	}
	ptr->keep_running = true;
}


/**
 \static to_false
 \brief Atribui FALSE à flag de controle de execução.
 \param ctx Objeto contendo identificador de memória compartilhada.
 \throws loopctrl_exp Caso ocorra problema ao acessar memória compartilhada.
 */
void loopctrl::to_false(lp_struct ctx) throw(loopctrl_exp)
{
	lp_struct*  ptr = static_cast<lp_struct*>(shmat(ctx.handler, NULL, 0));
	if (*(reinterpret_cast<int*>(ptr)) == -1) {
		throw loopctrl_exp(err_shmat(errno));
	}
	ptr->keep_running = false;
}


/**
 \static running
 \brief Obtém o valor da flag de controle de execução.
 \param ctx Objeto contendo identificador de memória compartilhada.
 \throws loopctrl_exp Caso ocorra problema ao acessar memória compartilhada.
 */
bool loopctrl::running(lp_struct ctx) throw(loopctrl_exp)
{
	lp_struct*  ptr = static_cast<lp_struct*>(shmat(ctx.handler, NULL, 0));
	if (*(reinterpret_cast<int*>(ptr)) == -1) {
		throw loopctrl_exp(err_shmat(errno));
	}
	return ptr->keep_running;
}


/**
 \static destroy
 \brief Remove bloco de memória compartilhada.
 \param ctx Objeto contendo identificador de memória compartilhada.
 \throws loopctrl_exp Caso ocorra problema ao acessar memória compartilhada.
 */
void loopctrl::destroy(lp_struct ctx) throw(loopctrl_exp)
{
	if (shmctl(ctx.handler, IPC_RMID, NULL) == -1)  {
		throw loopctrl_exp(err_shmctl(errno));
	}
}
