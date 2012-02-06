/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package tictactoe;

import java.awt.Dimension;
import java.rmi.Remote;
import java.rmi.RemoteException;



/**
 *
 * @author Eivind
 */
public interface Action extends Remote {
   
    public void sendMove(int x, int y, char myChar) throws RemoteException;
    public void getPlayer2(String host) throws RemoteException;
    public void newGame() throws RemoteException;
}
