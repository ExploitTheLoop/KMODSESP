package com.ayaaziii.free;

import android.content.Context;
import android.provider.Settings;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Random;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Utils {

    public static String getDevice(Context context){


        return Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);



    }



    public static void WriteEternal(Context context, String file, String data) {
        try {
            OutputStreamWriter outputStreamWriter = new OutputStreamWriter(context.openFileOutput(file, Context.MODE_PRIVATE));
            outputStreamWriter.write(data);
            outputStreamWriter.close();
        }
        catch (IOException e) {
            Log.e("Exception", "File write failed: " + e.toString());
        }
    }



    public static String ReadEternal(Context context,String filename) {

        String ret = "";

        try {
            InputStream inputStream = context.openFileInput(filename);

            if ( inputStream != null ) {
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
                String receiveString = "";
                StringBuilder stringBuilder = new StringBuilder();

                while ( (receiveString = bufferedReader.readLine()) != null ) {
                    stringBuilder.append(receiveString);
                }

                inputStream.close();
                ret = stringBuilder.toString();
            }
        }

        catch (FileNotFoundException e) {
            Log.e("login activity", "File not found: " + e.toString());
        } catch (IOException e) {
            Log.e("login activity", "Can not read file: " + e.toString());
        }

        return ret;
    }

/*

    public static String decryptString(String enc){
    //    MC mc = new MC();

        String dec = null;

        try {
            dec = new String(mc.decrypt(enc));
        } catch (Exception e) {
            e.printStackTrace();
        }

        return dec;
    }
*/
    public static void getRoot(){


        File vxposed = new File("/storage/emulated/0/vxposed/");
        if(!vxposed.exists()){
            vxposed.mkdir();
        }




        try{
            Process su = Runtime.getRuntime().exec("su");
            DataOutputStream outputStream = new DataOutputStream(su.getOutputStream());





            outputStream.writeBytes("su -c iptables -F\n");
            outputStream.flush();




            outputStream.writeBytes("exit\n");
            outputStream.flush();




            su.waitFor();

        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }


    public static boolean unpackZip(String path, String zipname)
    {
        InputStream is;
        ZipInputStream zis;
        try
        {
            String filename;
            is = new FileInputStream(path + zipname);
            zis = new ZipInputStream(new BufferedInputStream(is));
            ZipEntry ze;
            byte[] buffer = new byte[1024];
            int count;

            while ((ze = zis.getNextEntry()) != null)
            {
                filename = ze.getName();

                // Need to create directories if not exists, or
                // it will generate an Exception...
                if (ze.isDirectory()) {
                    File fmd = new File(path + filename);
                    fmd.mkdirs();
                    continue;
                }

                FileOutputStream fout = new FileOutputStream(path + filename);

                while ((count = zis.read(buffer)) != -1)
                {
                    fout.write(buffer, 0, count);
                }

                fout.close();
                zis.closeEntry();
            }

            zis.close();
        }
        catch(IOException e)
        {
            e.printStackTrace();
            return false;
        }

        return true;
    }


    public static String getSaltString() {
        String SALTCHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        StringBuilder salt = new StringBuilder();
        Random rnd = new Random();
        while (salt.length() < 5) { // length of the random string.
            int index = (int) (rnd.nextFloat() * SALTCHARS.length());
            salt.append(SALTCHARS.charAt(index));
        }
        String saltStr = salt.toString();
        return saltStr;

    }



    public static void startGameProtected(Context context){
        String filesdir = context.getFilesDir().getAbsolutePath();
        String shell = filesdir+"/start.sh";
        try{
            Process su = Runtime.getRuntime().exec("su");
            DataOutputStream outputStream = new DataOutputStream(su.getOutputStream());

            outputStream.writeBytes("sh %s\n".replace("%s",shell));
            outputStream.flush();

            outputStream.writeBytes("exit\n");
            outputStream.flush();



        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void stopServices(Context context){
        String filesdir = context.getFilesDir().getAbsolutePath();
        String shell = filesdir+"/stop.sh";

        try{
            Process su = Runtime.getRuntime().exec("su");
            DataOutputStream outputStream = new DataOutputStream(su.getOutputStream());

            outputStream.writeBytes("am force-stop %s\n".replace("%s",context.getPackageName())); outputStream.flush();
            outputStream.writeBytes("am force-stop %s\n".replace("%s","com.tencent.ig")); outputStream.flush();

            // outputStream.writeBytes("sh %s\n".replace("%s",shell));
            outputStream.flush();

            outputStream.writeBytes("exit\n");
            outputStream.flush();





        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}
