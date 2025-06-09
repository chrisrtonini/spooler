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

#include "fs.h"


/**
 \brief Atribui máscara para criação de arquivos e diretórios.
 \param mask Máscara (conforme man 3p umask).
 \note Deve-se levar em consideração que ao valor de "mask" aplicam-se as
 máscaras (& 0777) para diretórios e (& 0666) para arquivos. Desta forma,
 a invocação de fs::set_umask(022), implicará na criação de arquivos com as
 permissões 0644 (022 & 0666)
 */
void fs::set_umask(mode_t mask) NO_THROW
{
	umask(mask);
}


/**
 \brief Cria nodo no sistema de arquivos.
 \param path Caminho completo do nodo.
 \param mode Permissões do nodo criado.
 \param dev Tipo de nodo, conforme definições de sys/types.h
 \note Os valores para "dev" podem conter associações bitwise ("|").
 \returns Valor lógico, indicando sucesso ou falha na operação.
 \throws fs_exp Caso ocorra algum erro de syscall.
 */
bool fs::create_node(const std::string& path, mode_t mode, dev_t dev)
       throw(fs_exp)
{
       if (mknod(path.c_str(), mode, dev) == -1)   {
               throw fs_exp(fs::fs_err_msg(errno));
       }

       return true;
}


/**
 \brief Verifica a existência de um arquivo.
 \param path Caminho + nome de arquivo
 \returns Valor lógico indicando sucesso ou falha na operação.
 \throws fs_exp Caso ocorra algum erro de syscall.
 */
bool fs::found(const std::string& path) throw(fs_exp)
{
	bool ret = false;

	try {
		ret = fs::sys_access(path, F_OK);
	}
	catch (fs_exp& e)   {
		if (errno != ENOENT)	{
			throw;
		}
	}

	return ret;
}


/**
 \brief Testa disponibilidade de arquivo, dentro do modo especificado.
 \param path Caminho + nome de arquivo.
 \param mode Modo a ser testado.
 \returns Valor lógico indicando o resultado do teste.
 \throws fs_exp Caso ocorra algum erro de syscall.
	*/
bool fs::sys_access(const std::string& path, int mode) throw(fs_exp)
{
	int ret = 0;

	if ((ret = access(path.c_str(), mode)) < 0)   {
		throw fs_exp(fs::access_err_msg(errno));
	}

	return (ret == 0);
}


/**
 \brief Remove um arquivo do filesystem.
 \param path Caminho + nome de arquivo.
 \returns Valor lógico indicando se o arquivo foi ou não removido.
 \throws fs_exp Caso ocorra algum erro de syscall.
 */
bool fs::sys_unlink(const std::string& path) throw(fs_exp)
{
	int ret = 0;

	if ((ret = unlink(path.c_str())) < 0)   {
		throw fs_exp(fs::fs_err_msg(errno));
	}

	return (ret == 0);
}


/**
 \brief Cria um link simbólico para um arquivo (ou diretório)
 \param file_path Caminho (+ nome de arquivo) origem.
 \param link_path Caminho (+ nome de arquivo) destino.
 \returns Valor lógico indicando sucesso ou falha na criação do link.
 \throws fs_exp Caso ocorra algum erro de syscall.
 */
bool fs::sys_symlink(const std::string& file_path, const std::string& link_path)
	throw(fs_exp)
{
	int ret = -1;

	try {
		if (fs::found(file_path))   {
			if ((ret = symlink(file_path.c_str(), link_path.c_str())) < 0)  {
				throw fs_exp(fs::fs_err_msg(errno));
			}
		}
	}
	catch (fs_exp& e)   {
		throw;
	}

	return (ret == 0);
}


/**
 \brief Cria um diretório.
 \param path Caminho a ser criado.
 \param mode Permissões para o novo diretório.
 \returns Valor lógico indicando sucesso ou falha na criação do diretório.
 \throws fs_exp Caso ocorra alguma falha na invocação da syscall.
 */
bool fs::sys_mkdir(const std::string& path, mode_t mode) throw(fs_exp)
{
	int ret = -1;

	if (!path.empty())  {
		if ((ret = mkdir(path.c_str(), mode)) < 0)  {
			throw fs_exp(fs::fs_err_msg(errno));
		}
	}

	return (ret == 0);
}


/**
 \brief Remove um diretório.
 \param path Caminho para o nodo a ser removido.
 \returns Valor lógico indicando sucesso ou falha na remoção.
 \throws fs_exp Caso ocorra alguma falha na invocação da syscall.
 */
bool fs::sys_rmdir(const std::string& path) throw(fs_exp)
{
	int ret = -1;

	if ((ret = rmdir(path.c_str())) < 0) {
		throw fs_exp(fs::fs_err_msg(errno));
	}

	return (ret == 0);
}


