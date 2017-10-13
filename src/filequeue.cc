/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * spooler
 * Copyright (C) Chris Robert Tonini 2016 <chrisrtonini@gmail.com>
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

#include "filequeue.h"


/**
 \brief Método construtor.
 \details Inicializa elementos para controle de monitoramento de diretório,
 pois estes dependem de estados e estão distribuídos em diversos métodos.
 \param path Diretório a ser monitorado.
 \param prefix Critério de selecão de arquivos. Considerado seu 'prefixo'.
 \param timeout Tempo máximo de espera para o monitoramento de eventos.
 */
file_queue::file_queue(const std::string& path, const std::string& prefix,
                       const long timeout)
{
	//debug_log("file_queue::file_queue(...): Instanciacao.");
	m_scan_type = dir_scan;
	m_path = path;
	m_prefix = prefix;
	m_timeout = timeout;
	m_fd = 0;
	m_wd = 0;
	m_mask = IN_MOVED_TO | IN_CLOSE_WRITE;
}


/**
 \brief Método destruidor.
 \details Garante que não fique aberto o descritor de monitoramento de eventos,
 caso haja um em uso.
 */
file_queue::~file_queue(void)
{
	//debug_log("file_queue::~file_queue(): Destruicao.");
	stop_monitoring();
}


/**
 \brief Define nova máscara para monitoramento de eventos
 \param new_mask Máscara, cujos valores pode ser combinados por 'OR'.
 */
void file_queue::set_mask(uint32_t new_mask)
{
	char	fmt_msk[16];
	sprintf(fmt_msk, "%08X", new_mask);
	
	xdebug_log(std::string("file_queue::set_mask(0x") + into_string(fmt_msk) +
	           std::string(")"));
	
	m_mask = new_mask;
}


/**
 \brief Obtém a máscara de eventos em uso.
 \returns Valor OR'ed contendo a máscara de eventos monitorados.
 */
uint32_t file_queue::get_mask(void)
{
	char	fmt_msk[16];
	sprintf(fmt_msk, "%08X", m_mask);

	xdebug_log(std::string("file_queue::get_mask(0x") + into_string(fmt_msk) +
	           std::string(")"));
	
	return m_mask;
}


/**
 \brief Define o timeout de eventos.
 \param timeout Inteiro longo representando o tempo em milissegundos.
 */
void file_queue::set_timeout(const long timeout)
{
	xdebug_log(std::string("file_queue::set_timeout(") + 
	           into_string(timeout) + std::string(")"));
	
	m_timeout = timeout;
}


/**
 \brief Obtém o timeout de eventos.
 \returns Tempo em milissegundos (inteiro longo).
 */
long file_queue::get_timeout(void)
{
	xdebug_log(std::string("file_queue::get_timeout(") +
	           into_string(m_timeout) + std::string(")"));
	
	return m_timeout;
}


/**
 \brief Realiza a varredura de diretório.
 \details Esquadrinha diretório buscando por arquivos cujos nomes coincidam com
 o prefixo informado, e os adiciona à fila.
 \returns Valor lógico indicando se pelo menos um arquivo foi adicionado à lista.
 */
bool file_queue::directory_scan(void)
{
	//debug_log("file_queue::directory_scan()");

	DIR*			dir;
	struct dirent*  item;
	bool			got_one = false;
	std::string	 tmp_name;

	//debug_log("file_queue::directory_scan(): scanning");

	if ((dir = opendir(m_path.c_str())) == NULL)
		throw fs_exp(fs::err_opendir(errno));

	while ((item = readdir(dir)) != NULL)	{
		tmp_name = item->d_name;
		if (tmp_name.find(m_prefix, 0) == 0)	{
			got_one = true;
			xdebug_log(std::string("file_queue::directory_scan(): \"") +
			           tmp_name + std::string("\""));
			push(tmp_name);
		}
	}

	if (closedir(dir) == -1)
		throw fs_exp(fs::err_closedir(errno));

//	if (got_one)
//		debug_log("file_queue::directory_scan(): Encontrado!");
	
	return got_one;
}


/**
 \brief Monitora a ocorrência de um evento no diretório especificado.
 \details Verifica eventos relacionados a arquivos, e adiciona à fila o nome do
 arquivo associado ao evento.
 \returns Valor lógico indicando se pelo menos um arquivo foi adicionado à lista.
 */
