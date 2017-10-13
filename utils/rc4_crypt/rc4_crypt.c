/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * rc4_crypt
 * Copyright (C) Chris Robert Tonini 2016 <chrisrtonini@gmail.com>
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

#include	<stddef.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<openssl/rc4.h>
#include	<openssl/md5.h>


/**
 \func ascii2bits Converte caracteres em base hexadecimal em seu valor decimal.
 \para byte Caracter a ser processado (0-9-A-F)
 \returns Valor inteiro, codificado em 'char'
 */
char ascii2bits(char byte)	{
	char	ret = 0x00;

	switch	(byte)	{
		case	'0'	:
			ret = 0x00;
			break;
		case	'1'	:
			ret = 0x01;
			break;
		case	'2'	:
			ret = 0x02;
			break;
		case	'3'	:
			ret = 0x03;
			break;
		case	'4'	:
			ret = 0x04;
			break;
		case	'5'	:
			ret = 0x05;
			break;
		case	'6'	:
			ret = 0x06;
			break;
		case	'7'	:
			ret = 0x07;
			break;
		case	'8'	:
			ret = 0x08;
			break;
		case	'9'	:
			ret = 0x09;
			break;
		case	'A'	:
		case	'a'	:
			ret = 0x0a;
			break;
		case	'B'	:
		case	'b'	:
			ret = 0x0b;
			break;
		case	'C'	:
		case	'c'	:
			ret = 0x0c;
			break;
		case	'D'	:
		case	'd'	:
			ret = 0x0d;
			break;
		case	'E'	:
		case	'e'	:
			ret = 0x0e;
			break;
		case	'F'	:
		case	'f'	:
			ret = 0x0f;
			break;
		default		:
			ret = 0x00;
			break;
	}

	return ret;
}

/**
 \func hex2bin Converte uma string (2 caracteres) hexadecimal para decimal.
 \param val String de 2 posicoes em formato hexadecimal (um byte).
 \returns Valor inteiro, codificado em 'char'.
 */
char hex2bin(char* val)	{
	char	ret;
	char	hi;
	char	lo;

	hi = ascii2bits(val[0]);
	lo = ascii2bits(val[1]);

	ret = (((hi << 4) & 0xF0) | (lo & 0x0F));

	return ret;
}


/**
 \details Utiliza libcrypto e libssl para realizar codificacao e decodificacao
 criptografica de string, utilizando o protocolo RC4.
 */
int main(int argc, char* argv[])	{

	unsigned char	digest[MD5_DIGEST_LENGTH];
	char*		pwd;
	char*		op;
	char*		in;
	unsigned char	buff[2048];
	unsigned char	data[2048];
	char		val[3];
	int		len, i;
	RC4_KEY		key;

	memset(buff, 0, 2048);
	memset(data, 0, 2048);
	memset(val,  0, 3);

	if (argc < 4)	{
		fprintf(stderr, "\n%s:\nArgumentos insuficientes!\nSintaxe:\n", argv[0]);
		fprintf(stderr, "\t%s <senha> <operacao> <dados>\n", argv[0]);
		return (EXIT_FAILURE);
	};

	pwd = argv[1];
	op  = argv[2];
	in  = argv[3];
	len = strlen(in);

	if (op[0] >= 97)	{
		op[0] -= ' ';
	}

	if ((op[0] != 'C') && (op[0] != 'D'))	{
		fprintf(stderr, "\n%s:\n Operacao invalida!\nValores aceitos:\n", argv[0]);
		fprintf(stderr, "<C>riptografa\n<D>ecritografa\n");
		return (EXIT_FAILURE);
	}

	if (len > 2048)	{
		fprintf(stderr,
			"\n%s:\nO tamanho maximo do bloco de dados e 2048 bytes!\n", argv[0]);
		return (EXIT_FAILURE);
	}

	if (op[0] == 'D')	{
		len = (len / 2);
		i = 0;
		while (i < len)	{
			val[0] = in[(i*2)];
			val[1] = in[((i*2)+1)];
			buff[i++] = hex2bin(val);
		}
	}

	MD5((unsigned char*)pwd, strlen(pwd), digest);

	RC4_set_key(&key, MD5_DIGEST_LENGTH, digest);
	if (op[0] == 'C')	{
		RC4(&key, len, (const unsigned char*)in, data);
	}
	else	{
		RC4(&key, len, (const unsigned char*)buff, data);
	}

	if (op[0] == 'D')	{
		fprintf(stdout, "%s\n", data);
	}
	else	{
		for (i=0; i<len; i++)	{
			fprintf(stdout, "%02X", data[i]);
		}
		fprintf(stdout, "\n");
	}

	return (EXIT_SUCCESS);
}
