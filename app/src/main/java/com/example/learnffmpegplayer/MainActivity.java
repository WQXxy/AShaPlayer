package com.example.learnffmpegplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Toast;

import com.example.learnffmpegplayer.Bean.VideoBean;
import com.example.learnffmpegplayer.constants.ConstantsData;
import com.example.learnffmpegplayer.databinding.ActivityMainBinding;
import com.example.learnffmpegplayer.util.CommonUtils;

import java.util.ArrayList;

import static com.example.learnffmpegplayer.FFMediaPlayer.MEDIA_PARAM_VIDEO_DURATION;
import static com.example.learnffmpegplayer.FFMediaPlayer.MSG_DECODER_DONE;
import static com.example.learnffmpegplayer.FFMediaPlayer.MSG_DECODER_INIT_ERROR;
import static com.example.learnffmpegplayer.FFMediaPlayer.MSG_DECODER_READY;
import static com.example.learnffmpegplayer.FFMediaPlayer.MSG_DECODING_TIME;
import static com.example.learnffmpegplayer.FFMediaPlayer.MSG_REQUEST_RENDER;
import static com.example.learnffmpegplayer.FFMediaPlayer.VIDEO_RENDER_ANWINDOW;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback,FFMediaPlayer.EventCallback{

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
    };

    private static final int PERMISSION_REQUEST_CODE = 1;

    private ActivityMainBinding binding;

    private SurfaceView mSurfaceView;
    private Button mPlayButton;
    private Button mLastButton;
    private Button mNextButton;
    private SeekBar mSeekBar;
    private FFMediaPlayer player;
    private Boolean mIsPlaying =false;
    private boolean mIsTouching = false;
    Surface mSurface;

    private VideoBean mCurrentVideoBean;
    private ArrayList<VideoBean> mVideoList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        requestMyPermissions();
        initPath();
        initView();
        initListener();
        mSurfaceView.getHolder().addCallback(this);

    }

    private void initListener() {
        mPlayButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!mIsPlaying)
                {
                    player.play();
                    mIsPlaying =true;
                }
                else{
                    player.pause();
                    mIsPlaying =false;
                }
            }
        });
        mLastButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int index= mCurrentVideoBean.getIndex()-1;
                if(index<0){
                    mCurrentVideoBean=mVideoList.get(mVideoList.size()-1);
                }else{
                    mCurrentVideoBean=mVideoList.get(index);
                }
                if (player!=null){
                    player.unInit();
                    player.init(mCurrentVideoBean.getVideoPath(),VIDEO_RENDER_ANWINDOW,mSurface);
                    player.play();
                }
            }
        });
        mNextButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int index= mCurrentVideoBean.getIndex()+1;
                if(index>=mVideoList.size()){
                    mCurrentVideoBean=mVideoList.get(0);
                }else{
                    mCurrentVideoBean=mVideoList.get(index);
                }
                if (player!=null){
                    player.unInit();
                    player.init(mCurrentVideoBean.getVideoPath(),VIDEO_RENDER_ANWINDOW,mSurface);
                    player.play();
                }
            }
        });
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mIsTouching=true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                if (player != null) {
                    player.seekToPosition(mSeekBar.getProgress());
                    mIsTouching=false;
                }
            }
        });
    }

    private void initView() {
        mSurfaceView=findViewById(R.id.surface_view);
        mPlayButton =findViewById(R.id.play_btn);
        mLastButton=findViewById(R.id.last);
        mNextButton=findViewById(R.id.next);
        mSeekBar=findViewById(R.id.seekbar);
    }

    private void initPath(){
        CommonUtils.copyAssetsDirToSDCard(this,"byteflow","/sdcard");
        mVideoList=new ArrayList<>();
        for(int i=0;i< ConstantsData.pathData.length;i++){
            VideoBean temp=new VideoBean();
            temp.setIndex(i);
            temp.setVideoPath(Environment.getExternalStorageDirectory().getAbsolutePath()+
                    ConstantsData.pathData[i]);
            mVideoList.add(temp);
        }
        mCurrentVideoBean=mVideoList.get(0);
    }


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        player=new FFMediaPlayer();
        mSurface=surfaceHolder.getSurface();
        player.addEventCallback(this);
        player.init(mCurrentVideoBean.getVideoPath(),VIDEO_RENDER_ANWINDOW,mSurface);
        player.setLooping(true);
        mPlayButton.setVisibility(Button.VISIBLE);
        mLastButton.setVisibility(Button.VISIBLE);
        mNextButton.setVisibility(Button.VISIBLE);
        mSeekBar.setVisibility(SeekBar.VISIBLE);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        player.unInit();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (player != null) {
            player.pause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
        }
        if (player != null) {
            player.play();
        }
    }

    @Override
    public void onPlayerEvent(int msgType, float msgValue) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch(msgType){
                    case MSG_DECODER_INIT_ERROR:
                        break;
                    case MSG_DECODER_READY:
                        onDecoderReady();
                        break;
                    case MSG_DECODER_DONE:
                        break;
                    case MSG_REQUEST_RENDER:
                        break;
                    case MSG_DECODING_TIME:
                        if(!mIsTouching)
                            mSeekBar.setProgress((int) msgValue);
                        break;
                    default:
                        break;
                }
            }
        });
    }

    private void onDecoderReady() {
        int duration = (int) player.getMediaParams(MEDIA_PARAM_VIDEO_DURATION);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            mSeekBar.setMin(0);
        }
        mSeekBar.setMax(duration);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(this, "We need the permission: WRITE_EXTERNAL_STORAGE", Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void requestMyPermissions() {

        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 100);
        } else {
            Log.d("TAG", "requestMyPermissions: 有写SD权限");
        }
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 100);
        } else {
            Log.d("TAG", "requestMyPermissions: 有读SD权限");
        }
    }
    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }


}