package de.fruitfly.ovr.structs;

/**
 * Created by StellaArtois on 12/30/2015.
 */
public class ErrorInfo {
    String errorStr;
    int error = 0;
    boolean success = true;
    boolean unqualifiedSuccess = true;

    public ErrorInfo(String errorStr, int errorNo, boolean isSuccess, boolean isUnqualifiedSuccess) {
        this.errorStr = errorStr;
        this.error = errorNo;
        this.success = isSuccess;
        this.unqualifiedSuccess = isUnqualifiedSuccess;
    }
}
