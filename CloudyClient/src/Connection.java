import java.io.*;
import java.util.Calendar;
import java.util.Date;
import java.util.Timer;
import javax.microedition.io.Connector;
import javax.microedition.io.SocketConnection;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.TextField;

class command_id {
    // Messages that server send to client 
    public static final int SU_DRAW_IMAGE = 0;
    public static final int SU_CHANGE_COLOR = 1;
    public static final int SU_CHANGE_CLIP_RECT = 2;
    public static final int SU_DRAW_RECT = 3;
    public static final int SU_FILL_RECT = 4;
    public static final int SU_REFRESH = 5;
    public static final int SU_SET_DRAW_POSITION = 6;
    public static final int SU_DRAW_STRING = 7;
    
    // Messages that client send to server
    public static final int CMD_INIT = 8;
    public static final int CMD_CLICK = 9;
    public static final int CMD_KEYPRESS = 10;
    public static final int CMD_DATA = 11;
    public static final int CMD_CALLBACK = 12;
    public static final int CMD_ACK = 13;
            
    // Messages that app send to server
    public static final int APP_NOT_FOUND = 14;
    public static final int APP_START = 15;
    public static final int APP_MINIMIZE = 16;
    public static final int APP_EXEC = 17;
    public static final int APP_SHOW = 18;
    public static final int APP_END = 19;
    public static final int APP_INPUT_DATA = 20;
    public static final int APP_SYSCALL = 21;
    public static final int DO_LOGIN = 22;
    public static final int LOAD_IMAGE = 23;
    public static final int SET_CALLBACK = 24;
    
    // Messages that server send to app
    public static final int APP_RESIZE = 25;
    public static final int APP_REPAINT = 26;
    public static final int LOGIN = 27;
    
    //
    public static final int TOTAL_COMMANDS = 28;
};

public class Connection extends Thread {
    private CloudyClientEventListener eventListener;
    private SocketConnection socket;
    private DataInputStream socketInput = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;
    private int callbackId;
    private Rectangle rectangle = new Rectangle();
    private Color color = new Color();
    
    public Connection(CloudyClientEventListener listener) {
        eventListener = listener;
    }
    
    private int reverseBytes(int i) {
        return ((i & 0xff) << 24) + ((i & 0xff00) << 8) + ((i & 0xff0000) >>> 8) + ((i & 0xff000000) >>> 24);
    }
    
    private int readInt() throws IOException {
        return reverseBytes(is.readInt());
    }
    
    private void writeInt(DataOutputStream os, int i) throws IOException {
        os.writeInt(reverseBytes(i));
    }
    
    private void writeLong(DataOutputStream os, long l) throws IOException {
        os.writeLong(l);
    }
    
    private void readRectangle() throws IOException {
        rectangle.x = readInt();
        rectangle.y = readInt();
        rectangle.width = readInt();
        rectangle.height = readInt();
    }
    
    private void readColor() throws IOException {
        int c = readInt();
        color.red = c & 255;
        color.green = (c >> 8) & 255;
        color.blue = (c >> 16) & 255;
        color.alpha = (c >> 24) & 255;
    }
    
    public void init(int width, int height) {
        try {
            socket = (SocketConnection) Connector.open("socket://192.168.1.5:30000");
            socketInput = socket.openDataInputStream();
            os = socket.openDataOutputStream();
            writeInt(os, command_id.CMD_INIT);
            writeInt(os, width);
            writeInt(os, height);
            this.setPriority(MAX_PRIORITY);
        } catch (Exception ex) {
        }
    }
    
    public void displayError(Exception e) {
        if (e.getMessage() != null) {
            CloudyClient.logTextBox.insert(e.getMessage() + "\n" + e.toString(), 0);
        } else {
            CloudyClient.logTextBox.insert("Exception com mensagem null\n" + e.toString(), 0);
        }
        CloudyClient.display.setCurrent(CloudyClient.logTextBox);
    }
    