bool file_queue::kernel_scan(void)
{
//	debug_log("file_queue::kernel_scan()");
	
	fd_set						fds;
	char						eventq[NOTIFY_EVENT_QUEUE_LEN];
	char*						ptr;
	const struct inotify_event* ev;
	struct timeval				tmout;
	int							bytes = 0;
	int							test = 0;
	bool						got_one = false;
	std::string					tmp_name;

	memset(eventq, 0, NOTIFY_EVENT_QUEUE_LEN);
	FD_ZERO(&fds);

	try {
		start_monitoring();

//		debug_log("file_queue::kernel_scan() polling");
		FD_SET(m_fd, &fds);
		tmout.tv_sec = 0;
		tmout.tv_usec = m_timeout;

		try {
			// Como forma de evitar a quebra do mecanismo de encerramento da
			// aplicação, 'select' recebe um tratamento próprio de exceção,
			// de maneira a não propagá-la.
			test = select(FD_SETSIZE, &fds, NULL, NULL,
				          (m_timeout > 0 ? &tmout : NULL));
			if (test < 0)   {
				info_log("file_queue::kernel_scan(): select() = Interromper!");
				throw ctrl_exp(fs::err_select(errno));
			}
		}
		catch (ctrl_exp& err) {
			warn_log(std::string("file_queue::kernel_scan() ") + err.what());
			// Impede qualquer processamento de evento
			test = 0;
			throw;
		}

		// Sem nenhum uso, em virtude do mapeamento acima, para controle
		// do laço de execução do serviço.
		if (test < 0)
			throw fs_exp(fs::err_select(errno));

		if (test > 0)   {
			//debug_log("file_queue::kernel_scan() consultar eventos");
			bytes = read(m_fd, eventq, NOTIFY_EVENT_QUEUE_LEN);
			if (bytes < 0)
				throw fs_exp(fs::err_read(errno));

			if (bytes > 0)  {
				//debug_log("file_queue::kernel_scan() processar eventos");
				for (ptr = eventq; ptr < eventq + bytes; 
				 	   ptr += sizeof(struct inotify_event) + ev->len)  {
					ev = const_cast<const struct inotify_event*>(reinterpret_cast<struct inotify_event*>(ptr));
					if (ev->len)	{
						/*
						if (ev->mask & IN_CLOSE_WRITE)
							xdebug_log(std::string("file_queue::kernel_scan()") +
							           std::string(" IN_CLOSE_WRITE [") +
							           into_string(ev->name) +
							           std::string("]"));
						if (ev->mask & IN_MOVED_TO)
							xdebug_log(std::string("file_queue::kernel_scan()") +
							           std::string(" IN_MOVED_TO [") +
							           into_string(ev->name) +
							           std::string("]"));
						*/
						tmp_name= ev->name;
						if (tmp_name.substr(0, m_prefix.length()) == m_prefix)  {
							got_one = true;
							xdebug_log(std::string("file_queue::kernel_scan() ") +
							           std::string("inserindo \"") + tmp_name +
							           std::string("\" na fila"));
							push(tmp_name);
						}
					}
				}
			}
		}
		// Precisa ser removido para a lógica funcionar.
		// Ficou comentado para verificação de release beta teste.
		//stop_monitoring();
	}
	catch (...) {
		throw;
	}

	return got_one;
}


/**
 \brief Fornece o nome de um arquivo, quando disponível.
 \details Quando disponível na fila, remove o nome do topo, e o retorna. Quando
 a fila estiver vazia, efetua a coleta diretamente no diretório, respeitando o
 método de leitura.
 \note Ao instanciar o objeto, a forma de leitura é sempre por varredura, pois
 assim, se já ouver arquivos no diretório, eles serão coletados. A partir do
 momento que estes (caso existam) sejam processados, e não restar nenhum 
 arquivo, a forma de leitura passa a ser vinculado aos eventos de gravação (por
 padrão, mas pode ser modificado na instância).
 \returns Nome de arquivo, dentro da pasta especificada.
 */
std::string file_queue::scan_files(void)
{
	//debug_log("file_queue::scan_files()");
	std::string next_file = "";
	
	while (empty()) {
		if (m_scan_type == dir_scan)	{
			if (!directory_scan())  {
				//debug_log(std::string("file_queue::scan_files() alterado ") +
				//          std::string("'scan_type' de 'dir_scan' para ") +
				//          std::string("'event_scan'"));
				m_scan_type = event_scan;
			}
		}
		else	{
			try {
				kernel_scan();
			}
			catch (ctrl_exp& err)   {
				info_log("file_queue::scan_files() solicitada interrupcao!");
				break;
			}
		}
	}

	if (!empty())   {
		next_file = front();
		pop();
		xdebug_log(std::string("file_queue::scan_files() \"") + next_file +
			       std::string("\" retirado da fila"));
	}

	return next_file;
}


/**
 \brief Prepara descritores para monitoramento de eventos em diretório.
 */
void file_queue::start_monitoring(void)
{
//	debug_log("file_queue::start_monitoring()");
	
	if (m_fd <= 0)  {
		//debug_log("file_queue::start_monitoring() instanciar descritor");
		if ((m_fd = inotify_init()) < 0)
			throw fs_exp(fs::err_inotify_init(errno));
	}

	if (m_wd <= 0)  {
		//debug_log("file_queue::start_monitoring() instanciar monitor de evento");
		if ((m_wd = inotify_add_watch(m_fd,
						const_cast<char*>(m_path.c_str()), m_mask)) < 0)
			throw fs_exp(fs::err_inotify_add_watch(errno));
	}
}


/**
 \brief Finaliza mecanismo de monitoramente de eventos.
 */
void file_queue::stop_monitoring(void)
{
	//debug_log("file_queue::stop_monitoring()");

	if (m_fd)   {
		try {
			reset_monitoring();
		}
		catch (...) {
			throw;
		}
		close(m_fd);
		m_fd = 0;
	}
}


/**
 \brief Reinicializa monitor de eventos, para uma nova interação com o S.O.
 */
void file_queue::reset_monitoring(void)
{
	//debug_log("file_queue::reset_monitoring()");
	
	if (m_wd)   {
		if (inotify_rm_watch(m_fd, m_wd) < 0)
			throw fs_exp(fs::err_inotify_rm_watch(errno));
		m_wd = 0;
	}
}
