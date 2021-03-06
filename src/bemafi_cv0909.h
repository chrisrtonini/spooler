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

#ifndef _BEMAFI_CV0909_H_
#define _BEMAFI_CV0909_H_

#include "bemafi.h"

#ifdef WIN32

#define	DLL_IMP			__declspec(dllimport)
#define CALL_CONV		__stdcall

#else

#define	CALL_CONV

#ifdef __cplusplus

#define DLL_IMP
extern "C"	{

#else

#define DLL_IMP			extern

#endif

#endif

#define BEMAEX			DLL_IMP int CALL_CONV

	
// Funções de inicialização
BEMAEX Bematech_FI_NomeiaRelatorioGerencialCV0909(const char* idx,
	const char* titulo);
BEMAEX Bematech_FI_NomeiaTotalizadorNaoSujeitoIcmsCV0909(int idx,
    const char* descr, const char* tipo);
BEMAEX Bematech_FI_ProgramaAliquotaCV0909(const char* valor, int tipo,
    const char* idx);
BEMAEX Bematech_FI_ProgramaFormaPagamentoCV0909(const char* idx,
    const char* descr, int vinc);
BEMAEX Bematech_FI_ProgramaHorarioVeraoCV0909(int modo);

// Funções de Cupom Fiscal
BEMAEX Bematech_FI_AbreCupomCV0909(const char* doc, const char* nome,
    const char* endr);
BEMAEX Bematech_FI_AcrescimoDescontoItemCV0909(const char* item, 
    const char* tipo, const char* modo, const char* valor);
BEMAEX Bematech_FI_AcrescimoDescontoSubtotalCV0909(const char* tipo,
    const char* modo, const char* valor);
BEMAEX Bematech_FI_CancelaAcrescimoDescontoItemCV0909(const char* tipo,
    const char* item);
BEMAEX Bematech_FI_CancelaAcrescimoDescontoSubtotalCV0909(const char* tipo);
BEMAEX Bematech_FI_CancelaCupomAtualCV0909(void);
BEMAEX Bematech_FI_EfetuaFormaPagamentoIndiceCV0909(const char* idx,
    const char* vlr, const char* parc, const char* descr, const char* codpgto);
BEMAEX Bematech_FI_TerminaFechamentoCupomCV0909(const char* msg, int adic, 
    int corte);
BEMAEX Bematech_FI_VendeItemCV0909(const char* cod, const char* descr,
    const char* aliq, const char* qtde, int dec_qtde, const char* vlr,
    const char* un, int dec_vlr, const char* modo);
// IMPORTANTE: na função Bematech_FI_VendeItemCV0909, há divergência na docu-
//				mentação sobre o tipo de dado do parâmetro "dec_vlr". No github
//				encontrei "int", e no CHM fornecido pela Bematech, "const 
//				char*". Contudo, o parâmetro (análogo) "dec_qtde", em ambas
//				fontes, encontra-se definido como "int".

// Funções de informação da impressora
BEMAEX Bematech_FI_BufferRespostaCV0909(char* buffer);
BEMAEX Bematech_FI_DadosUltimaReducaoCV0909(char* buffer);
BEMAEX Bematech_FI_MinutosEmitindoDocumentosFiscaisCV0909(char* minutos);
BEMAEX Bematech_FI_NumeroCupomCV0909(char* coo);
BEMAEX Bematech_FI_NumeroOperacoesNaoFiscaisCV0909(char* numero);
BEMAEX Bematech_FI_NumeroSerieCV0909(char* nr_serie);
BEMAEX Bematech_FI_RetornoAliquotasCV0909(char* buffer);
BEMAEX Bematech_FI_RetornoImpressoraCV0909(int* cat, int* ret1, int* ret2,
    int* ret3, int* ret4);
BEMAEX Bematech_FI_TempoEmitindoOperacionalCV0909(char* emitindo,
    char* operacional);
BEMAEX Bematech_FI_VerificaFormasPagamentoCV0909(char* buffer);
BEMAEX Bematech_FI_VerificaIndiceAliquotasIssCV0909(char* buffer);
BEMAEX Bematech_FI_VerificaRecebimentoNaoFiscalCV0909(char* buffer);
BEMAEX Bematech_FI_VerificaTotalizadoresNaoFiscaisCV0909(char* buffer);
BEMAEX Bematech_FI_VersaoFirmwareCV0909(char* versao);

// Funções de operações não fiscais
BEMAEX Bematech_FI_AbreComprovanteNaoFiscalVinculadoCV0909(int seq, 
    const char* idx, int parcelas, int parcela, const char* doc,
    const char* nome, const char* endr);
BEMAEX Bematech_FI_AbreRecebimentoNaoFiscalCV0909(const char* doc,
    const char* nome, const char* endr);
BEMAEX Bematech_FI_AbreRelatorioGerencialCV0909(const char* titulo);
BEMAEX Bematech_FI_EfetuaRecebimentoNaoFiscalCV0909(const char* idx_tot,
    const char* valor);
BEMAEX Bematech_FI_EstornoFormasPagamentoCV0909(const char* orig,
    const char* dest, const char* valor, int seq, const char* msg);
BEMAEX Bematech_FI_EstornoNaoFiscalVinculadoCV0909(const char* doc,
    const char* nome, const char* endr, const char* coo);
BEMAEX Bematech_FI_FechaRecebimentoNaoFiscalCV0909(const char* msg, int corte);
BEMAEX Bematech_FI_FechaRelatorioGerencialCV0909(int corte);
BEMAEX Bematech_FI_SangriaCV0909(const char* valor, const char* msg);
BEMAEX Bematech_FI_SuprimentoCV0909(const char* valor, const char* msg);
BEMAEX Bematech_FI_SegundaViaNaoFiscalVinculadoCV0909(void);
BEMAEX Bematech_FI_ReimpressaoNaoFiscalVinculadoCV0909(void);
BEMAEX Bematech_FI_UsaRelatorioGerencialCV0909(const char* texto);

// Funções de relatórios fiscais
BEMAEX Bematech_FI_LeituraMemoriaFiscalDataCV0909(const char* ini, 
    const char* fim, const char* flag);
BEMAEX Bematech_FI_LeituraMemoriaFiscalReducaoCV0909(const char* ini,
    const char* fim, const char* flag);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialDataCV0909(const char* ini,
    const char* fim, const char* flag);
BEMAEX Bematech_FI_LeituraMemoriaFiscalSerialReducaoCV0909(const char* ini,
    const char* fim, const char* flag);
BEMAEX Bematech_FI_ReducaoZCV0909(const char* data, const char* hora,
    int transmite);

// Outras funções
BEMAEX Bematech_FI_AcionaGuilhotinaCV0909(int modo);
BEMAEX Bematech_FI_DownloadMFCV0909(const char* path, const char* tipo,
    const char* ini, const char* fim);
BEMAEX Bematech_FI_DownloadMFDCV0909(const char* path, const char* tipo,
    const char* ini, const char* fim);
BEMAEX Bematech_FI_DownloadSBCV0909(const char* path);
BEMAEX Bematech_FI_ImpressaoFitaDetalheCV0909(const char* tipo, const char* ini,
    const char* fim);
BEMAEX Bematech_FI_ImprimeRTDCV0909(const char* msg);
BEMAEX Bematech_FI_InterrompeLeiturasCV0909(void);
BEMAEX Bematech_FI_ProgramaIdAplicativoCV0909(const char* id);


#ifndef WIN32

#ifdef __cplusplus
}

#endif

#endif

#endif
