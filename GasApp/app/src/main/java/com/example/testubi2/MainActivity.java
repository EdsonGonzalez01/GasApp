package com.example.testubi2;

import android.app.Activity;
import android.app.NotificationManager;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.ubidots.ApiClient;
import com.ubidots.Value;
import com.ubidots.Variable;

public class MainActivity extends Activity {

    private TextView Gas;
    private TextView GasText;
    private ImageView tempImg;
    private Boolean active;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Gas = findViewById(R.id.gas);
        GasText = findViewById(R.id.gasText);
        tempImg = findViewById(R.id.tempImg);
        active = true;
        new ApiUbidots().execute();

    }


    @Override
    protected void onStop() {

        super.onStop();
        active = false;
    }

    @Override
    protected void onStart() {
        super.onStart();
        active = true;
        new ApiUbidots().execute();
    }


    public class ApiUbidots extends AsyncTask<Integer, ProgressUpdate, Void> {
        private final String API_KEY_UBIDOTS = "BBFF-fa11c4afe5b4eff24d42aeac44aeaa3476a";
        private final String GasVariable_ID = "64552bdf09878e000b2c8084";




        @Override
        protected Void doInBackground(Integer... params) {
            while (active) {
                ApiClient apiClient = new ApiClient(API_KEY_UBIDOTS); //API_KEY de Ubidots
                Variable gas = apiClient.getVariable(GasVariable_ID); //Obtener referencia a la variable de temperatura con su ID
                Value[] valoresGas = gas.getValues(); //Obtener valores cambiantes de temperatura

                Log.wtf("UBIDOTS", String.valueOf(valoresGas[0].getValue()));


                String valorActualGas = String.valueOf(valoresGas[0].getValue());
                String gasText;
                if (Double.parseDouble(valorActualGas) >= 2000) {
                    gasText = "Bien";
                } else if (Double.parseDouble(valorActualGas) > 1000 && Double.parseDouble(valorActualGas) < 2000) {
                    gasText = "Medio";
                }else{
                    gasText = "Bajo";
                }
            //obtener valor actual de temperatura
                publishProgress(new ProgressUpdate(valorActualGas, gasText)); //actualizar UI con valores actuales
            }
            return null;
        }

        @Override
        protected void onProgressUpdate(ProgressUpdate... values) {
            super.onProgressUpdate(values);

            System.out.println(values[0].gas);
            System.out.println(values[0].gasText);

            Gas.setText(values[0].gas);
            GasText.setText(values[0].gasText);
        }

        @Override
        protected void onPostExecute(Void unused) {
            super.onPostExecute(unused);
            new ApiUbidots().execute();
        }
    }

    class ProgressUpdate {
        public final String gas;
        public final String gasText;

        public ProgressUpdate(String gas, String gasText) {
            this.gas = gas;
            this.gasText = gasText;

            if (Double.parseDouble(this.gas) >= 2000 && this.gas != null) {
                tempImg.setColorFilter(Color.GREEN);

            } else if (Double.parseDouble(this.gas) > 1000 && Double.parseDouble(this.gas) < 2000 && this.gas != null) {
                    tempImg.setColorFilter(Color.YELLOW);
                }else{
                    NotificationCompat.Builder builder = new NotificationCompat.Builder(MainActivity.this, "My notification");
                    builder.setContentTitle("Notificacion de GasApp");
                    builder.setContentText("Tu nivel de gas esta bajo, te recomendamos que surtas ahora.");
                    builder.setSmallIcon(R.drawable.ic_launcher_background);
                    builder.setAutoCancel(true);

                    NotificationManagerCompat managerCompat = NotificationManagerCompat.from(MainActivity.this);
                    managerCompat.notify(1, builder.build());
                    tempImg.setColorFilter(Color.RED);
                }
            }
        }
}