package com.example.monitoring_system.ui.theme;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.Button;
import android.os.Handler;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.appcompat.app.AppCompatActivity;

import android.net.wifi.WifiManager;
import android.net.wifi.WifiInfo;

import androidx.appcompat.app.AlertDialog;
import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;

import android.widget.Toast;
import android.Manifest;
import android.content.pm.PackageManager;
//import androidx.appcompat.app.AppCompatActivity;
import java.util.ArrayList;
import java.util.List;

import com.example.monitoring_system.R;

import android.content.SharedPreferences;

public class MainActivity extends AppCompatActivity {

    public static MainActivity instance;
    private RecyclerView recyclerView;
    private ChatAdapter chatAdapter;
    //    private List<ChatMessage> chatHistory = new ArrayList<>();
    private List<String> chatHistory = new ArrayList<>();
    private EditText inputMessage;
    private Button sendButton;
    private SocketClient socketClient;
    // 权限请求码
    private static final int REQUEST_PERMISSION = 100;
    private WifiManager wifiManager;
    private Handler uiHandler = new Handler();
    private SharedPreferences sharedPreferences;
    private final String TAG = "MainActivity..";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        instance = this;

        wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        recyclerView = findViewById(R.id.recyclerView);
        inputMessage = findViewById(R.id.inputMessage);
        sendButton = findViewById(R.id.sendButton);

        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        chatAdapter = new ChatAdapter(chatHistory);
        recyclerView.setAdapter(chatAdapter);

        sharedPreferences = getSharedPreferences("ChatHistory", MODE_PRIVATE);
        loadChatHistory();

        // 检查是否有足够的权限来访问 Wi-Fi 信息
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            // 如果没有权限，请求权限
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_PERMISSION);
        } else {
            // 如果有权限，则检测 Wi-Fi 连接状态
            checkWifiConnection();
        }


        sendButton.setOnClickListener(v -> {
            String message = inputMessage.getText().toString();

            if (!message.isEmpty()) {
//                ChatMessage chatMessage = new ChatMessage(message);
                socketClient.sendMessage(message);
                updateChat("1" + message);
                inputMessage.setText("");
            }
        });
    }

    private void loadChatHistory() {
        String history = sharedPreferences.getString("history", "");
        if (!history.isEmpty()) {
            String[] messages = history.split("\n");
            for (String message : messages) {
                chatHistory.add(message);
            }
            chatAdapter.notifyDataSetChanged();
        }
    }

    public void updateChat(String message) {
        Log.i(TAG, "updateChat: ");
        Log.i(TAG, "message: " + message);
        chatHistory.add(message);
        chatAdapter.notifyItemInserted(chatHistory.size() - 1);
        chatAdapter.scrollToBottom(findViewById(R.id.recyclerView));
        // 保存聊天历史
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString("history", String.join("\n", chatHistory));
        editor.apply();
    }

    @Override
    protected void onStart() {
        super.onStart();
        // 检查WiFi连接
        checkWifiConnection();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (socketClient != null) {
            socketClient.closeConnection();
        }
    }

    // 权限请求结果处理
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // 权限被授予，继续检查 Wi-Fi 连接
                checkWifiConnection();
            } else {
                // 权限被拒绝，提示用户
                Toast.makeText(this, "需要位置权限才能检查 Wi-Fi 连接状态", Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void checkWifiConnection() {
        // 获取当前 Wi-Fi 连接的信息
        WifiInfo currentConnection = wifiManager.getConnectionInfo();

        // 判断是否已连接 Wi-Fi（SSID 不为空且不为默认值）
        if (currentConnection == null || currentConnection.getSSID().equals("<unknown ssid>")) {
            // 如果没有连接 Wi-Fi，则显示提示弹窗
            showWifiDialog();
        } else {

            if (socketClient == null) {
                socketClient = new SocketClient(uiHandler);
                socketClient.connect();
            }
        }
    }

    private void showWifiDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Wi-Fi 未连接")
                .setMessage("当前没有连接到 Wi-Fi 网络(已连接且同意位置权限点击取消)。")
                .setCancelable(false)
                .setPositiveButton("设置 Wi-Fi", (dialog, which) -> {
                    // 跳转到 Wi-Fi 设置页面
                    Intent intent = new Intent(android.provider.Settings.ACTION_WIFI_SETTINGS);
                    startActivity(intent);
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .show();
    }
}
