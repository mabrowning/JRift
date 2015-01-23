package de.fruitfly.ovr.structs;

public class Recti
{
    public Recti()
    {

    }

    public Recti(Vector2i POS, Sizei SIZE)
    {
        Pos = POS;
        Size = SIZE;
    }

    public Vector2i Pos = new Vector2i();
    public Sizei    Size = new Sizei();
}
