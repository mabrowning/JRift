package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.util.BufferUtil;

import java.nio.FloatBuffer;

public class Matrix4f
{
    public Matrix4f(float m11, float m12, float m13, float m14,
                    float m21, float m22, float m23, float m24,
                    float m31, float m32, float m33, float m34,
                    float m41, float m42, float m43, float m44)
    {
        M[0][0] = m11; M[0][1] = m12; M[0][2] = m13; M[0][3] = m14;
        M[1][0] = m21; M[1][1] = m22; M[1][2] = m23; M[1][3] = m24;
        M[2][0] = m31; M[2][1] = m32; M[2][2] = m33; M[2][3] = m34;
        M[3][0] = m41; M[3][1] = m42; M[3][2] = m43; M[3][3] = m44;
    }

    public Matrix4f(Quatf q)
    {
        float ww = q.w*q.w;
        float xx = q.x*q.x;
        float yy = q.y*q.y;
        float zz = q.z*q.z;

        M[0][0] = ww + xx - yy - zz;       M[0][1] = 2 * (q.x*q.y - q.w*q.z); M[0][2] = 2 * (q.x*q.z + q.w*q.y); M[0][3] = 0;
        M[1][0] = 2 * (q.x*q.y + q.w*q.z); M[1][1] = ww - xx + yy - zz;       M[1][2] = 2 * (q.y*q.z - q.w*q.x); M[1][3] = 0;
        M[2][0] = 2 * (q.x*q.z - q.w*q.y); M[2][1] = 2 * (q.y*q.z + q.w*q.x); M[2][2] = ww - xx - yy + zz;       M[2][3] = 0;
        M[3][0] = 0;                       M[3][1] = 0;                       M[3][2] = 0;                       M[3][3] = 1;
    }

    public Matrix4f()
    {

    }

    Matrix4f(Matrix4f c)
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                M[i][j] = c.M[i][j];
    }

    public Matrix4f inverted()
    {
        float det = Determinant();
        if (det == 0)
            return null;
        return Adjugated().Multiply(1.0f/det);
    }

    Matrix4f Multiply(float s)
    {
        Matrix4f d = new Matrix4f(this);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                d.M[i][j] *= s;

        return d;
    }

    public Matrix4f transposed()
    {
        return new Matrix4f(M[0][0], M[1][0], M[2][0], M[3][0],
                M[0][1], M[1][1], M[2][1], M[3][1],
                M[0][2], M[1][2], M[2][2], M[3][2],
                M[0][3], M[1][3], M[2][3], M[3][3]);
    }

    float SubDet (final int[] rows, final int[] cols)
    {
        return M[rows[0]][cols[0]] * (M[rows[1]][cols[1]] * M[rows[2]][cols[2]] - M[rows[1]][cols[2]] * M[rows[2]][cols[1]])
                - M[rows[0]][cols[1]] * (M[rows[1]][cols[0]] * M[rows[2]][cols[2]] - M[rows[1]][cols[2]] * M[rows[2]][cols[0]])
                + M[rows[0]][cols[2]] * (M[rows[1]][cols[0]] * M[rows[2]][cols[1]] - M[rows[1]][cols[1]] * M[rows[2]][cols[0]]);
    }

    float Cofactor(int I, int J)
    {
        int indices[][] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};
        return ((I+J)&1) != 0 ? -SubDet(indices[I],indices[J]) : SubDet(indices[I],indices[J]);
    }

    float Determinant()
    {
        return M[0][0] * Cofactor(0,0) + M[0][1] * Cofactor(0,1) + M[0][2] * Cofactor(0,2) + M[0][3] * Cofactor(0,3);
    }

    Matrix4f Adjugated()
    {
        return new Matrix4f(Cofactor(0,0), Cofactor(1,0), Cofactor(2,0), Cofactor(3,0),
                Cofactor(0,1), Cofactor(1,1), Cofactor(2,1), Cofactor(3,1),
                Cofactor(0,2), Cofactor(1,2), Cofactor(2,2), Cofactor(3,2),
                Cofactor(0,3), Cofactor(1,3), Cofactor(2,3), Cofactor(3,3));
    }

    public FloatBuffer toFloatBuffer() 
    {
        FloatBuffer buf = BufferUtil.createFloatBuffer(16);

        buf.put(M[0][0]);buf.put(M[0][1]);buf.put(M[0][2]);buf.put(M[0][3]);
        buf.put(M[1][0]);buf.put(M[1][1]);buf.put(M[1][2]);buf.put(M[1][3]);
        buf.put(M[2][0]);buf.put(M[2][1]);buf.put(M[2][2]);buf.put(M[2][3]);
        buf.put(M[3][0]);buf.put(M[3][1]);buf.put(M[3][2]);buf.put(M[3][3]);
        buf.flip();

        return buf;
    }
    
    public float M[][] = new float[4][4];
}
