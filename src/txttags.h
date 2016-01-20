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

#ifndef _TXTTAGS_H_
#define _TXTTAGS_H_

#define SERVER_ID_TAG			"server_id:"
#define SERVER_ID_TAG_LEN		10

#define SESSION_ID_TAG			"session_id:"
#define SESSION_ID_TAG_LEN		11

#define COMMAND_ID_TAG			"command_id:"
#define COMMAND_ID_TAG_LEN		11

#define PARAM_SEPARATOR			":"

#define PARAM_MSG_BEGIN			"mensagem{"
#define PARAM_MSG_BEGIN_LEN		9
#define PARAM_MSG_END			"}"
#define PARAM_MSG_END_LEN		1

#endif  // _TXTTAGS_H_
