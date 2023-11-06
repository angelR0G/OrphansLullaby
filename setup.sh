
##For the first use give permissions writting on the terminal chmod +x setup.sh
##Install unzip in linux manajaro doing pamac install unzip

##Code dor download libs and media

#Delete libs folder 
rm -Rf libs
#Create libs folder
mkdir libs
#Download libs from dropbox
wget https://www.dropbox.com/sh/hw2prqslv4ewu8r/AACJklVx4rj9BJU-cOqkEr6oa?dl=1
#Unzip contents on libs directory
unzip AACJklVx4rj9BJU-cOqkEr6oa?dl=1 -x / -d libs/
#Delete the zip
rm AACJklVx4rj9BJU-cOqkEr6oa?dl=1



#Delete media folder 
rm -Rf media
#Create media folder
mkdir media
#Download media from dropbox
wget https://www.dropbox.com/sh/isnoysa2o46umk2/AABpv6pW5NwttLqC-lOo-NUOa?dl=1
#Unzip contents on media directory
unzip AABpv6pW5NwttLqC-lOo-NUOa?dl=1 -x / -d media/
#Delete the zip
rm AABpv6pW5NwttLqC-lOo-NUOa?dl=1



#Delete tools folder 
rm -Rf tools
#Create tools folder
mkdir tools
#Download tools from dropbox
wget https://www.dropbox.com/sh/epaxlx42rf5t1v7/AAD4jUK8xtKme3HikDlIo1F3a?dl=1
#Unzip contents on tools directory
unzip AAD4jUK8xtKme3HikDlIo1F3a?dl=1 -x / -d tools/
#Delete the zip
rm AAD4jUK8xtKme3HikDlIo1F3a?dl=1