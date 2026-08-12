package com.example.monitoring_system.ui.theme;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

public class SocketClient {
    private Socket socket;
    private BufferedReader inputStreamReader;
    private OutputStream outputStream;
    private Handler uiHandler;
    private String serverAddress = "192.168.4.1";  // 这里替换为 8266 网关的 IP 地址
    private int port = 8266;
    private String TAG = "SOCKET..";

    public SocketClient(Handler uiHandler) {
        this.uiHandler = uiHandler;
    }

    public void connect() {
        new Thread(() -> {
            try {
                socket = new Socket(serverAddress, port);
                inputStreamReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                outputStream = socket.getOutputStream();

                // 开始监听服务器消息
                listenForMessages();
            } catch (UnknownHostException e) {
                e.printStackTrace();

                // 获取 MainActivity 的 Context 并显示 Toast
                uiHandler.post(() -> {
                    if (MainActivity.instance != null) {
                        Toast.makeText(MainActivity.instance.getApplicationContext(), "无法连接到服务器，请检查网络", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (IOException e) {
                e.printStackTrace();

                // 获取 MainActivity 的 Context 并显示 Toast
                uiHandler.post(() -> {
                    if (MainActivity.instance != null) {
                        Toast.makeText(MainActivity.instance.getApplicationContext(), "连接失败，请重试", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }).start();
    }

    private void listenForMessages() {
        new Thread(() -> {
            try {

                String receivedMessage;
                while ((receivedMessage = inputStreamReader.readLine()) != null) {
                    // 确保将 instance 赋值为最终变量
                    final String finalReceivedMessage = receivedMessage;
                    Log.i(TAG, "listenForMessages: " + receivedMessage);


                    // 将收到的消息传递到UI线程
                    uiHandler.post(() -> {
                        // 使用 final 局部变量 mainActivityInstance 来更新聊天界面
                        if (MainActivity.instance != null) {
                            // 0代表接收，消息格式： 0/1 + message
                            MainActivity.instance.updateChat("0" + finalReceivedMessage);
                        }
                    });
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    public void sendMessage(String message) {
        new Thread(() -> {
            try {
//                ChatMessage chatMessage = new ChatMessage(message, isSentByCurrentUser);
//                chatHistory.add(chatMessage);
//                chatAdapter.notifyDataSetChanged();
                if (outputStream != null) {
                    outputStream.write((message + "\n").getBytes());
                    outputStream.flush();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    public void closeConnection() {
        try {
            if (socket != null && !socket.isClosed()) {
                socket.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
