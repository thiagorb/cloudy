public class Color {
    public int alpha;
    public int red;
    public int green;
    public int blue;
    
    public Color(int red, int green, int blue, int alpha) {
        this.red = red;
        this.green = green;
        this.blue = blue;
    }

    Color() {
        
    }
    
    public String toString() {
        return "Cor: [R: " + this.red + ", G: " + this.green + ", B: " + this.blue + "]";
    }
}
