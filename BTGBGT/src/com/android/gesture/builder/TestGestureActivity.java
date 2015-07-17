package com.android.gesture.builder;

import java.util.ArrayList;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.gesture.Gesture;
import android.gesture.GestureLibrary;
import android.gesture.GestureOverlayView;
import android.gesture.Prediction;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.widget.TextView;
import android.widget.Toast;


public class TestGestureActivity extends Activity {

//BLUETOOTH

    // Message types sent from the BluetoothChatService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    // Key names received from the BluetoothChatService Handler
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";
    
    
    // Layout Views
    private TextView mTitle;
    
    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    
    // Name of the connected device
    private String mConnectedDeviceName = null;
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the chat services
    private BluetoothSerialService mSerialService = null;
    
    //END BLUETOOTH
    
		@Override
		protected void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);

			requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);   
			setContentView(R.layout.test_gesture);
			getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE, R.layout.custom_title);
			
			final TextView text = (TextView) findViewById(R.id.text);
			final GestureOverlayView overlay = (GestureOverlayView) findViewById(R.id.gestures_overlay);
			
			//BLUETOOTH
			
			 // Set up the custom title
	        mTitle = (TextView) findViewById(R.id.title_left_text);
	        mTitle.setText(R.string.application_name);
	        mTitle = (TextView) findViewById(R.id.title_right_text);
	        
	        // Get local Bluetooth adapter
	        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

	        // If the adapter is null, then Bluetooth is not supported
	        if (mBluetoothAdapter == null) {
	            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
	            finish();
	            return;
	        }
	        
	        //END BLUETOOTH

			overlay.addOnGestureListener(new GestureOverlayView.OnGestureListener() {
				
				@Override
				public void onGestureStarted(GestureOverlayView overlay, MotionEvent event) {
				}
				
				@Override
				public void onGestureEnded(GestureOverlayView overlay, MotionEvent event) {
					final Gesture gesture = overlay.getGesture();
					final GestureLibrary store = GestureBuilderActivity.getStore();
					ArrayList<Prediction> predictions = store.recognize(gesture);
					if (predictions.size() > 0) {
						Prediction prediction = predictions.get(0);
						if (prediction.score > 5.0) {
							String message = prediction.name.toString();
			                sendMessage(message);
							text.append(prediction.name);
						}
					}
				}
				
				@Override
				public void onGestureCancelled(GestureOverlayView overlay, MotionEvent event) {
				}
				
				@Override
				public void onGesture(GestureOverlayView overlay, MotionEvent event) {
				}
			});
		}

		public void done(View v) {
			setResult(RESULT_OK);
			finish();
		}
		
		 //BLUETOOTH
		
		private void sendMessage(String message) {
	        // Check that we're actually connected before trying anything
	        if (mSerialService.getState() != BluetoothSerialService.STATE_CONNECTED) {
	            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
	            return;
	        }

	        // Check that there's actually something to send
	        if (message.length() > 0) {
	            // Get the message bytes and tell the BluetoothChatService to write
	            byte[] send = message.getBytes();
	            mSerialService.write(send);
	        }
	    }
		
	    @Override
		protected void onStart() {
			// TODO Auto-generated method stub
			super.onStart();
				if (!mBluetoothAdapter.isEnabled()) {
		            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
		        // Otherwise, setup the chat session
		        } else {
		            if (mSerialService == null) setupChat();
		        }
		}
	    
	    private void setupChat() {
			// TODO Auto-generated method stub
	    	 // Initialize the BluetoothChatService to perform bluetooth connections
	    	mSerialService = new BluetoothSerialService(this, mHandler);

		}
	    
	    @Override
	    public synchronized void onResume() {
	        super.onResume();

	        // Performing this check in onResume() covers the case in which BT was
	        // not enabled during onStart(), so we were paused to enable it...
	        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
	        if (mSerialService != null) {
	            // Only if the state is STATE_NONE, do we know that we haven't started already
	            if (mSerialService.getState() == BluetoothSerialService.STATE_NONE) {
	              // Start the Bluetooth chat services
	            	mSerialService.start();
	            }
	        }
	    }
	    
	    @Override
	    protected void onDestroy() {
	        super.onDestroy();
	   
	        //BLUETOOTH
	        if (mSerialService != null) mSerialService.stop();
	        //END BLUETOOTH
	    }

	    //END BLUETOOTH
	    
	    
	    @Override
	    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
	        super.onActivityResult(requestCode, resultCode, data);
	        
	        //BLUETOOTH

	        switch (requestCode) {
	        case REQUEST_CONNECT_DEVICE:
	            // When DeviceListActivity returns with a device to connect
	            if (resultCode == Activity.RESULT_OK) {
	                // Get the device MAC address
	                String address = data.getExtras()
	                                     .getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
	                // Get the BLuetoothDevice object
	                BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
	                // Attempt to connect to the device
	                mSerialService.connect(device);
	            }
	            break;
	        case REQUEST_ENABLE_BT:
	            // When the request to enable Bluetooth returns
	            if (resultCode == Activity.RESULT_OK) {
	                // Bluetooth is now enabled, so set up a chat session
	                setupChat();
	            } else {
	                // User did not enable Bluetooth or an error occured
	                Toast.makeText(this, "bt_not_enabled_leaving", Toast.LENGTH_SHORT).show();
	                //finish();
	            }
	        }
	    
	        //END BLUETOOTH
	    }
	    
	    //BLUETOOTH

	    // The Handler that gets information back from the BluetoothChatService
		private final Handler mHandler = new Handler() {
	        @Override
	        public void handleMessage(Message msg) {
	            switch (msg.what) {
	            case MESSAGE_STATE_CHANGE:
	                switch (msg.arg1) {
	                case BluetoothSerialService.STATE_CONNECTED:
	                    mTitle.setText(R.string.title_connected_to);
	                    mTitle.append(mConnectedDeviceName);
	                    /*mConversationArrayAdapter.clear();*/
	                    break;
	                case BluetoothSerialService.STATE_CONNECTING:
	                    mTitle.setText(R.string.title_connecting);
	                    break;
	                case BluetoothSerialService.STATE_LISTEN:
	                case BluetoothSerialService.STATE_NONE:
	                    mTitle.setText(R.string.title_not_connected);
	                    break;
	                }
	                break;
	            case MESSAGE_WRITE:
	                byte[] writeBuf = (byte[]) msg.obj;
	                // construct a string from the buffer
					String writeMessage = new String(writeBuf);
	               /* mConversationArrayAdapter.add("Me:  " + writeMessage);*/
	                break;
	            /*case MESSAGE_READ:
	                byte[] readBuf = (byte[]) msg.obj;
	                // construct a string from the valid bytes in the buffer
	                String readMessage = new String(readBuf, 0, msg.arg1);
	                mConversationArrayAdapter.add(mConnectedDeviceName+":  " + readMessage);
	                break;*/
	            case MESSAGE_DEVICE_NAME:
	                // save the connected device's name
	                mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
	                Toast.makeText(getApplicationContext(), "Connected to "
	                               + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
	                break;
	            case MESSAGE_TOAST:
	                Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
	                               Toast.LENGTH_SHORT).show();
	                break;
	            }
	        }
	    };
	    
	    private void ensureDiscoverable() {
	        if (mBluetoothAdapter.getScanMode() !=
	            BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
	            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
	            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
	            startActivity(discoverableIntent);
	        }
	    }
	    
		@Override
		public boolean onCreateOptionsMenu(Menu menu) {
			// TODO Auto-generated method stub
			MenuInflater inflater = getMenuInflater();
			inflater.inflate(R.menu.option, menu);
			return true;
		}

		@Override
		public boolean onOptionsItemSelected(MenuItem item) {
			// TODO Auto-generated method stub
			switch(item.getItemId()){
				case R.id.scan:
					 Intent serverIntent = new Intent(this, DeviceListActivity.class);
			         startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
					return true;
				case R.id.discoverable:
					ensureDiscoverable();
					return true;
			}
			return false;
		}
		
		//END BLUETOOTH
		
}
