package de.fruitfly.ovr.structs;

/**
 * Created by StellaArtois on 12/30/2015.
 */
public class ErrorInfo {
    public String errorStr;
    public int error = 0;
    public boolean success = true;
    public boolean unqualifiedSuccess = true;
    public boolean shutdownReq = false;

    public static int OVR_SHUTDOWN_APP_REQUESTED = 9999;

    public ErrorInfo() {}

    public ErrorInfo(String errorStr, int errorNo, boolean isSuccess, boolean isUnqualifiedSuccess) {
        this.errorStr = errorStr;
        this.error = errorNo;
        this.success = isSuccess;
        this.unqualifiedSuccess = isUnqualifiedSuccess;
        if (this.error == OVR_SHUTDOWN_APP_REQUESTED) {
            this.shutdownReq = true;
        }
    }
}
