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

#ifndef _FS_H_
#define _FS_H_

#include <exception>
#include <string>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "excp.h"


/**
 \class fs
 \brief Coleção de métodos estáticos para uso do sistema de arquivos.
 */
class fs
{
	public:
		static void			set_umask(mode_t mask) NO_THROW;
		static bool			create_node(const std::string& path, mode_t mode,
										dev_t dev)
								throw(fs_exp);
		static bool			found(const std::string& path)
								throw(fs_exp);
		static bool			sys_access(const std::string& path, int mode)
								throw(fs_exp);
		static bool			sys_unlink(const std::string& path)
								throw(fs_exp);
		static bool			sys_symlink(const std::string& file_path,
										const std::string& link_path)
								throw(fs_exp);
		static bool			sys_mkdir(const std::string& path, mode_t mode)
								throw(fs_exp);
		static bool			sys_rmdir(const std::string& path)
								throw(fs_exp);
		static bool			mkfifo(const std::string& path, mode_t mode)
								throw(fs_exp);
		static std::string	err_opendir(int code) NO_THROW;
		static std::string	err_closedir(int code) NO_THROW;
		static std::string  err_inotify_init(int code) NO_THROW;
		static std::string  err_inotify_add_watch(int code) NO_THROW;
		static std::string  err_inotify_rm_watch(int code) NO_THROW;
		static std::string  err_select(int code) NO_THROW;
		static std::string  err_read(int code) NO_THROW;
		static std::string  random_name(const std::string& base) NO_THROW;
		static void			copy_file(const std::string& orig, 
										const std::string& dest);

	private:
		static const char*  access_err_msg(int code) NO_THROW;
		static const char*  fs_err_msg(int code) NO_THROW;
		
};

#endif // _FS_H_
