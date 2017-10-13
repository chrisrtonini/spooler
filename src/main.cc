/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) Chris Robert Tonini 2015 <chrisrtonini@gmail.com>
 * 
 * spooler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * spooler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <ios>
#include <fstream>
#include <exception>

#include <unistd.h>
#include <signal.h>

#include "txtcollector.h"
#include "txtwriter.h"

#include "factory.h"

#include "ecf.h"
#include "ecfbroker.h"
#include "log.h"
#include "utils.h"


// Variável global. Estrutura de parâmetros de configurações
// da aplicação.
datapair app_cfg;

#define SPOOLER_CFG_DEFAULT "/etc/spooler/spooler.cfg"


/**
 \static Estrutura de dados pública.
 \brief Utilizada para controle de contexto de execução, é compartilhada entre
 o manipulador de signal e o laço de execução.
 */
static lp_struct	context;


/**
 \func signal_handler
 \brief Manipulador de sinais, garantindo o fechamento e destruição de
 estruturas de controle mais "sensíveis".
 \param sig_num Identificador do signal a ser tratado.
 */
void signal_handler(int sig_num)
{
	switch (sig_num)
	{
		case SIGSEGV	:
			error_log("signal_handler() Falha de segmentacao");
			//loopctrl::to_false(context);
			exit(SIGSEGV);
			break;
		case SIGQUIT	:
		case SIGINT		:
		case SIGTERM	:
			info_log("signal_handler() Solicitada interrupcao/termino");
			loopctrl::to_false(context);
			break;
	}
}


/**
 \func signal_register
 \brief Associa manipulador de signal aos eventos de INTerrupt, TERMitation e
 SEGmentation fault.
 */
void signal_register(void)
{
	struct sigaction	sigact;

	sigact.sa_handler = signal_handler;
	sigact.sa_flags = 0;
	sigemptyset(&sigact.sa_mask);
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGSEGV, &sigact, NULL);

	debug_log(
	    "signal_register() Redefinidos handlers: SIGINT, SIGTERM e SIGSEGV");
}


/**
 \func serializer
 \brief Gera pacote de saída, em arquivo de texto.
 \param path Caminho + nome do arquivo de resposta.
 \param pkt Estrutura de dados contendo pacote de resposta.
 */
void serializer(const std::string& path, const response_packet* pkt)
{
	std::string rsp = path;
	std::string tmp = path;
	std::string lck;
	size_t		pos = rsp.find("arqcmd");

	if (pos != std::string::npos)   {
		rsp.replace(pos, 6, "arqret");
		tmp.replace(pos, 6, "_rqret");
	}
	else	{
		error_log("serializer() Erro ao definir nome do arquivo de retorno");
		throw spooler_exp("Erro ao definir nome do arquivo de retorno.");
	}

	try {
		std::ofstream   out(tmp.c_str());
		txt_writer		obj(&out);
		
		obj.write_packet(*pkt);
		out.close();
		
		fs::copy_file(tmp, rsp);

		fs::sys_unlink(tmp);
		
		lck = rsp;
		lck += ".lck";
		std::ofstream   out_lck(lck.c_str());
		out_lck << "";
		out_lck.close();
	}
	catch (...) {
		error_log("serializer() Erro ao gravar arquivo de retorno");
		throw spooler_exp("Erro ao gravar arquivo de retorno.");
	}
}


/**
 \func load_cfg
 \brief Carrega o arquivo de configurações e alimenta estrutura de dados.
 */
void load_cfg(datapair* cfg, const std::string& file)
{
	std::ifstream   hnd;
	std::string		msg;
	std::string		line;

	if (!fs::found(file))   {
		msg = "Arquivo de configuracoes \"";
		msg += file;
		msg += "\" nao encontrado!";
		throw spooler_exp(msg);
	}

	try {
		hnd.open(file.c_str(), std::ifstream::in);
		hnd.exceptions(std::ios::failbit | std::ios::badbit);
		do  {
			hnd >> line;
			if (!hnd.eof())
				cfg->from_raw_data(line);
		} while (!hnd.eof());
		hnd.close();
	}
	catch (...) {
		bool fim = hnd.eof();

		if (hnd.is_open())
			hnd.close();

		if (!fim)
			throw;
	}
}


/**
 \func close_up
 \brief Fecha stream de log, gravando mensagem sem formatacao, em caso de
 emergencia.
 \param out Canal para gravao de log.
 \param msg Mensagem a ser registrada.
 */
void close_up(std::ofstream& out, const std::string& msg)
{
	if (out.is_open())  {
		out << msg << std::endl;
		out.flush();
		out.close();
	}
}


/**
 \func main
 \brief Rotina principal.
 \details Instancia elementos necessários ao funcionamento da aplicação, e
 inicia laço de execução.
 */
