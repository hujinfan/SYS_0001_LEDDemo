package com.android.server;

import android.os.ILedService;

public class LedService extends ILedService.Stub{
    private static final String TAG = "LedService";

	/* call native c function to access hardware*/
    public int ledCtrl(int which, int status) throws android.os.RemoteException
	{
		return nateive_ledCtrl(which, status);
	}

	public LedService(){
		native_ledOpen();
	}

	public static native int native_ledOpen();
	public static native int native_ledClose();
	public static native int nateive_ledCtrl(int which, int status);
}