    public void run() {
        while (true) {
            try {
                int total_bytes = reverseBytes(socketInput.readInt());
                byte[] receiving_data = new byte[total_bytes];
                int i = 0;
                int s;
                while (total_bytes > 0) {
                    s = socketInput.read(receiving_data, i, total_bytes);
                    if (s < 0) break;
                    i += s;
                    total_bytes -= s;
                }
                ByteArrayInputStream bis = new ByteArrayInputStream(receiving_data);
                while (bis.available() > 0) {
                    is = new DataInputStream(bis);
                    i = readInt();
                    byte str[];
                    int id = i;
                    long t0;
                    long t1;
                    switch (id) {
                        case command_id.SU_CHANGE_CLIP_RECT:
                            readRectangle();
                            eventListener.change_clip_rect(rectangle);
                            break;
                        case command_id.SU_DRAW_RECT:
                            eventListener.draw_rect(readInt(), readInt());
                            break;
                        case command_id.SU_FILL_RECT:
                            eventListener.fill_rect(readInt(), readInt());
                            break;
                        case command_id.SU_CHANGE_COLOR:
                            readColor();
                            eventListener.change_color(color);
                            break;
                        case command_id.SU_REFRESH:
                            t0 = System.currentTimeMillis();
                            eventListener.refresh();
                            t1 = System.currentTimeMillis();
                            //CloudyClient.logs = CloudyClient.logs.concat("refresh [" + (t1 - t0) + "ms]\n");
                            break;
                        case command_id.APP_INPUT_DATA:
                            eventListener.input_data();
                            break;
                        case command_id.SU_SET_DRAW_POSITION:
                            int x = readInt();
                            int y = readInt();
                            eventListener.set_draw_position(x, y);
                            break;
                        case command_id.SU_DRAW_IMAGE:
                            t0 = System.currentTimeMillis();
                            s = readInt();
                            LimitedInputStream lis = new LimitedInputStream(is);
                            lis.setSizeMax(s);
                            try {
                                Image img = Image.createImage(lis);
                                t1 = System.currentTimeMillis();
                                //CloudyClient.logs = CloudyClient.logs.concat("img load [size: " + s + ", t: " + (t1 - t0) + "ms]\n");
                                eventListener.draw_image(img);
                            } catch (Exception e) {
                                displayError(e);
                                return;
                            }
                            lis.skipRemaining();
                            break;
                        case command_id.SU_DRAW_STRING:
                            s = readInt();
                            str = new byte[s];
                            is.read(str);
                            eventListener.draw_string(new String(str));
                            break;
                        case command_id.SET_CALLBACK:
                            callbackId = readInt();
                            break;
                    }
                }
            } catch (IOException e) {
                displayError(e);
                break;
            }
        }
    }
    
    synchronized void send_command(byte[] cmd_content) throws IOException {
        os.write(cmd_content);
    }

    void send_data(byte[] bytes) {
        try {
            ByteArrayOutputStream bos = new ByteArrayOutputStream(16 + bytes.length);
            DataOutputStream cmdos = new DataOutputStream(bos);
            writeInt(cmdos, command_id.CMD_CALLBACK);
            writeInt(cmdos, callbackId);
            writeInt(cmdos, command_id.CMD_DATA);
            writeInt(cmdos, bytes.length);
            cmdos.write(bytes);
            send_command(bos.toByteArray());
        } catch (IOException e) {
            displayError(e);
        }
    }

    void cmd_click(int x, int y) {
        try {
            ByteArrayOutputStream bos = new ByteArrayOutputStream(12);
            DataOutputStream cmdos = new DataOutputStream(bos);
            writeInt(cmdos, command_id.CMD_CLICK);
            writeInt(cmdos, x);
            writeInt(cmdos, y);
            send_command(bos.toByteArray());
        } catch (IOException e) {
            displayError(e);
        }
    }

    void ack(int cmd) {
        try {
            ByteArrayOutputStream bos = new ByteArrayOutputStream(16);
            DataOutputStream cmdos = new DataOutputStream(bos);
            writeInt(cmdos, command_id.CMD_ACK);
            writeInt(cmdos, cmd);
            writeLong(cmdos, System.currentTimeMillis());
            send_command(bos.toByteArray());
        } catch (IOException e) {
            displayError(e);
        }
    }
}