/**
 \brief Cria um named pipe
 \param path Caminho + nome do pipe
 \param mode Permissões do pipe
 \returns Valor lógico indicando sucesso ou falha na criação do fifo.
 \throws fs_exp Caso ocorra algum problema na invocação da syscall.
 */
bool fs::mkfifo(const std::string& path, mode_t mode) throw(fs_exp)
{
	bool ret = false;

	try {
		ret = fs::create_node(path, S_IFIFO|mode, 0);
	}
	catch (fs_exp* e)   {
		throw;
	}

	return ret;
}


/**
 \brief Obtém descrição de erro gerado por opendir.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_opendir(int code) NO_THROW
{
	std::string	msg;

	switch (code)
	{
		case EACCES			:
			msg = "Permissao de busco negada a alguns dos componentes do path.";
			break;
		case ELOOP			:
			msg = "Identificado loop em link simbolico na resolucao do caminho.";
			break;
		case ENAMETOOLONG   :
			msg = "Caminho ou um de seus componentes muito longo.";
			break;
		case ENOENT			:
			msg = "Diretorio nao encontrado (ou nao informado).";
			break;
		case ENOTDIR		:
			msg = "Caminho nao corresponde a um diretorio.";
			break;
		case EMFILE			:
			msg = "Numero maximo de descritores utilizados por este processo.";
			break;
		case ENFILE			:
			msg = "Muitos arquivos abertos.";
			break;
		default				:
			msg = "Erro desconhecido (opendir(...)).";
	}

	return msg;
}


/**
 \brief Obtém descrição de erro gerado por closedir.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_closedir(int code) NO_THROW
{
	std::string	msg;

	switch (code)
	{
		case EBADF			:
			msg = "Descritor de diretorio invalido.";
			break;
		case EINTR			:
			msg = "closedir() interrompido por signal.";
			break;
		default				:
			msg = "Erro desconhecido (closedir(...)).";
	}

	return msg;
}


/**
 \brief Obtém descrição de erro gerado por inotify_init.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_inotify_init(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EINVAL			:
			msg = "Valor invalido especificado para 'flags'.";
			break;
		case EMFILE			:
			msg = "Numero maximo de instancias de inotify alcancado.";
			break;
		case ENFILE			:
			msg = "Numero maximo de descritores alcancado.";
			break;
		case ENOMEM			:
			msg = "Memoria do kernel insuficiente.";
			break;
		default				:
			msg = "Erro desconhecido (inotify_init()).";
	}

	return msg;
}


/**
 \brief Obtém descrição de erro gerado por inotify_add_watch.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_inotify_add_watch(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EACCES			:
			msg = "Acesso de leitura nao permitido.";
			break;
		case EBADF			:
			msg = "Descritor invalido.";
			break;
		case EFAULT			:
			msg = "Pathname fora do espaco de enderecamento do processo.";
			break;
		case EINVAL			:
			msg = "Mascara de eventos invalida ou descritor nao 'inotify'.";
			break;
		case ENOENT			:
			msg = "Caminho invalido.";
			break;
		case ENOMEM			:
			msg = "Memoria do kernel insuficiente.";
			break;
		case ENOSPC			:
			msg = "Limite de uso de recursos atingido.";
			break;
		default				:
			msg = "Erro desconhecido (inotify_add_watch(...)).";
	}

	return msg;
}


/**
 \brief Obtém descrição de erro gerado por inotify_rm_watch.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_inotify_rm_watch(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EBADF			:
			msg = "Descritor invalido.";
			break;
		case EINVAL			:
			msg = "Descritor de aquivo ou de watch invalido.";
			break;
		default				:
			msg = "Erro desconhecido (inotify_rm_watch(...)).";
	}

	return msg;
}


/**
 \brief Obtém descrição de erro gerado por select.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_select(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EBADF			:
			msg = "Pelo menos um descritor de fs_set e' invalido.";
			break;
		case EINTR			:
			msg = "Capturada ocorrencia de 'signal'.";
			break;
		case EINVAL			:
			msg = "Numero negativo de FDs ou timeout invalido.";
			break;
		case ENOMEM			:
			msg = "Impossivel alocar memoria para tabelas internas.";
			break;
		default				:
			msg = "Erro desconhecido (select(...)).";
	}

	return msg;
}


/**
 \brief Obtém descrição de erro gerado por read.
 \param code Código de erro (errno).
 \returns String contendo mensagem de erro.
 */
std::string fs::err_read(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EAGAIN			:
//		case EWOULDBLOCK	:
			msg = "Descritor em uso. Tente ler novamente.";
			break;
		case EBADF			:
			msg = "Descritor invalido.";
			break;
		case EFAULT			:
			msg = "Buffer fora do espaco de enderamento.";
			break;
		case EINTR			:
			msg = "Leitura interrompida por 'signal'.";
			break;
		case EINVAL			:
			msg = "Argumentos de leitura invalidos.";
			break;
		case EIO			:
			msg = "Erro de I/O.";
			break;
		case EISDIR			:
			msg = "Descritor associado a um diretorio.";
			break;
		default				:
			msg = "Erro desconhecido (read(...)).";
	}

	return msg;
}


