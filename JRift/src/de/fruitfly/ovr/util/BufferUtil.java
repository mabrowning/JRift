package de.fruitfly.ovr.util;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

public class BufferUtil
{
    public static synchronized ByteBuffer createByteBuffer(int size)
    {
        return ByteBuffer.allocateDirect(size).order(ByteOrder.nativeOrder());
    }

    public static IntBuffer createIntBuffer(int size)
    {
        return createByteBuffer(size << 2).asIntBuffer();
    }

    public static FloatBuffer createFloatBuffer(int size)
    {
        return createByteBuffer(size << 2).asFloatBuffer();
    }
}
