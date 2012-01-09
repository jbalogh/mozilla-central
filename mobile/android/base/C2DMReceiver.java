package org.mozilla.gecko;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;


public class C2DMReceiver extends BroadcastReceiver {

    public final void onReceive(Context context, Intent intent) {
        Log.e("jbalogh", "C2DM Broadcast");
        intent.setClass(context, C2DMService.class);
        context.startService(intent);
        setResult(Activity.RESULT_OK, null, null);
    }
}
