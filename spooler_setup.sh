#!/bin/bash

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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA


# Configuracoes
[ ! -d /etc/spooler ] && mkdir /etc/spooler
[ -e /etc/spooler/spooler.cfg ] && \
	mv -f /etc/spooler/spooler.cfg /etc/spooler/spooler.cfg.$$
install spooler/spooler.cfg /etc/spooler/ -o root -g root -m 644
if [ -e /etc/bemafi.xml ];
then
	TMP_FILE=/tmp/bemafi.xml.$$
	EDITED=""
	GO_ON="1"
	cat /etc/bemafi.xml | sed -e 's/$/\\n/g' | sed -e 's/\t/\\t/g' | \
	while [ "${GO_ON}" == "1" ];
	do
		read -r LINHA
		[ $? != 0 ] && GO_ON=0
		TST_PORTA=$(echo -e -n ${LINHA} | grep "<Porta>")
		TST_PATH=$(echo -e -n ${LINHA} | grep "<Path>")
		TST_PROTO=$(echo -e -n ${LINHA} | grep "<ProtocoloUnico>")
		
		if [ -n "${TST_PORTA}" ];
		then
			EDITED="\t\t<Porta>USB</Porta>"
		else
			if [ -n "${TST_PATH}" ];
			then
				EDITED="\t\t<Path>/tmp/</Path>"
			else
				if [ -n "${TST_PROTO}" ];
				then
					EDITED="\t\t<ProtocoloUnico>1</ProtocoloUnico>"
				else
					EDITED=$(echo ${LINHA})
				fi
			fi
		fi
		echo ${EDITED} | sed -e 's/\\n$//g' | sed -e 's/\\t/\t/g' >> ${TMP_FILE}
		
	done
	cp -v /etc/bemafi.xml /etc/bemafi.xml.$$
	mv ${TMP_FILE} /etc/bemafi.xml
	chown root:root /etc/bemafi.xml
	chmod 664 /etc/bemafi.xml
fi

# Utilitarios
install setup_totcnf.pl /usr/local/bin/ -o root -g root -m 755
install setup_fpgto.pl /usr/local/bin/ -o root -g root -m 755

# Servico
install ecf_spooler /etc/init.d -o root -g root -m 755
chkconfig --add ecf_spooler

service ecf_spooler start
