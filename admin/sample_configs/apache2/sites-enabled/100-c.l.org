
<VirtualHost *:80>
        ServerName www.c.l
	ServerAlias c.l
	RewriteEngine on
        ReWriteCond %{SERVER_PORT} !^443$
        RewriteRule ^/(.*) https://%{HTTP_HOST}/$1 [NC,R,L]
</VirtualHost>

<VirtualHost *:443>
        ServerName www.c.l
	ServerAlias c.l


	SSLEngine on
	SSLCertificateFile    /etc/ssl/certs/ssl-cert-snakeoil.pem
	SSLCertificateKeyFile /etc/ssl/private/ssl-cert-snakeoil.key


	DocumentRoot /usr/share/owncloud
	<Directory />
    		Options +FollowSymLinks
	        AllowOverride ALL
	    	<IfVersion < 2.3>
		        order allow,deny
        		allow from all
		</IfVersion>
    		<IfVersion >= 2.3>
		        Require all granted
		</IfVersion>
	</Directory>

ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/
	<Directory "/usr/lib/cgi-bin">
		AllowOverride None
		Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
		Order allow,deny
		Allow from all
	</Directory>

	ErrorLog ${APACHE_LOG_DIR}/error.log

	# Possible values include: debug, info, notice, warn, error, crit,
	# alert, emerg.

	CustomLog ${APACHE_LOG_DIR}/access.log combined

	<FilesMatch "\.(cgi|shtml|phtml|php)$">
		SSLOptions +StdEnvVars
	</FilesMatch>
	<Directory /usr/lib/cgi-bin>
		SSLOptions +StdEnvVars
	</Directory>

</VirtualHost>

