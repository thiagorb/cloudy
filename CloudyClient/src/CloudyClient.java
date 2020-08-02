import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;

public class CloudyClient extends MIDlet {
    public static TextBox textInputScreen = new TextBox("", "", 100, TextField.ANY);
    public static TextBox logTextBox = new TextBox("", "", 10000, TextField.ANY);
    public static CloudyDisplay cloudyDisplay;
    public static Command cmdOk;
    public static Command cmdPause;
    public static Display display;
    public static boolean started = false;
    public static String logs = "";

    public void startApp() {
        if (!started) {
            started = true;
            display = Display.getDisplay(this);
            cmdOk = new Command("Ok", Command.SCREEN, 0);
            textInputScreen.addCommand(cmdOk);
            textInputScreen.setCommandListener(new CommandListener() {
                public void commandAction(Command c, Displayable d) {
                    if (c == cmdOk) {
                        cloudyDisplay.after_text_input(textInputScreen.getString());
                        display.setCurrent(cloudyDisplay);
                    }
                }
            });
            logTextBox.addCommand(cmdOk);
            logTextBox.setCommandListener(new CommandListener() {
                public void commandAction(Command c, Displayable d) {
                    if (c == cmdOk) {
                        display.setCurrent(cloudyDisplay);
                    }
                }
            });
            cloudyDisplay = new CloudyDisplay(this);
            display.setCurrent(cloudyDisplay);
        }
    }
    
    public void pauseApp() {
        logTextBox.setChars(logs.toCharArray(), 0, logs.length());
        CloudyClient.display.setCurrent(logTextBox);
    }
    
    public void destroyApp(boolean unconditional) {
    }
    
    public void textInput() {
        textInputScreen.setChars("".toCharArray(), 0, 0);
        display.setCurrent(textInputScreen);
    }
}
