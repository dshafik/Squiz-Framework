This README describes how to configure the Exporting system to generate screenshots and PDFs using Firefox.

If this is for a dedicated MySource install, it's easier to set up.
This guide assumes the MySource install is in /var/www/mysource4
This guide assumes the webserver users' folder is /var/www

Extract the required resources:
cd /var/www/mysource4/Resources/firefox-setup
tar -xf mozilla_profile.tar.gz -C /var/www/
mv /var/www/mozilla_profile /var/www/.mozilla

Install common fonts:
tar -xzvf msttcorefonts.tar.gz
mv msttcorefonts /var/www/.fonts


If this is not for a dedicated MySource install, there are some manual steps to follow.

First, run Firefox for the first time to generate a profile.
This guide assumes the profile is /home/squiz/.mozilla/firefox/[profileID]
This guide assumes the MySource install is in /var/www/mysource4

Extract the required resources:
cd /var/www/mysource4/Resources/firefox-setup
tar -xzvf resources.tgz

Install the flash plugin:
mkdir /home/squiz/.mozilla/plugins
cp libflashplayer.so /home/squiz/.mozilla/plugins

Install the Command Line Print extension:
cp -r \{243cc1ee-c821-4544-aebb-54c44125b9aa\} /home/squiz/.mozilla/firefox/[profileID]/extensions/

Install common fonts:
tar -xzvf msttcorefonts.tar.gz
mv msttcorefonts /home/squiz/.fonts
