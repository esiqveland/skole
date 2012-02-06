/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package tictactoe;

import java.net.Inet4Address;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.sound.midi.Soundbank;
import javax.sound.midi.SysexMessage;

/**
 *
 * @author eivind
 */
public class TicTacToe extends UnicastRemoteObject implements Action, Constants {
    private static String hostname;

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
        TicTacToe obj = null;
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
            obj = new TicTacToe("Player1", 'X');
            hostname = String.valueOf(obj.findHostname());
            
            if(!hostname.isEmpty()) {
                throw new RemoteException("NOT A HOST!");
            }

            try {
                //Instantiate RmiServer
                localstubretainer = (Action)obj;
                // Bind this object instance to the name "RmiServer"
                Naming.rebind("//localhost/TTT", obj);
                //obj.setRemoteObj(localstubretainer);
                System.out.println("Exported server");
                
            } catch (Exception e) {
                System.err.println("RMI server exception:" + e);
                e.printStackTrace();
            }

        } catch (RemoteException e) {
            //do nothing, error means registry already exists
            System.out.println("java RMI registry already exists.");
            System.out.println("Starting a client instead...");
            try {
                System.out.println("IP: "+Inet4Address.getLocalHost().getHostAddress());
                obj.setPlayer("Player2", 'O');
                System.out.println("Trying to open host: //"+hostname+"/TTT");
                remotestubretainer = (Action)Naming.lookup("//"+hostname+"/TTT");
                obj.setRemoteObj(remotestubretainer);
            } catch (NotBoundException ex) {
                Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
            } catch (MalformedURLException ex) {
                Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
            } catch (Exception ex) {
                Logger.getLogger(TicTacToe.class.getName()).log(Level.SEVERE, null, ex);
            }
            if (obj == null) {
                System.out.println("Everything went to shits!!");
            }
            try {
                // Bind this object instance to the name "RmiServer"
                Naming.rebind("//localhost/TTTClient", obj);
                localstubretainer = (Action)obj;
                Thread.sleep(1000); // sleep to make sure export is finished
                remotestubretainer.getPlayer2(Inet4Address.getLocalHost().getHostAddress());
                
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
    public void getPlayer2(String host) throws RemoteException {
        try {
            remotestubretainer = (Action)Naming.lookup("//"+host+"/TTTClient");
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

    public String findHostname() {
        return gui.findHost();
    }

    public void setPlayer(String player2, char c) {
        gui.setPlayer(player2, c);
    }
}
