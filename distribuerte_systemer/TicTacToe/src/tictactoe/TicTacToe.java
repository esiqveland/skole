/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package tictactoe;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.sound.midi.SysexMessage;

/**
 *
 * @author eivind
 */
public class TicTacToe extends UnicastRemoteObject implements Action, Constants {

    TicTacToeGui gui;
    static Action remotestubretainer; // used to retain object from garbage collection
    static Action localstubretainer; // used to retain object from garbage collection
    
    public TicTacToe(String player, char mark) throws RemoteException {
        gui = new TicTacToeGui(player, mark);
    }

    @Override
    public void sendMove(int x, int y, char myChar) throws RemoteException {
        System.err.println("Got a message: x: "+x+" y: "+y + " char: "+ myChar);
        gui.setMark(x, y, myChar);
        System.err.println("Char set!");
    }

    public static void main(String[] args) {
        
        if(System.getSecurityManager() == null) {
            // no idea what this does
            LiberalSecurityManager lsm = new LiberalSecurityManager();
            System.setSecurityManager(lsm);
            System.out.println("Security manager installed.");
        } else {
            System.out.println("Security manager already exists.");
        }
        
        try { // try setting up a registry, then export (bind) our remote object
            LocateRegistry.createRegistry(1099); 
            System.out.println("java RMI registry created.");
            try {
                //Instantiate RmiServer
                TicTacToe obj = new TicTacToe("Player1", 'X');
                localstubretainer = (Action)obj;
                // Bind this object instance to the name "RmiServer"
                Naming.rebind("//localhost/TTT", obj);
                obj.setRemoteObj(localstubretainer);
                System.out.println("PeerServer bound in registry");
                
            } catch (Exception e) {
                System.err.println("RMI server exception:" + e);
                e.printStackTrace();
            }

        } catch (RemoteException e) {
            //do nothing, error means registry already exists
            System.out.println("java RMI registry already exists.");
            System.out.println("Starting a client instead...");
            TicTacToe cl = null;
            try {
                cl = new TicTacToe("Player2", 'O');
                remotestubretainer = (Action)Naming.lookup("//localhost/TTT");
                cl.setRemoteObj(remotestubretainer);
            } catch (NotBoundException ex) {
                Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
            } catch (MalformedURLException ex) {
                Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
            } catch (RemoteException ex) {
                Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
            }
            if (cl == null) {
                System.out.println("Everything went to shits!!");
            }
            try {
                // Bind this object instance to the name "RmiServer"
                Naming.rebind("//localhost/TTTClient", cl);
                localstubretainer = (Action)cl;
                Thread.sleep(1000); // sleep to make sure export is finished
                remotestubretainer.getPlayer2();
                
                System.out.println("PeerServer bound in registry");
            } catch (Exception ex) {
                System.err.println("RMI CLients server exception:" + ex);
                ex.printStackTrace();
            }
        }       
    }

    public void setRemoteObj(Action obj) {
        gui.setActionObject(obj);
    }

    /* 
     * Fetch the remote object for player2 and save this remote object for
     * use in the gui (in player1).
     * This should already have been exported by player2.
     */
    @Override
    public void getPlayer2() throws RemoteException {
        try {
            remotestubretainer = (Action)Naming.lookup("//localhost/TTTClient");
            this.setRemoteObj(remotestubretainer);
            gui.println("Found player2!");

        } catch (NotBoundException ex) {
            
        } catch (MalformedURLException ex) {
            
        } catch (RemoteException ex) {
            
        }
    }
    
    public void newGame() throws RemoteException {
        gui.clearBoard();
    }
}
