package com.itas109.cserialport;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.os.Bundle;

import com.itas109.cserialport.jni.*;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import java.util.ArrayList;
import java.util.List;

import android.widget.ArrayAdapter;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    CSerialPort serialPort = new CSerialPort();

    MyListener listener = new MyListener(serialPort);
    MyHotPlugListener hotPlugListener = new MyHotPlugListener();

    SerialPortInfoVector spInfoVec = new SerialPortInfoVector();

    private Spinner spinnerPortName;
    private ArrayAdapter<String> adapterPortName;
    private List<String> dataListPortName;
    private EditText editTextReceive;

    static {
        System.loadLibrary("cserialport");
    }

    class MyListener extends CSerialPortListener {
        public MyListener(CSerialPort sp) {
            super();

            countRead = 0;
            m_sp = sp;
        }

        public void onReadEvent(String portName, long readBufferLen) {
            byte[] data = new byte[(int) readBufferLen];
            int recLen = m_sp.readAllData(data);
            String str = new String(data);

            String info = String.format("%s - Count: %d, Length: %d, Str: %s", portName, ++countRead, recLen, str);
            Log.i("onReadEvent", info);

            // fixed Swig::DirectorException: Can't create handler inside thread that has not called Looper.prepare()
            new Handler(Looper.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                    // Toast.makeText(MainActivity.this, info, Toast.LENGTH_SHORT).show();
                    editTextReceive.append(str);
                }
            });

        }

        private CSerialPort m_sp;
        private int countRead;
    }

    class MyHotPlugListener extends CSerialPortHotPlugListener {
        public MyHotPlugListener() {
            super();
        }

        public void onHotPlugEvent(String portName, int isAdd) {
            String info = String.format("portName: %s, isAdded: %d", portName, isAdd);
            // Toast.makeText(MainActivity.this, info, Toast.LENGTH_SHORT).show();
            Log.i("onHotPlugEvent", info);
        }
    }

    private void getAvaiablePorts() {
        spInfoVec = CSerialPortInfo.availablePortInfos();

        // dataList
        dataListPortName = new ArrayList<>();
        for (int i = 0; i < spInfoVec.size(); ++i) {
            SerialPortInfo serialPortInfo = spInfoVec.get(i);
            dataListPortName.add(serialPortInfo.getPortName());
            Log.i("getAvaiablePorts", serialPortInfo.getPortName());
        }
        // Adapter
        adapterPortName = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, dataListPortName);
        adapterPortName.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Spinner
        spinnerPortName = findViewById(R.id.spinnerPortName);
        spinnerPortName.setAdapter(adapterPortName);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        editTextReceive = findViewById(R.id.editTextReceive);

        // connect for read
        serialPort.connectReadEvent(listener);
        // connect for hot plug
        serialPort.connectHotPlugEvent(hotPlugListener);

        getAvaiablePorts();

        Button buttonRefreshPort = findViewById(R.id.buttonRefreshPort);
        buttonRefreshPort.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getAvaiablePorts();
            }
        });

        Button buttonOpenPort = findViewById(R.id.buttonOpenPort);
        buttonOpenPort.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(0 == spinnerPortName.getCount()){
                    Toast.makeText(MainActivity.this, "No Avaiable Port", Toast.LENGTH_SHORT).show();
                    return;
                }
                String portName = spinnerPortName.getSelectedItem().toString();
                serialPort.init(portName, // windows:COM1 Linux:/dev/ttyS0
                        9600, // baudrate
                        Parity.ParityNone, // parity
                        DataBits.DataBits8, // data bit
                        StopBits.StopOne, // stop bit
                        FlowControl.FlowNone, // flow
                        4096 // read buffer size
                );
                serialPort.setReadIntervalTimeout(0); // read interval timeout
                serialPort.open();

                String info = String.format("Open %s %s", portName, serialPort.isOpen() ? "Success" : serialPort.getLastErrorMsg());
                Toast.makeText(MainActivity.this, info, Toast.LENGTH_SHORT).show();
            }
        });

        Button buttonClosePort = findViewById(R.id.buttonClosePort);
        buttonClosePort.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                serialPort.close();

                String info = String.format("Close %s %s", serialPort.getPortName(), serialPort.isOpen() ? "Failed" : "Success");
                Toast.makeText(MainActivity.this, info, Toast.LENGTH_SHORT).show();
            }
        });

        Button buttonSend = findViewById(R.id.buttonSend);
        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setTitle("send");

                final EditText input = new EditText(MainActivity.this);
                input.setText("https://github.com/itas109/CSerialPort");
                builder.setView(input);

                builder.setPositiveButton("send", (dialog, which) -> {
                    String inputText = input.getText().toString();
                    if (!inputText.isEmpty()) {
                        byte[] data = inputText.getBytes();
                        int len = serialPort.writeData(data, data.length);
                        if (len > 0) {
                            Toast.makeText(MainActivity.this, "writeData " + len, Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(MainActivity.this, "writeData failed. " + serialPort.getLastErrorMsg(), Toast.LENGTH_SHORT).show();
                        }
                    } else {
                        Toast.makeText(MainActivity.this, "send data is empty", Toast.LENGTH_SHORT).show();
                    }
                });

                builder.setNegativeButton("cancel", (dialog, which) -> dialog.cancel());

                builder.show();
            }
        });
    }
}