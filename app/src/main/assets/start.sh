am force-stop com.tencent.ig

rm -rf /data/data/com.tencent.ig/files
touch /data/data/com.tencent.ig/files
rm -rf /data/data/com.tencent.ig/app_crashrecord
touch /data/data/com.tencent.ig/app_crashrecord

echo "done. starting game now"
sleep 1
am start -n com.tencent.ig/com.epicgames.ue4.SplashActivity


