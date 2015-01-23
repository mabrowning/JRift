package de.fruitfly.ovr.enums;

public enum Axis
{
    Axis_X(0),
    Axis_Y(1),
    Axis_Z(2);

    private final int enuum;

    public int value()
    {
        return this.enuum;
    }

    private Axis(int value)
    {
        this.enuum = value;
    }
}
