<VirtualHost *:80> 
     ServerAdmin webmaster@example.org     
     ServerName ot.l
     ServerAlias www.ot.l
     DocumentRoot /var/www/www-ot/ot/

 <Directory /var/www/www-ot/ot>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>

<VirtualHost *:443> 
     ServerAdmin webmaster@example.org     
     ServerName ot.l
     ServerAlias www.ot.l
     DocumentRoot /var/www/www-ot/ot/



	SSLEngine on
	SSLCertificateFile    /etc/ssl/certs/ssl-cert-snakeoil.pem
	SSLCertificateKeyFile /etc/ssl/private/ssl-cert-snakeoil.key


 <Directory /var/www/www-ot/ot>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>
