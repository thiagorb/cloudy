import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.GameCanvas;

public class CloudyDisplay extends GameCanvas implements CloudyClientEventListener {
    private Graphics g;
    private Connection connection = new Connection(this);
    private int draw_x;
    private int draw_y;
    private CloudyClient midlet;
    
    public CloudyDisplay(CloudyClient midlet) {
        super(false);
        this.setFullScreenMode(true);
        this.midlet = midlet;
        g = this.getGraphics();
        g.setFont(Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL));
        connection.init(this.getWidth(), this.getHeight());
        connection.start();
    }
    
    public void change_clip_rect(Rectangle rect) {
        g.setClip(rect.x, rect.y, rect.width, rect.height);
    }

    public void change_color(Color color) {
        System.out.println(color.toString());
        g.setColor(color.red, color.green, color.blue);
    }

    public void fill_rect(int width, int height) {
        g.fillRect(draw_x, draw_y, width, height);
    }

    public void draw_rect(int width, int height) {
        g.drawRect(draw_x, draw_y, width, height);
    }

    public void input_data() {
        midlet.textInput();
    }
    
    public void after_text_input(String resposta) {
        connection.ack(command_id.CMD_DATA);
        connection.send_data(resposta.getBytes());
    }

    public void set_draw_position(int x, int y) {
        draw_x = x;
        draw_y = y;
    }

    public void draw_string(String str) {
        System.out.println(str);
        System.out.println(draw_x);
        System.out.println(draw_y);
        System.out.println(Integer.toHexString(g.getColor()));
        g.drawString(str, draw_x, draw_y, Graphics.BASELINE | Graphics.LEFT);
    }

    public void draw_image(Image img) {
        g.drawImage(img, draw_x, draw_y, Graphics.LEFT | Graphics.TOP);
    }
    
    public void pointerReleased(int x, int y) {
        connection.cmd_click(x, y);
        connection.ack(command_id.CMD_CLICK);
    }

    public void refresh() {
        this.flushGraphics();
        connection.ack(command_id.SU_REFRESH);
    }
}
