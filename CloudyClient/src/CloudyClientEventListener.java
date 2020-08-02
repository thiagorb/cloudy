
import javax.microedition.lcdui.Image;

public interface CloudyClientEventListener {
    public void change_clip_rect(Rectangle rectangle);
    public void change_color(Color color);
    public void fill_rect(int width, int height);

    public void refresh();

    public void draw_rect(int width, int height);

    public void input_data();
    public void set_draw_position(int x, int y);
    public void draw_string(String str);

    public void draw_image(Image img);
}
