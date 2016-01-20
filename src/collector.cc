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

#include "collector.h"


/**
 \brief Construtor.
 \param ctrl_file Caminho completo do arquivo de semáforo.
 */
collector::collector(const lp_struct* ctx)
{
	m_ctx = const_cast<lp_struct*>(ctx);
	m_running = true;
}


/*
 \brief Executa o laço de coleta.
 \details Aceita novos pacotes de requisição e dispara o callback.
 */
int collector::run(bool bckground)
{
	pid_t   my_pid = 0;
	int		code = COLLECTOR_RUNNING;

	// Sinalização, em memória compartilhada, do flag de
	// continuação da execução do laço de coleta.
	debug_log("collector::run() Definindo estado inicial do laco");
	loopctrl::to_true(*m_ctx);

	// Caso solicitada execução em background, deve ser
	// criado o processo filho.
	if (bckground)  {
		debug_log("collector::run() Rodar em background");
		my_pid = fork();
		if (my_pid == -1)   {
			std::string msg;
			switch (my_pid)
			{
				case EAGAIN		:
					msg = "Recursos insuficientes para criar processo filho.";
					break;
				case ENOMEM		:
					msg = "Espaco de armazenamento disponivel insuficiente.";
					break;
				default			:
					msg = "Erro desconhecido (fork()).";
			}
			error_log(std::string("collector::run() ") + msg);
			throw collector_exp(msg);
		}
	}

	// Executar o laço de coleta...
	if (my_pid == 0)	{
		debug_log("collector::run() Execucao do processo filho");
		while (m_running)   {
			debug_log("collector::run() Continuar execucao");
			try {
				debug_log("collector::run() Invocar 'accept()'");
				code = accept();
				debug_log("collector::run() Avaliar retorno 'accept()'");
				if (code != COLLECTOR_RUNNING)  {
					debug_log("collector::run() Sinalizado fim do laco");
					loopctrl::to_false(*m_ctx);
					if (code == COLLECTOR_ABORT)	{
						error_log(
						    std::string("collector::run() Sinalizada ") +
						    std::string("interrupcao do laco ") +
						    dquote(MSG_COLLECTOR_ABORT));
						throw collector_exp(MSG_COLLECTOR_ABORT);
					}
				}
			}
			catch (std::exception& err) {
				error_log(std::string("collector::run(): ") + err.what());
				throw;
			}
		}
	}

	// Retornar o código da última chamada de "accept"
	debug_log(std::string("collector::run(): ") + into_string(code));
	return code;
}


/**
 \brief Obtém status de continuidade do loop de execução
 \returns Valor lógico.
 */
bool collector::keep_running(void)
{
	return (m_running = loopctrl::running(*m_ctx));
}
