am force-stop com.tencent.ig
am force-stop com.pubg.krmobile

chmod 777 /data/media/0/Android/data/com.tencent.ig/

chmod 777 /data/media/0/Android/data/com.pubg.krmobile
su -c iptables -F

echo "done"