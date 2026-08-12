package com.example.monitoring_system.ui.theme;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.provider.Settings;
import android.widget.Toast;

public class WiFiUtils {
    public static boolean isWiFiConnected(Context context) {
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        WifiInfo currentConnection = wifiManager.getConnectionInfo();
        return currentConnection != null && currentConnection.getNetworkId() != -1;
    }

    public static void promptUserToEnableWiFi(Context context) {
        Toast.makeText(context, "WiFi 未连接，请连接到 WiFi 后重试", Toast.LENGTH_LONG).show();
        Intent intent = new Intent(Settings.ACTION_WIFI_SETTINGS);
        context.startActivity(intent);
    }
}
