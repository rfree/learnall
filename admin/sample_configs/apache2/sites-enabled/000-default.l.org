<VirtualHost *:80> 
     ServerAdmin webmaster@example.org     
     ServerName default.l
     DocumentRoot /var/www/www-defaultpage/

 <Directory /var/www/www-defaultpage>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>

<VirtualHost *:443> 
     ServerAdmin webmaster@example.org     
     ServerName default.l
     DocumentRoot /var/www/www-defaultpage/

	SSLEngine on
	SSLCertificateFile    /etc/ssl/certs/ssl-cert-snakeoil.pem
	SSLCertificateKeyFile /etc/ssl/private/ssl-cert-snakeoil.key

 <Directory /var/www/www-defaultpage>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>

