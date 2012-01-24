package org.mozilla.gecko;

import java.util.ArrayList;
import java.util.List;
import java.util.Iterator;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import android.app.Activity;
import android.app.IntentService;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;


public class C2DMService extends IntentService {
    private static final String LOGTAG = "c2dm";

    public static final String REGISTER = "com.google.android.c2dm.intent.REGISTER";
    public static final String REGISTER_CALLBACK = "com.google.android.c2dm.intent.REGISTRATION";
    public static final String MESSAGE = "com.google.android.c2dm.intent.RECEIVE";

    // TODO: make this configurable.
    private static final String SENDER = "fennec.ubuntu@gmail.com";
    private static final String CHECK_REGISTRATION = "org.mozilla.gecko.c2dm.CHECK";
    private static final String PREF = "c2dm.registration_id";
    private static final String PUSH_SERVER = "http://push.jbalogh.me";

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

        if (intent.getAction().equals(REGISTER_CALLBACK)) {
            Log.e("jbalogh", "(registration)");
            onRegistration(intent);
        } else if (intent.getAction().equals(MESSAGE)) {
            Log.e("jbalogh", "(message)");
        }
    }

    private void onRegistration(Intent intent) {
        setRegistrationId(intent.getStringExtra("registration_id"));
    }

    public static void register(Context context) {
        Log.e(LOGTAG, "registering for c2dm");
        String registrationId = getRegistrationId();

        if (registrationId.equals("")) {
            Intent regIntent = new Intent(REGISTER);
            regIntent.putExtra("app", PendingIntent.getBroadcast(context, 0, new Intent(), 0));
            regIntent.putExtra("sender", SENDER);
            context.startService(regIntent);
            Log.e(LOGTAG, "sent REGISTER intent");
        } else {
            Log.e("jbalogh", "existing id: " + registrationId);
            setRegistrationId(registrationId);
        }
    }

    void showExtras(Intent intent) {
        Iterator<String> iterator = intent.getExtras().keySet().iterator();
        while (iterator.hasNext()) {
            String key = iterator.next();
            Log.e("jbalogh", "  " + key + ": " + intent.getStringExtra(key));
        }
    }

    private static String getRegistrationId() {
        SharedPreferences settings = GeckoApp.mAppContext.getPreferences(Activity.MODE_PRIVATE);
        return settings.getString(PREF, "");
    }

    private static void setRegistrationId(String registrationId) {
        SharedPreferences settings = GeckoApp.mAppContext.getPreferences(Activity.MODE_PRIVATE);
        settings.edit().putString(PREF, registrationId).apply();
        GeckoPreferences.setPreference(PREF, registrationId);
        registerWithServer(registrationId);
    }

    private static void registerWithServer(String registrationId) {
        Log.e("jbalogh", "registering with server");
        HttpClient client = new DefaultHttpClient();
        HttpPost post = new HttpPost(PUSH_SERVER);
        try {
            List<NameValuePair> body = new ArrayList<NameValuePair>(1);
            body.add(new BasicNameValuePair("registration_id", registrationId));
            post.setEntity(new UrlEncodedFormEntity(body));
            Log.e("jbalogh", "posting");
            HttpResponse response = client.execute(post);
            Log.e("jbalogh", response.toString());
        } catch(Exception ex) {
            Log.e("jbalogh", "http error", ex);
        }
    }
}
