package de.fruitfly.ovr.structs;

/**
 * Created by StellaArtois on 12/30/2015.
 */
public class ErrorInfo {
    public String errorStr;
    public int error = 0;
    public boolean success = true;
    public boolean unqualifiedSuccess = true;

    public ErrorInfo() {}

    public ErrorInfo(String errorStr, int errorNo, boolean isSuccess, boolean isUnqualifiedSuccess) {
        this.errorStr = errorStr;
        this.error = errorNo;
        this.success = isSuccess;
        this.unqualifiedSuccess = isUnqualifiedSuccess;
    }
}
