<VirtualHost *:80> 
     ServerAdmin webmaster@example.org     
     ServerName monero.l
     ServerAlias www.monero.l
     DocumentRoot /var/www/www-monero/monero/

 <Directory /var/www/www-monero/ot>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>

<VirtualHost *:443> 
     ServerAdmin webmaster@example.org     
     ServerName monero.l
     ServerAlias www.monero.l
     DocumentRoot /var/www/www-monero/monero/



	SSLEngine on
	SSLCertificateFile    /etc/ssl/certs/ssl-cert-snakeoil.pem
	SSLCertificateKeyFile /etc/ssl/private/ssl-cert-snakeoil.key


 <Directory /var/www/www-monero/monero>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>
