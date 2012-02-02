/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package tictactoe;

import java.rmi.Remote;
import java.rmi.RemoteException;

/**
 *
 * @author Eivind
 */
public interface PlayMove extends Remote {
    // <T> T executeTask(Task<T> t) throws RemoteException;
    boolean PlayMove(Action m) throws RemoteException;
    
}
