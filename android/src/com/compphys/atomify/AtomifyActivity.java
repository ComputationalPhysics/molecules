package com.compphys.atomify;

import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

public class AtomifyActivity extends QtActivity
{
   @Override
   public void onCreate(Bundle savedInstanceState)
   {
        super.onCreate(savedInstanceState);
        System.out.println("Setting FLAG_KEEP_SCREEN_ON in activity");
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
}