/**
 \brief Obtém mensagem descritiva de erro.
 \details A mensagem resultante, corresponde ao código de erro passado como 
 argumento. O conjunto de mensagens corresponde exclusivamente aos erros
 produzidos pela invocação da syscall "access".
 \param code Código de erro.
 \returns Vetor de caracteres contendo a mensagem.
 */
const char* fs::access_err_msg(int code) NO_THROW
{
	std::string msg;
	
	switch (code)
	{
		case EACCES:		{
			msg = "Acesso negado";
			break;
		}
		case ELOOP:			{
			msg = "Muitos links simbolicos";
			break;
		}
		case ENAMETOOLONG:  {
			msg = "Pathname muito longo";
			break;
		}
		case ENOENT:		{
			msg = "Arquivo inexistente ou link simbolico quebrado";
			break;
		}
		case ENOTDIR:		{
			msg = "Pathname (total ou parcial) nao eh realmente um diretorio";
			break;
		}
		case EROFS:			{
			msg = "Arquivo somente de leitura";
			break;
		}
		case EFAULT:		{
			msg = "Pathname fora do espaco enderecavel";
			break;
		}
		case EINVAL:		{
			msg = "Modo especificado incorretamente";
			break;
		}
		case EIO:			{
			msg = "Erro de entrada/saida ocorrido";
			break;
		}
		case ENOMEM:		{
			msg = "Memoria parao kernel insuficiente";
			break;
		}
		case ETXTBSY:		{
			msg = "Erro ao gravar arquivo executavel sendo executado";
			break;
		}
		default:		{
			msg = "Erro desconhecido durante a chamada \"access\"";
			break;
		}
	}

	return (msg.c_str());
}


/**
 \brief Obtém mensagem descritiva de erro.
 \details A mensagem resultante, corresponde ao código de erro passado como 
 argumento. O conjunto de mensagens exclui a avaliação de erros da syscall
 "access".
 \param code Código de erro.
 \returns Vetor de caracteres contendo a mensagem.
 */
const char* fs::fs_err_msg(int code) NO_THROW
{
	std::string msg;

	switch (code)
	{
		case EACCES:		{
			msg = "Acesso negado";
			break;
		}
		case EEXIST:		{
			msg = "Arquivo de destino ja existe";
			break;
		}
		case EFAULT:		{
			msg = "Arquivo ou link simbolico fora da faixa de enderecamento";
			break;
		}
		case EIO:			{
			msg = "Erro de entrada/saida";
			break;
		}
		case ELOOP:			{
			msg = "Muitos links simbolicos no caminho do arquivo";
			break;
		}
		case ENAMETOOLONG:  {
			msg = "Caminho de arquivo ou link simbolico muito longo";
			break;
		}
		case ENOENT:		{
			msg = "Falha ao encontrar arquivo, ou link e uma string vazia";
			break;
		}
		case ENOMEM:		{
			msg = "Memoria do kernel insuficiente";
			break;
		}
		case ENOSPC:		{
			msg = "Diretorio destino cheio";
			break;
		}
		case ENOTDIR:		{
			msg = "Algun componente do path destino nao eh diretorio";
			break;
		}
		case EPERM:			{
			msg = "Criacao de arquivo ou link simbolico nao permitida";
			break;
		}
		case EROFS:			{
			msg = "Filesystem de destino eh somente para leitura";
			break;
		}
		default:			{
			msg = "Erro desconhecido acessando filesystem";
			break;
		}
	}

	return (msg.c_str());
}


/**
 \brief Cria nome de arquivo aleatório.
 \details Utiliza uma base para ser o radical do nome, e o PID atual, 
 embaralhado para produzir um nome único.
 \param base Radical parao nome de arquivo.
 \returns String contendo nome "randomizado".
 */
std::string fs::random_name(const std::string& base) NO_THROW
{
	pid_t		my_pid = getpid();
	std::string rname = base;
	char		aux[7];

	sprintf(aux, "%06d", my_pid);

	if (!strfry(aux))   {
		sprintf(aux, "%06d", my_pid);
	}

	rname += aux;

	return rname.c_str();
}


/**
 \brief Copia o conteúdo de um arquivo para outro.
 \param orig Nome (path completo) do arquivo de origem.
 \param dest Nome (path completo) do arquivo de destino.
 */
void fs::copy_file(const std::string& orig, const std::string& dest)
{
	try {
		std::ifstream   in(orig.c_str());
		std::ofstream   out(dest.c_str());

		out << in.rdbuf();

		out.close();
		in.close();
	}
	catch (...) {
		throw;
	}
}