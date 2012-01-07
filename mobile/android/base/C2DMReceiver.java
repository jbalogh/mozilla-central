package org.mozilla.gecko;

import java.util.Iterator;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;


public class C2DMReceiver extends BroadcastReceiver {

    public final void onReceive(Context context, Intent intent) {
        Log.e("jbalogh", "C2DM Broadcast:");
        Log.e("jbalogh", "package name: " + context.getPackageName());
        Log.e("jbalogh", "resource path: " + context.getPackageResourcePath());
        Log.e("jbalogh", "component: " + intent.getComponent().getClassName());
        Log.e("jbalogh", "action: " + intent.getAction());
        showExtras(intent);

        if (intent.getAction().equals("com.google.android.c2dm.intent.REGISTRATION")) {
            Log.e("jbalogh", "(registration)");
        } else if (intent.getAction().equals("com.google.android.c2dm.intent.RECEIVE")) {
            Log.e("jbalogh", "(message)");
        }

        setResult(Activity.RESULT_OK, null, null);
    }

    void showExtras(Intent intent) {
        Iterator<String> iterator = intent.getExtras().keySet().iterator();
        while (iterator.hasNext()) {
            String key = iterator.next();
            Log.e("jbalogh", "  " + key + ": " + intent.getStringExtra(key));
        }
    }
}
