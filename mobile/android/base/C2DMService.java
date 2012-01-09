package org.mozilla.gecko;

import java.util.Iterator;

import android.app.IntentService;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.util.Log;


public class C2DMService extends IntentService {
    private static final String LOGTAG = "c2dm";

    public static final String REGISTER = "com.google.android.c2dm.intent.REGISTER";
    public static final String REGISTER_CALLBACK = "com.google.android.c2dm.intent.REGISTRATION";
    public static final String MESSAGE = "com.google.android.c2dm.intent.RECEIVE";

    // TODO: make this configurable.
    private static final String SENDER = "fennec.ubuntu@gmail.com";

    public C2DMService() {
        super("C2DMService");
    }

    public void onHandleIntent(Intent intent) {
        Context context = getApplicationContext();
        Log.e("jbalogh", "handle intent");
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
    }

    public static void register(Context context) {
        Log.e(LOGTAG, "registering for c2dm");
        Intent regIntent = new Intent(REGISTER);
        regIntent.putExtra("app", PendingIntent.getBroadcast(context, 0, new Intent(), 0));
        regIntent.putExtra("sender", SENDER);
        context.startService(regIntent);
        Log.e(LOGTAG, "sent REGISTER intent");
    }

    void showExtras(Intent intent) {
        Iterator<String> iterator = intent.getExtras().keySet().iterator();
        while (iterator.hasNext()) {
            String key = iterator.next();
            Log.e("jbalogh", "  " + key + ": " + intent.getStringExtra(key));
        }
    }
}
