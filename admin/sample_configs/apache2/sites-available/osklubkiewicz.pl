<VirtualHost *:80> 
     ServerAdmin webmaster@example.org     
     ServerName osklubkiewicz.pl
     ServerAlias www.osklubkiewicz.pl
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
     ServerName osklubkiewicz.pl
     ServerAlias www.osklubkiewicz.pl
     DocumentRoot /var/www/www-kinga/kinga

 <Directory /var/www/www-kinga/kinga>
    Options FollowSymLinks MultiViews
    AllowOverride All
    Order allow,deny
    allow from all
  </Directory>
</VirtualHost>
