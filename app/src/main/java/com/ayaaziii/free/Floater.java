package com.ayaaziii.free;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import com.topjohnwu.superuser.Shell;

import java.io.DataOutputStream;
import java.io.IOException;

public class Floater extends Service {
    public static boolean isRunning = false;

    private WindowManager windowManager;
    private View mFloatingView;
    private LinearLayout patches;
    private TextView initText;
    private ESPView overlayView;

    @Override
    public void onCreate() {
        super.onCreate();

        windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        overlayView = new ESPView(this);
        mFloatingView = LayoutInflater.from(this).inflate(getResID("activity_floating", "layout"), null);

        DrawCanvas();
        FloatButton();
    }

    @Override
    public void onDestroy() {
        Stop();
        isRunning = false;
        if (mFloatingView != null) {
            windowManager.removeView(mFloatingView);
            mFloatingView = null;
        }
        if (overlayView != null) {
            windowManager.removeView(overlayView);
            overlayView = null;
        }
        super.onDestroy();
    }

    private void DrawCanvas() {
        final WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT,
                getLayoutType(),
                WindowManager.LayoutParams.FLAG_FULLSCREEN |
                        WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE |
                        WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL |
                        WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                PixelFormat.TRANSLUCENT);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }

        windowManager.addView(overlayView, params);
    }

    private void FloatButton() {
        final WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                getLayoutType(),
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);
        params.gravity = Gravity.TOP | Gravity.START;
        params.x = 0;
        params.y = 100;
        windowManager.addView(mFloatingView, params);
        final View floter = mFloatingView.findViewById(getID("floater_container"));
        final View menu = mFloatingView.findViewById(getID("menu_container"));

        patches = mFloatingView.findViewById(getID("patches"));

        floter.setVisibility(View.GONE);
        menu.setVisibility(View.VISIBLE);

        String htmltxt = "<html><head><style>body{color: white;font-weight:bold;font-family:Courier, monospace;}</style></head><body><marquee class=\"GeneratedMarquee\" direction=\"left\" scrollamount=\"4\" behavior=\"scroll\">ESP</marquee></body></html>";

        WebView wv = mFloatingView.findViewById(getID("webv"));
        wv.setBackgroundColor(Color.TRANSPARENT);
        wv.loadData(htmltxt, "text/html", "utf-8");

        mFloatingView.findViewById(getID("mclose")).setOnClickListener(view -> {
            floter.setVisibility(View.VISIBLE);
            menu.setVisibility(View.GONE);
        });
        mFloatingView.setOnTouchListener(new View.OnTouchListener() {
            private int initialX;
            private int initialY;
            private float initialTouchX;
            private float initialTouchY;

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        initialX = params.x;
                        initialY = params.y;
                        initialTouchX = event.getRawX();
                        initialTouchY = event.getRawY();
                        return true;
                    case MotionEvent.ACTION_UP:
                        v.performClick();
                        int differenceX = (int) Math.abs(initialTouchX - event.getRawX());
                        int differenceY = (int) Math.abs(initialTouchY - event.getRawY());
                        if (differenceX < 10 && differenceY < 10 && floter.getVisibility() == View.VISIBLE) {
                            floter.setVisibility(View.GONE);
                            menu.setVisibility(View.VISIBLE);
                        }
                        return true;
                    case MotionEvent.ACTION_MOVE:
                        params.x = initialX + (int) (event.getRawX() - initialTouchX);
                        params.y = initialY + (int) (event.getRawY() - initialTouchY);
                        windowManager.updateViewLayout(mFloatingView, params);
                        return true;
                }
                return false;
            }
        });

        initText = addText("Connecting Servers...");
        new Thread(() -> {
            if (!isRunning) {
                /*
                 * PUG Global = 1
                 * PUG KR = 2
                 * PUG VNG = 3
                 * PUG Taiwan = 4
                 */
                startDaemon(1);
                try {
                    Thread.sleep(1300);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (Init() < 0) {
                    new Handler(Looper.getMainLooper()).post(() -> {
                        Toast.makeText(Floater.this, "Game Not Running!", Toast.LENGTH_SHORT).show();
                        Floater.this.stopSelf();
                    });
                } else {
                    isRunning = true;
                    new Handler(Looper.getMainLooper()).post(Floater.this::startMenu);
                }
            }
        }).start();
    }

    private void startMenu(){
        patches.removeView(initText);
       /* addSpace(16);
        addText("-:Memory Hacks:-");
        addSwitch("Bypass Game Security", (buttonView, isChecked) -> Switch(19, isChecked));
        addSpace(12);
        addSwitch("Aimbot 1.1", (buttonView, isChecked) -> Switch(16, isChecked));
        addSwitch("Recoil Control 1.0 (Lobby)", (buttonView, isChecked) -> Switch(17, isChecked));
        addSwitch("MagicBullet + HS (Game)", (buttonView, isChecked) -> Switch(20, isChecked));
        addSwitch("Speed Hack", (buttonView, isChecked) -> Switch(21, isChecked)); */
        addSpace(25);
        addText("ESP FUNCTIONS");
        addSwitch("Activate ESP [Lobby]", (buttonView, isChecked) -> {
            Switch(0, isChecked);
        });
      /*  addSpace(25);
        addText("HACKS");
        addSwitch("ANTIREPORT [island]", ipjListener);
        addSwitch("Aimlock", (buttonView, isChecked) -> Switch(16, isChecked));
        addSwitch("Less Recoil", (buttonView, isChecked) -> Switch(17, isChecked));
        addSwitch("Headshot Magicbullet", (buttonView, isChecked) -> Switch(18, isChecked));*/
        addSpace(35);
        addText("PLAYER ESP MENU");
        addSwitch("Show Player Name", (buttonView, isChecked) -> Switch(1, isChecked));
        addSwitch("Show Player Health", (buttonView, isChecked) -> Switch(2, isChecked));
        addSwitch("Show Player Distance", (buttonView, isChecked) -> Switch(3, isChecked));
        addSwitch("Show Skeleton", (buttonView, isChecked) -> Switch(14, isChecked));
        addSwitch("Show Lines", (buttonView, isChecked) -> Switch(5, isChecked));
        addSwitch("Show Box", (buttonView, isChecked) -> Switch(6, isChecked));
        addSwitch("Show 360 Alert", (buttonView, isChecked) -> Switch(7, isChecked));
        addSwitch("Show Near Enemys", (buttonView, isChecked) -> Switch(8, isChecked));
        addSwitch("Show TeamMate", (buttonView, isChecked) -> Switch(4, isChecked));
        addSwitch("Ignore Bots", (buttonView, isChecked) -> Switch(35, isChecked));

        addSeekbar(30, 10, new SeekBar.OnSeekBarChangeListener() {
            TextView sizetext = addText("Player Name Size: 15");
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int pindex = (progress + 4);
                float psize = (progress + 4.0f);
                String tsize = "Player Name Size: " + pindex;
                sizetext.setText(tsize);
                Size(0, psize);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        addSpace(25);
        addText("ITEM MENU");
        addSpace(20);
        addSwitch("Show Name Vehicles", (buttonView, isChecked) -> Switch(9, isChecked));
        addSwitch("Show Distance items", (buttonView, isChecked) -> Switch(10, isChecked));
        addSwitch("Show Vehicles", (buttonView, isChecked) -> Switch(11, isChecked));
        addSwitch("Show LootBox", (buttonView, isChecked) -> Switch(12, isChecked));
        addSwitch("Show All Items", (buttonView, isChecked) -> Switch(13, isChecked));
        addSwitch("Show Airdrop", (buttonView, isChecked) -> Switch(15, isChecked));

        addSpace(16);
        addSeekbar(30, 10, new SeekBar.OnSeekBarChangeListener() {
            TextView sizetext = addText("Items Size: 15");
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int pindex = (progress + 4);
                float psize = (progress + 4.0f);
                String tsize = "Items Size: " + pindex;
                sizetext.setText(tsize);
                Size(1, psize);
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        Button shut = new Button(this);
        patches.addView(shut);
        shut.setBackgroundResource(R.drawable.center_fab);
        shut.setText("SHUT ESP");
        shut.setTextColor(Color.WHITE);
        shut.setOnClickListener(view -> Utils.stopServices(this));



    }

    private void startDaemon(int mode){
        new Thread(() -> {
            String cmd = getFilesDir() + "/kdaemon " + mode;
            if(Shell.rootAccess()){
                Shell.su(cmd).submit();
            } else {
                Shell.sh(cmd).submit();
            }
        }).start();
    }

//    private int getProcessID(String pkg) {
//        int pid = -1;
//        if (Shell.rootAccess()) {
//            String cmd = "for p in /proc/[0-9]*; do [[ $(<$p/cmdline) = " + pkg + " ]] && echo ${p##*/}; done";
//            List<String> outs = new ArrayList<>();
//            Shell.su(cmd).to(outs).exec();
//            if (outs.size() > 0) {
//                pid = Integer.parseInt(outs.get(0));
//            }
//        } else {
//            Shell.Result out = Shell.sh("/system/bin/ps -A | grep \"" + pkg + "\"").exec();
//            List<String> output = out.getOut();
//            Log.d("PTEST", "TEST: " + Arrays.toString(output.toArray()));
//            if (output.isEmpty() || output.get(0).contains("bad pid")) {
//                out = Shell.sh("/system/bin/ps | grep \"" + pkg + "\"").exec();
//                output = out.getOut();
//                if (!output.isEmpty() && !output.get(0).contains("bad pid")) {
//                    for (int i = 0; i < output.size(); i++) {
//                        String[] results = output.get(i).trim().replaceAll("( )+", ",").replaceAll("(\n)+", ",").split(",");
//                        if (results[8].equals(pkg)) {
//                            pid = Integer.parseInt(results[1]);
//                        }
//                    }
//                }
//            } else {
//                for (int i = 0; i < output.size(); i++) {
//                    String[] results = output.get(i).trim().replaceAll("( )+", ",").replaceAll("(\n)+", ",").split(",");
//                    for (int j = 0; j < results.length; j++) {
//                        if (results[j].equals(pkg)) {
//                            pid = Integer.parseInt(results[j - 7]);
//                        }
//                    }
//                }
//            }
//        }
//        return pid;
//    }

    //Native Funcs
    public static native int Init();

    public static native void DrawOn(ESPView espView, Canvas canvas);

    public static native void Switch(int num, boolean flag);

    public static native void Size(int num, float size);

    public static native void Stop();

    //UI Elements
    private void addSpace(int space) {
        View separator = new View(this);
        LinearLayout.LayoutParams params = setParams();
        params.height = space;
        separator.setLayoutParams(params);
        separator.setBackgroundColor(Color.TRANSPARENT);
        patches.addView(separator);
    }

    private void addSwitch(String name, CompoundButton.OnCheckedChangeListener listener) {
        final Switch sw = new Switch(this);
        sw.setText(name);
        sw.setTextSize(12);
        sw.setTextColor(Color.BLACK);
        sw.getThumbDrawable().setColorFilter(Color.WHITE, PorterDuff.Mode.MULTIPLY);
        sw.setOnClickListener(view -> {
            if (sw.isChecked()) {
                sw.getThumbDrawable().setColorFilter(0xFF1DE9B6, PorterDuff.Mode.MULTIPLY);
            } else {
                sw.getThumbDrawable().setColorFilter(Color.WHITE, PorterDuff.Mode.MULTIPLY);
            }
        });
        sw.setOnCheckedChangeListener(listener);
        sw.setLayoutParams(setParams());
        patches.addView(sw);
        addSpace(16);
    }

    private void addSeekbar(int max, int def, final SeekBar.OnSeekBarChangeListener listener) {
        SeekBar sb = new SeekBar(this);
        sb.setMax(max);
        sb.setProgress(def);
        sb.setLayoutParams(setParams());
        sb.setOnSeekBarChangeListener(listener);
        patches.addView(sb);
        addSpace(12);
    }

    private TextView addText(String text) {
        TextView tv = new TextView(this);
        tv.setText(text);
        tv.setTextSize(16);
        tv.setTextColor(Color.BLUE);
        tv.setLayoutParams(setParams());
        patches.addView(tv);
        addSpace(12);
        return tv;
    }

    private LinearLayout.LayoutParams setParams() {
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.MATCH_PARENT);
        params.gravity = Gravity.CENTER_VERTICAL;
        return params;
    }

    private boolean isTablet() {
        DisplayMetrics metrics = getResources().getDisplayMetrics();
        float yInches = metrics.heightPixels / metrics.ydpi;
        float xInches = metrics.widthPixels / metrics.xdpi;
        double diagonalInches = Math.sqrt(xInches * xInches + yInches * yInches);
        return diagonalInches >= 6.5;
    }

    private float getBestTextSize() {
        DisplayMetrics metrics = getResources().getDisplayMetrics();
        float d = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 8f, metrics);
        if (isTablet())
            d += 7.f;
        return (d > 20 && !isTablet()) ? 20 : d;
    }

    private float dipToPixels() {
        DisplayMetrics metrics = getResources().getDisplayMetrics();
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 8f, metrics);
    }

    private int getLayoutType() {
        int LAYOUT_FLAG;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_PHONE;
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_TOAST;
        } else {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
        }
        return LAYOUT_FLAG;
    }

    private int getResID(String name, String type) {
        return getResources().getIdentifier(name, type, getPackageName());
    }

    private int getID(String name) {
        return getResID(name, "id");
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }


    CompoundButton.OnCheckedChangeListener ipjListener = (compoundButton, b) -> {
        if(b){
            ipj("true");
        } else {
            ipj("false");
        }
    };

    public void ipj(String state){


        String pkg = "com.tencent.ig";
        int uid = 0;
        try {
            uid = getApplicationContext().getPackageManager().getApplicationInfo(pkg, 0).uid;
            Toast.makeText(getApplicationContext(),"UID : "+uid,Toast.LENGTH_SHORT).show();
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        String[] commands_on = new String[]{

                "su -c iptables -I OUTPUT -m owner --uid-owner %s -p tcp -j DROP".replace("%s",""+uid),

        };

        String[] commands_off = new String[]{

                "su -c iptables -D OUTPUT -m owner --uid-owner %s -p tcp -j DROP".replace("%s",""+uid),


        };

        try {
            Process process = Runtime.getRuntime().exec("su");
            DataOutputStream os = new DataOutputStream(process.getOutputStream());



            if(state.equals("true")){
                for(String cmd : commands_on){
                    os.writeBytes(cmd+"\n"); os.flush();

                }
            } else if(state.equals("false")) {
                for(String cmd : commands_off){
                    os.writeBytes(cmd+"\n"); os.flush();

                }
            }


        } catch (IOException e) {
            e.printStackTrace();
        }



    }
}
