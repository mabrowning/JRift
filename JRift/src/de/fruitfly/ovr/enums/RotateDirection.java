package de.fruitfly.ovr.enums;

public enum RotateDirection
{
    Rotate_CCW(1),
    Rotate_CW(-1);

    private final int enuum;

    public int value()
    {
        return this.enuum;
    }

    private RotateDirection(int value)
    {
        this.enuum = value;
    }
}
