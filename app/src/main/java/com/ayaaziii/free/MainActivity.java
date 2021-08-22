package com.ayaaziii.free;

import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.topjohnwu.superuser.Shell;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {
    static int GameType = 1;


    static{
        System.loadLibrary("kmods");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        copyFile("kdaemon");
        copyFile("start.sh");
        copyFile("stop.sh");
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_main);


        if(Shell.rootAccess()){
            Shell.su("setenforce 0").submit();
        }

        if (Build.VERSION.SDK_INT >= 23 && !Settings.canDrawOverlays(this)) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 123);
        }

        Spinner gameType = findViewById(R.id.gameType);
        gameType.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                GameType = position + 1;
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {}
        });

        Button mStart = findViewById(R.id.startbtn);
        mStart.setOnClickListener(v -> startPatcher());
        Button mStop = findViewById(R.id.stopbtn);
        mStop.setOnClickListener(v -> stopservices());
        Button mProtectStart = findViewById(R.id.startProtected);
        mProtectStart.setOnClickListener(v -> startservices());
    }

    void stopservices(){
        stopService(new Intent(this, Floater.class));
        Utils.stopServices(this);

    }

    private void copyFile(String filename) {
        try {
            File file = new File(getFilesDir(), filename);

            InputStream in = getAssets().open(filename);
            OutputStream out = new FileOutputStream(file);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }

            in.close();
            out.flush();
            out.close();

            file.setExecutable(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    void startservices(){
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {

                startPatcher();
            }
        },35*1000);
        Utils.startGameProtected(getApplicationContext());

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 123) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                if (!Settings.canDrawOverlays(this)) {
                    Toast.makeText(this, "Please allow this permission, so " + getString(R.string.app_name) + " could be drawn.", Toast.LENGTH_LONG).show();
                }
            }
        }
    }

    void startPatcher() {
        if (Build.VERSION.SDK_INT >= 23 && !Settings.canDrawOverlays(this)) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 123);
        } else {
            startFloater();
        }
    }

    private void startFloater() {
        stopService(new Intent(this, Floater.class));
        startService(new Intent(this, Floater.class));
    }

    }

