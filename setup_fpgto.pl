#!/usr/bin/perl

# ----------------------------------------------------------------------------
# Utilitário para carregar tabela de Meios de Pagamento, a partir de um 
# arquivo de texto contendo um registro por linha, de acordo com o seguinte
# formato:
# NOME MOEDA:V
# Onde "V" pode ser "S" indicando vinculacao da moeda com TEF, ou "N", caso
# contrario.
# ----------------------------------------------------------------------------

# setup_fpgto.pl
#
# Copyright (C) Chris Robert Tonini 2015 <chrisrtonini@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
# 
# YOU SHOULD HAVE RECEIVED A COPY OF THE GNU GENERAL PUBLIC LICENSE ALONG
# WITH THIS PROGRAM.  IF NOT, SEE <http://www.gnu.org/licenses/>.

use 5.008;
use strict;
use warnings FATAL => 'all';

use Carp qw(carp croak confess cluck);
use Getopt::Long;


# Variaveis globais
our $id_sessao;
our $hnd;
our $arq_cmd = "arqcmd.9999";
our $arq_ret = "arqret.9999";
our $arq_tmp = "arqtmp.9999";
our $ret_lck = "arqret.9999.lck";


# Rotina para verificar se ha resposta do spooler
sub wait_for_lock	{
	my $count = 0;
	my $found = 0;
	
	do	{
		$found = 1 if (-e $ret_lck);
		if (! $found)	{
			sleep 1;
			$count++;
		}
	} while ((! $found) and ($count < 30));
	
	return $found;
}


# Limpar arquivos de resposta
sub clean_up	{
	unlink($arq_ret);
	unlink($ret_lck);
}


# Rotina para abertura de sessao.
sub begin_session	{
	open($hnd, ">", $arq_tmp) 
		or confess "begin_session: erro ao criar arquivo $arq_tmp ($!)\n";
	
	print $hnd "<ecf_sgi_begin>\n";
	print $hnd "ecf_begin_session_request\n";
	print $hnd "<ecf_sgi_end>\n";
	
	close($hnd);
	rename($arq_tmp, $arq_cmd);
	
	confess "begin_session: timeout (30s) !\n" if (!wait_for_lock());
	
	$id_sessao = "";
	
	open($hnd, "<", $arq_ret)
		or confess "begin_session: erro ao abrir arquivo $arq_ret ($!)\n";
	
	while (my $line = <$hnd>)	{
		chomp($line);
		if ($line =~ /session_id/)	{
			$id_sessao = $line;
			$id_sessao =~ s/.*session_id://;
			print "Sessao aberta: $id_sessao\n";
		}
	}
	
	close($hnd);
	
	clean_up();
}


# Rotina para encerramento de sessao
sub end_session	{
	open($hnd, ">", $arq_tmp)
		or confess "end_session: erro ao criar arquivo $arq_tmp ($!)\n";
	
	print $hnd "<ecf_sgi_begin>\n";
	print $hnd "ecf_end_session_request\n";
	print $hnd "session_id:$id_sessao\n";
	print $hnd "<ecf_sgi_end>\n";
	
	close($hnd);
	rename($arq_tmp, $arq_cmd);
	
	confess "end_session: timeout (30s) !\n" if (!wait_for_lock());
	
	open($hnd, "<", $arq_ret)
		or confess "end_session: erro ao abrir arquivo $arq_ret ($!)\n";
	
	my $get_code = 0;
	my $get_msg = 0;
	my $ret_code = 0;
	while (my $line = <$hnd>)	{
		chomp($line);
		if ($get_msg)	{
			$get_msg = 0;
			print "Msg: $line\n";
		}
		if ($get_code)	{
			$get_code = 0;
			$ret_code = $line;
			if ($ret_code == 0)	{
				print "Sessao encerrada!\n";
			}
			else	{
				$get_msg = 1;
				print "Encerramento de sessao: $ret_code\n";
			}
		}
		if ($line =~ /session_id/)	{
			$get_code = 1;
		}
	}
	
	close($hnd);
	
	clean_up();
}


sub set_moeda	{
	my $descr	= shift;
	my $flag	= shift;
	my $retcode	= undef;
	my $count	= 0;
	
	print "Programar: [$descr][$flag]\n";
	
	open($hnd, ">", $arq_tmp) 
		or confess "set_moeda: erro ao criar arquivo $arq_tmp ($!)\n";
	
	print $hnd "<ecf_sgi_begin>\n";
	print $hnd "ecf_execute_request\n";
	print $hnd "session_id:$id_sessao\n";
	print $hnd "command_id:set_forma_pgto\n";
	print $hnd "descricao:$descr\n";
	print $hnd "vinculado:";
	print $hnd ($flag eq "S" ? "true" : "false") . "\n";
	print $hnd "<ecf_sgi_end>\n";
	
	close($hnd);
	rename($arq_tmp, $arq_cmd);
	
	confess "set_moeda: timeout (30s) !\n" if (!wait_for_lock());
	
	open($hnd, "<", $arq_ret)
		or confess "set_moeda: erro ao abrir arquivo $arq_ret ($!)\n";
	
	while (my $line = <$hnd>)	{
		$count ++;
		# <ecf_sgi_begin>
		# ecf_execute_response
		# session_id:...
		# [retcode]
		# [msgs]
		# <ecf_sgi_end>
		if ($count == 4)	{
			chomp($line);
			$retcode = $line;
			print "Retorno: $retcode\n";
		}
		elsif ($count > 4)	{
			if ($retcode > 0)	{
				chomp($line);
				if ($line !~ /<ecf_sgi_end>/)	{
					print "Msg: $line\n";
				}
			}
		}
	}
	
	close($hnd);
	
	clean_up();
}


sub usage	{
	print STDERR "\n-= Utilitario para setup de formas de pagamento =-\n";
	print STDERR "\nSintaxe:\n\tperl setup_fpgto.pl -f <arquivo.txt>\n";
	print STDERR "Onde:\n\t<arquivo.txt> contem lista de moedas.\n\n";
	print STDERR "Observacao:\n";
	print STDERR "\tCada linha do arquivo estara no seguinte formato:\n";
	print STDERR "\t\t\"NOME MOEDA:F\"\n";
	print STDERR "\tConsiderando que \"F\" tera valor \"S\" indicando ";
	print STDERR "vinculacao com TEF,\n ou \"N\", caso contrario.\n";
	print STDERR "\tA descricao sera limitada pelo tamanho maximo permitido ";
	print STDERR "do ECF.\n\n";
}


my $fpgto_file = "";
my $cmdline = GetOptions(
	"file=s"	=> \$fpgto_file
) or usage();

if ($cmdline)	{
	if (length($fpgto_file) == 0)	{
		usage();
	}
	elsif (-e $fpgto_file)	{
		begin_session();
		
		my $list;
		open($list, "<", $fpgto_file)
			or confess "Erro ao abrir arquivo $fpgto_file ($!)\n";
		
		while (my $item = <$list>)	{
			chomp($item);
			my ($nome_moeda, $flag_moeda) = split(/:/, $item);
			
			set_moeda($nome_moeda, $flag_moeda);
		}
		close($list);
		
		sleep 1;
		end_session();
	}
	else	{
		confess "Arquivo $fpgto_file nao pode ser lido!\n";
	}
}
