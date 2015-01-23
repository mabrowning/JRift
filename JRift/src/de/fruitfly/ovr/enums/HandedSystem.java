package de.fruitfly.ovr.enums;

public enum HandedSystem
{
    Handed_R(1),
    Handed_L(-1);

    private final int enuum;

    public int value()
    {
        return this.enuum;
    }

    private HandedSystem(int value)
    {
        this.enuum = value;
    }
}
