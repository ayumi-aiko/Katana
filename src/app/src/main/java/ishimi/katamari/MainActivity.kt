package ishimi.katamari
import android.os.Bundle
import android.view.KeyEvent
import android.view.inputmethod.EditorInfo
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.android.material.materialswitch.MaterialSwitch
import com.google.android.material.textfield.TextInputEditText

class MainActivity : AppCompatActivity() {
    external fun testRoot(): Boolean;
    external fun isDaemonEnabled(): Boolean;
    external fun manageDaemon(enableDaemon: Boolean): Boolean;
    external fun addPackageIntoList(packageToAdd: String): Boolean;
    external fun removePackageFromList(packageToRemove: String): Boolean;
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        val inputFieldAdd = findViewById<TextInputEditText>(R.id.appToAdd);
        val inputFieldRemove = findViewById<TextInputEditText>(R.id.appToRemove);
        val daemonToggle = findViewById<MaterialSwitch>(R.id.enableDaemon);
        // never disable this!
        daemonToggle.isEnabled = true; // it's alr enabled but i added this for debugging purposes!
        if(!isDaemonEnabled()) daemonToggle.isChecked = false;
        else daemonToggle.isChecked = true;
        if(!testRoot()) {
            MaterialAlertDialogBuilder(this)
                .setTitle("Katana")
                .setMessage(this.getString(R.string.failedtogetroot))
                .setCancelable(true)
                .setPositiveButton(this.getString(R.string.closetheapp)) { _, which: Int ->
                    finishAffinity();
                }
                .setOnCancelListener {
                    finishAffinity();
                }
                .show();
        }
        inputFieldAdd.setOnEditorActionListener { _, actionId, event ->
            if(actionId == EditorInfo.IME_ACTION_DONE || (event != null && event.keyCode == KeyEvent.KEYCODE_ENTER && event.action == KeyEvent.ACTION_DOWN)) {
                val input = inputFieldAdd.text?.toString()?.trim();
                //if(!input.isNullOrEmpty()) Toast.makeText(this, input, Toast.LENGTH_SHORT).show(); DEBUG CODE!
                if(!input.isNullOrEmpty()) if(removePackageFromList(input)) true;
                false;
            }
            else false;
        }
        inputFieldRemove.setOnEditorActionListener { _, actionId, event ->
            if(actionId == EditorInfo.IME_ACTION_DONE || (event != null && event.keyCode == KeyEvent.KEYCODE_ENTER && event.action == KeyEvent.ACTION_DOWN)) {
                val input = inputFieldRemove.text?.toString()?.trim();
                //if(!input.isNullOrEmpty()) Toast.makeText(this, input, Toast.LENGTH_SHORT).show(); DEBUG CODE!
                if(!input.isNullOrEmpty()) if(addPackageIntoList(input)) true;
                false;
            }
            else false;
        }
        daemonToggle.setOnCheckedChangeListener { buttonView, isChecked ->
            if(isChecked) {
                if(manageDaemon(true)) {
                    buttonView.isChecked = true;
                    Toast.makeText(this, this.getString(R.string.successstartingkatamari), Toast.LENGTH_SHORT).show();
                }
                else {
                    buttonView.isChecked = false;
                    Toast.makeText(this, this.getString(R.string.failureinstartingkatamari), Toast.LENGTH_SHORT).show();
                }
            }
            else {
                if(manageDaemon(false)) {
                    buttonView.isChecked = false;
                    Toast.makeText(this, this.getString(R.string.stoppedkatamarisuccessfully), Toast.LENGTH_SHORT).show();
                }
                else {
                    buttonView.isChecked = true;
                    Toast.makeText(this, this.getString(R.string.failedtostopkatamariservice), Toast.LENGTH_SHORT).show();
                }
            }
        }
    }
    companion object {
        init {
            System.loadLibrary("katamari");
        }
    }
}