int main(int argc, char* argv[])
{
	std::string		hw;
	std::string		cfg_file;
	std::ofstream   log_stream;
	int				code = COLLECTOR_RUNNING;
	txt_collector*  eng;
	ecf_broker*		broker;
	ecf*			ifiscal;
		
	// Output da "assinatura" da aplicação.
	std::cerr << APP_NAME << " - " << APP_DESCR << " (";
	std::cerr << APP_VERSION << "." << APP_RELEASE << APP_STATUS << ")";
	std::cerr << std::endl << std::endl;

	// Verificação de argumentos de linha de comando.
	if (argc < 2)   {
		std::cerr << "Nenhum arquivo de configuracoes informado." << std::endl;
		std::cerr << "Assumindo: /etc/spooler/spooler.cfg" << std::endl;
		cfg_file = SPOOLER_CFG_DEFAULT;
	}
	else
		cfg_file = argv[1];

	// Carga de configurações...
	try {
		load_cfg(&app_cfg, cfg_file);
	}
	catch (spooler_exp& err)	{
		std::cerr << "Impossiver carregar parametros ("  << cfg_file << ")";
		std::cerr << std::endl << std::endl;
		throw;
	}
	catch (...) {
		throw;
	}

	// Preparar log da aplicação...
	try {
		log_stream.open(app_cfg.get_value_by_key(APP_LOG_FILE).c_str(),
		    std::ios_base::app);
		log::logger::setup(&log_stream, 
		    static_cast<log::level>(
			    atoi(app_cfg.get_value_by_key(APP_LOG_LEVEL).c_str())),
		    static_cast<bool>(
			    atoi(app_cfg.get_value_by_key(APP_LOG_VERBOSE).c_str())));
		
		info_log(std::string(APP_NAME) + std::string(" - ") +
		         std::string(APP_DESCR) + std::string(" (") +
		         std::string(APP_VERSION) + std::string(".") +
		         std::string(APP_RELEASE) + std::string(APP_STATUS) +
		         std::string(")"));
		
		info_log("main() --- Inicio ---");
	}
	catch (std::ofstream::failure& err) {
		std::cerr << "Falha ao abrir arquivo de log: " << std::endl;
		throw;
	}
	catch (std::bad_alloc& err) {
		close_up(log_stream, err.what());
		std::cerr << "Erro ao alocar memoria para log stream: " << std::endl;
		throw;
	}
	catch (std::exception& err)  {
		close_up(log_stream, err.what());
		std::cerr << "Impossivel instanciar log stream." << std::endl;
		throw;
	}
	
	try {
		hw = app_cfg.get_value_by_key(APP_ECF_DRIVER);
		info_log("main() Instanciar driver de ECF");
		debug_log(std::string("main() Driver = ") + hw);
		ifiscal = factory::create_ecf(factory::vendor_id(hw), 
		                              app_cfg.get_value_by_key(APP_ECF_SPECIFIC));
	}
	catch (spooler_exp& err)	{
		close_up(log_stream, err.what());
		std::cerr << "Erro ao instanciar controlador de ECF." << std::endl;
		throw;
	}
	catch (std::exception& err) {
		close_up(log_stream, err.what());
		std::cerr << "Ocorreu uma exceção ao instanciar driver de ECF.";
		std::cerr << std::endl << std::endl;
		throw;
	}

	try {
		info_log("main() Instanciar camada de conversao");
		broker = new ecf_broker(ifiscal);
	}
	catch (spooler_exp& err)	{
		close_up(log_stream, err.what());
		std::cerr << "Erro ao instanciar conversor de chamadas." << std::endl;
		throw;
	}
	catch (std::exception& err) {
		close_up(log_stream, err.what());
		std::cerr << "Ocorreu uma exceção ao instanciar camada de conversao.";
		std::cerr << std::endl << std::endl;
		throw;
	}

	try {
		debug_log("main() Definir token para controle de loop");
		context.proc_id = getpid();
		debug_log("main() Inicializar controle de loop");
		loopctrl::init(&context);

		debug_log("main() Registrar manipuladores de signal");
		signal_register();
	}
	catch (loopctrl_exp& err)   {
		close_up(log_stream, err.what());
		std::cerr << "Erro ao inicializar controle de laco." << std::endl;
		throw;
	}
	catch (std::exception& err) {
		close_up(log_stream, err.what());
		std::cerr << "Ocorreu uma exceção ao inicializar controle de laco.";
		std::cerr << std::endl << std::endl;
		throw;
	}

	try {
		debug_log("main() Instanciar coletor de pacotes");
		eng = new txt_collector(&context);

		debug_log("main() Setup do coletor de pacotes");
		eng->setup(app_cfg, broker, serializer);
	}
	catch (std::exception& err) {
		close_up(log_stream, err.what());
		std::cerr << "Erro ao instanciar coletor de pacotes." << std::endl;
		throw;
	}

	try {
		info_log("main() Iniciar coleta");
		code = eng->run(
		    atoi(app_cfg.get_value_by_key(APP_RUN_BCKGROUND).c_str()));

		if (!context.keep_running)  {
			debug_log("main() Desalocar coletor de pacotes");
			delete eng;
		}
	}
	catch (std::exception& err) {
		close_up(log_stream, err.what());
		std::cerr << "Erro ao executar laco da aplicacao." << std::endl;
		throw;
	}


	info_log(std::string("main() Codigo de retorno: ") + into_string(code));
	return code;
}
