package ishimi.katamari
import android.content.Context
import android.os.Bundle
import android.view.KeyEvent
import android.view.inputmethod.EditorInfo
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.button.MaterialButton
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.android.material.materialswitch.MaterialSwitch
import com.google.android.material.textfield.TextInputEditText
import java.io.File

class MainActivity : AppCompatActivity() {
    external fun testRoot(): Boolean;
    external fun isDaemonEnabled(): Boolean;
    external fun manageDaemon(enableDaemon: Boolean): Boolean;
    external fun addPackageIntoList(packageToAdd: String): Boolean;
    external fun removePackageFromList(packageToRemove: String): Boolean;
    external fun importPackageList(): Int;
    external fun exportPackageList(): Int;
    external fun doesModuleExists(): Boolean;
    //funcs
    fun Context.toast(resId: Int) =
        Toast.makeText(this, getString(resId), Toast.LENGTH_SHORT).show()

    fun Context.toast(text: String) =
        Toast.makeText(this, text, Toast.LENGTH_SHORT).show()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // let's just make the dir whatever.
        val dir = File(getExternalFilesDir(null), "mngrmlwk");
        if(!dir.exists()) dir.mkdirs();
        val inputFieldAdd = findViewById<TextInputEditText>(R.id.appToAdd);
        val inputFieldRemove = findViewById<TextInputEditText>(R.id.appToRemove);
        val daemonToggle = findViewById<MaterialSwitch>(R.id.enableDaemon);
        val importButton = findViewById<MaterialButton>(R.id.importPreviousState);
        val exportButton = findViewById<MaterialButton>(R.id.ExportState);
        // never disable this!
        daemonToggle.isEnabled = true; // it's alr enabled but i added this for debugging purposes!
        if(!isDaemonEnabled()) {
            inputFieldAdd.isEnabled = false;
            inputFieldRemove.isEnabled = false;
            daemonToggle.isChecked = false;
        }
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
        if(!doesModuleExists()) {
            MaterialAlertDialogBuilder(this)
                .setTitle("Katana")
                .setMessage(this.getString(R.string.modulenotfound))
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
                if(!input.isNullOrEmpty()) if(removePackageFromList(input)) true;
                false;
            }
            else false;
        }
        inputFieldRemove.setOnEditorActionListener { _, actionId, event ->
            if(actionId == EditorInfo.IME_ACTION_DONE || (event != null && event.keyCode == KeyEvent.KEYCODE_ENTER && event.action == KeyEvent.ACTION_DOWN)) {
                val input = inputFieldRemove.text?.toString()?.trim();
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
        importButton.setOnClickListener {
            toast(R.string.importpkglists);
            val messageRes = when (importPackageList()) {
                0 -> R.string.successfullyimportpkglists;
                1 -> R.string.failedtoimportpkglists;
                41 -> R.string.failedtoremovetempfile;
                66 -> R.string.failedtocc2tempfile;
                67 -> R.string.importfilenotfound;
                else -> R.string.failedtoimportpkglists;
            }
            toast(messageRes);
        }
        exportButton.setOnClickListener {
            toast(R.string.exportpkglists);
            val messageRes = when(exportPackageList()) {
                0 -> R.string.successfullyexportpkglists;
                else -> R.string.failedexportpkglists;
            }
            toast(messageRes);
        }
    }
    companion object {
        init {
            System.loadLibrary("katamari");
        }
    }
}