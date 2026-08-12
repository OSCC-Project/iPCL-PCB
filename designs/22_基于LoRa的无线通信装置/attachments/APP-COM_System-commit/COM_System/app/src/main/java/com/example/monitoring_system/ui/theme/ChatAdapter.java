package com.example.monitoring_system.ui.theme;


import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.view.Gravity;

import androidx.recyclerview.widget.RecyclerView;

import com.example.monitoring_system.R;

import java.util.List;

import android.util.Log;


public class ChatAdapter extends RecyclerView.Adapter<ChatAdapter.ChatViewHolder> {

    private List<String> chatHistory;
    //    private RecyclerView recyclerView; // 添加 RecyclerView 引用
    private final String TAG = "ChatAdapter..";

    public ChatAdapter(List<String> chatHistory) {
        this.chatHistory = chatHistory;
    }

    @Override
    public ChatViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.chat_item, parent, false);
        return new ChatViewHolder(view);
    }

    @Override
    public void onBindViewHolder(ChatViewHolder holder, int position) {
        String message = chatHistory.get(position);
//        Log.i(TAG, "onBindViewHolder: message["+message);
        if (message.length() > 1) {
            holder.messageTextView.setText(message.substring(1));
        } else {
            message = "1"+message;
            holder.messageTextView.setText(message);
        }

        // 根据消息的发送者设置不同的气泡样式
        if (isCurrentUserMessage(message)) {
            // 设置当前用户的消息右对齐
            holder.messageTextView.setBackgroundResource(R.drawable.chat_bubble_right);
            holder.messageTextView.setLayoutParams(new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT
            ));
            ((LinearLayout.LayoutParams) holder.messageTextView.getLayoutParams()).gravity = Gravity.END;
        } else {
            // 设置接收方的消息左对齐
            holder.messageTextView.setBackgroundResource(R.drawable.chat_bubble_left);
            // 调整布局参数来让消息靠左显示
            holder.messageTextView.setLayoutParams(new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT
            ));
            ((LinearLayout.LayoutParams) holder.messageTextView.getLayoutParams()).gravity = Gravity.START;
        }
    }

    @Override
    public int getItemCount() {
        return chatHistory.size();
    }

    // 判断当前消息是否是当前用户的消息
    private boolean isCurrentUserMessage(String message) {
        Log.i(TAG, "isCurrentUserMessage: " + message.substring(0, 1));
        // 判断规则，根据实际情况来修改
        return message.substring(0, 1).equals("1");
    }

    public static class ChatViewHolder extends RecyclerView.ViewHolder {
        public TextView messageTextView;

        public ChatViewHolder(View itemView) {
            super(itemView);
            messageTextView = itemView.findViewById(R.id.messageTextView);
        }
    }

    public void scrollToBottom(RecyclerView recyclerView) {
        Log.i(TAG, "scrollToBottom: ");
        // 确保 RecyclerView 滚动到最后一条消息
        if (recyclerView != null) {
//            Log.i(TAG, "recyclerView: != NULL");
            recyclerView.post(() -> {
//                int position = getItemCount() - 1;
                int position = getItemCount();
//                Log.i(TAG, "position: " + position);
                if (position >= 0) {
                    recyclerView.smoothScrollToPosition(position);
                }
            });
        }

    }
}
