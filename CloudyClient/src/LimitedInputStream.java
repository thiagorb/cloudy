

import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.InputStream;

public class LimitedInputStream
        extends InputStream {
    private long sizeMax;
    private long count;
    private boolean closed;
    private InputStream is;

    public LimitedInputStream(InputStream pIn, long pSizeMax) {
        is = pIn;
        sizeMax = pSizeMax;
    }

    public LimitedInputStream(InputStream pIn) {
        is = pIn;
        sizeMax = 0;
    }

    /**
     * Reads the next byte of data from this input stream. The value
     * byte is returned as an <code>int</code> in the range
     * <code>0</code> to <code>255</code>. If no byte is available
     * because the end of the stream has been reached, the value
     * <code>-1</code> is returned. This method blocks until input data
     * is available, the end of the stream is detected, or an exception
     * is thrown.
     * <p>
     * This method
     * simply performs <code>in.read()</code> and returns the result.
     *
     * @return     the next byte of data, or <code>-1</code> if the end of the
     *             stream is reached.
     * @exception  IOException  if an I/O error occurs.
     * @see        java.io.FilterInputStream#in
     */
    public int read() throws IOException {
        if (count >= sizeMax) return -1;
        int res = is.read();
        if (res != -1) {
            count++;
        }
        return res;
    }
    
    public int read(byte[] b) throws IOException {
        if (count >= sizeMax) return -1;
        int len = (int)(sizeMax - count);
        if (len > b.length) len = b.length;
        int res = is.read(b, 0, len);
        if (res > 0) {
            count += res;
        }
        return res;
    }

    /**
     * Reads up to <code>len</code> bytes of data from this input stream
     * into an array of bytes. If <code>len</code> is not zero, the method
     * blocks until some input is available; otherwise, no
     * bytes are read and <code>0</code> is returned.
     * <p>
     * This method simply performs <code>in.read(b, off, len)</code>
     * and returns the result.
     *
     * @param      b     the buffer into which the data is read.
     * @param      off   The start offset in the destination array
     *                   <code>b</code>.
     * @param      len   the maximum number of bytes read.
     * @return     the total number of bytes read into the buffer, or
     *             <code>-1</code> if there is no more data because the end of
     *             the stream has been reached.
     * @exception  NullPointerException If <code>b</code> is <code>null</code>.
     * @exception  IndexOutOfBoundsException If <code>off</code> is negative,
     * <code>len</code> is negative, or <code>len</code> is greater than
     * <code>b.length - off</code>
     * @exception  IOException  if an I/O error occurs.
     * @see        java.io.FilterInputStream#in
     */
    
    public int read(byte[] b, int off, int len) throws IOException {
        if (count >= sizeMax) return -1;
        if (count + len >= sizeMax) len = (int) (sizeMax - count);
        int res = is.read(b, off, len);
        if (res > 0) {
            count += res;
        }
        return res;
    }

    /**
     * Returns, whether this stream is already closed.
     * @return True, if the stream is closed, otherwise false.
     * @throws IOException An I/O error occurred.
     */
    public boolean isClosed() throws IOException {
        return closed;
    }

    /**
     * Closes this input stream and releases any system resources
     * associated with the stream.
     * This
     * method simply performs <code>in.close()</code>.
     *
     * @exception  IOException  if an I/O error occurs.
     * @see        java.io.FilterInputStream#in
     */
    
    public void skipRemaining() throws IOException {
        skip(sizeMax - count);
    }
    
    public void setSizeMax(int value) {
        count = 0;
        sizeMax = value;
    }
    
    public void close() throws IOException {
        closed = true;
        super.close();
    }
}