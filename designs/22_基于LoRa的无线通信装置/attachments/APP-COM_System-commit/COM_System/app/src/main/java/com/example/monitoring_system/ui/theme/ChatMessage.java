package com.example.monitoring_system.ui.theme;

public class ChatMessage {
    private String message;
    private boolean isSentByCurrentUser;  // 是否是当前用户发送的消息

    // 构造方法
    public ChatMessage(String message, boolean isSentByCurrentUser) {
        this.message = message;
        this.isSentByCurrentUser = isSentByCurrentUser;
    }

    public String getMessage() {
        return message;
    }

    public boolean isSentByCurrentUser() {
        return isSentByCurrentUser;
    }
}