/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package tictactoe;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author eivind
 */
public class TicTacToe implements Action, Constants {

    TicTacToeGui gui;
    private Action mystubretainer;
    
    public TicTacToe(String player, char mark) {
        gui = new TicTacToeGui(player, mark);
        //gui.newGame();
    }

    @Override
    public void sendMove(int x, int y, char myChar) throws RemoteException {
        System.err.println("Got a message: x: "+x+" y: "+y + " char: "+ myChar);
        gui.setMark(x, y, myChar);
        System.err.println("Char set!");
    }

    public static void main(String[] args) {
                
        try {
            Action ac = null;
            try {
                Registry registry = LocateRegistry.getRegistry(10000);
                ac = (Action)registry.lookup("TTT");
            }  catch(Exception e) {
                System.err.println("ERR finding client\nWill make my own: " + e.getMessage());
            }
            System.out.println("Test1");
            if(ac == null) { // start own server
                TicTacToe t = new TicTacToe("Player1", 'X');
                
                // Bind the remote object's stub in the registry
                Action stub = (Action) UnicastRemoteObject.exportObject(t, 10000);
                t.setStub(stub);
                //Registry registry = LocateRegistry.getRegistry();
                Registry registry = LocateRegistry.createRegistry(10000);
                registry.bind("TTT", stub);
                //t.shit(stub);
                System.err.println("Server ready");
            } else { // We are the other player
                TicTacToe t = new TicTacToe("Player2", 'O');
                t.shit(ac);
                // we must also register this new TicTacToe object with the
                // one already running somehow...
                Action stub = (Action) UnicastRemoteObject.exportObject(t, 10000);
                Registry registry = LocateRegistry.getRegistry(10000);
                registry.bind("TTT2", stub);
                ac.getPlayer2();
                
            }
        } catch(Exception e) {
            System.err.println("ERR: " + e.getMessage());
            System.err.println("ERROR: " + e.getStackTrace());
        }
        
    }

    public void shit(Action obj) {
        gui.setActionObject(obj);
    }

    @Override
    public Square[][] getBoard() throws RemoteException {
        return gui.getBoard();
    }

    @Override
    public int getX() throws RemoteException {
        return BOARD_SIZE;
    }

    @Override
    public int getY() throws RemoteException {
        return BOARD_SIZE;
    }

    /* 
     * Fetch the remote object for player2.
     * If this method is called, we shuare[][] getBoard() throws RemoteException {
        return gui.getBoard();
    }

    @Override
    public int getX() throws RemoteException {
        return BOARD_SIZE;
    }

    @Override
    public int getY() throws RemoteException {
        return BOARD_SIZE;
    }

    /* 
     * Fetch the remote object for plaould be player1, and already
     * be in contact with player2.
     */
    @Override
    public void getPlayer2() {
        try {
            Registry registry = LocateRegistry.getRegistry(10000);
            Action ac = (Action) registry.lookup("TTT2");
            this.shit(ac);
        } catch (Exception ex) {
            System.err.println("Err: Could not get player2 Action object");
            //Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private void setStub(Action stub) {
        mystubretainer = stub;
    }

}
