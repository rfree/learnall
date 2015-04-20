<VirtualHost *:80> 
     ServerAdmin webmaster@example.org     
     ServerName kinga.l
     ServerAlias osklubkiewicz.pl
     DocumentRoot /var/www/www-kinga/kinga

 <Directory /var/www/www-kinga/kinga>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>

<VirtualHost *:443> 
     ServerAdmin webmaster@example.org     
     ServerName kinga.l
     ServerAlias osklubkiewicz.pl
     DocumentRoot /var/www/www-kinga/kinga

SSLEngine on
SSLCertificateFile    /etc/ssl/certs/ssl-cert-snakeoil.pem
SSLCertificateKeyFile /etc/ssl/private/ssl-cert-snakeoil.key

 <Directory /var/www/www-kinga/kinga>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>
