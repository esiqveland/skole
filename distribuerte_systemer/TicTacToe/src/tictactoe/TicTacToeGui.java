package tictactoe;

import java.awt.*;
import java.awt.event.*;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.*;

/**
 * Graphical user interface to a Tic Tac Toe application.
 * The GUI is incomplete because it has no associated player.
 * It also needs a routine for checking if one of the players
 * have got five marks in a row.
 */
public class TicTacToeGui extends JFrame implements Constants, ActionListener {
	/** Textfield showing what mark you use ('X' or 'O') */
	private JTextField id;
	/** TextArea giving feedback to the user */
	private TextArea display;
	/** The panel containing the board */
	private JPanel boardPanel;
	/** The squares of the board */
	private Square board[][];
	/** The menu bar */
	private JMenuBar menuBar;
	/** The game submenu */
	private JMenu gameMenu;
	/** Game submenu choices */
	private JMenuItem newGameItem, quitItem;
	
        Action remoteobj;
	/** The name of the player using this GUI */
	private String myName;
	/** The mark used by this player ('X' or 'O') */
	private char myMark;
        private char charTurn;
        private boolean isDone;

        

	/**
	 * Creates a new GUI.
	 * @param name	The name of that player.
	 * @param mark	The mark used by that player.
	 */
	public TicTacToeGui(String name, char mark) {
		myName = name;
		myMark = mark;
                charTurn = 'O';
                isDone = false;


		// Create GUI components:
		// The display at the bottom:
		display = new TextArea("", 4, 30, TextArea.SCROLLBARS_VERTICAL_ONLY);
		display.setEditable(false);
		// The name field at the top:
		id = new JTextField();
		id.setEditable(false);
		id.setText(myName + ": You are player " + myMark);
		// The board:
		JPanel gridPanel = new JPanel();
		gridPanel.setLayout(new GridLayout(BOARD_SIZE, BOARD_SIZE, 0, 0));
		board = new Square[BOARD_SIZE][BOARD_SIZE];
		for(int row = 0; row < board.length; row++) 
			for(int col = 0; col < board[row].length; col++) {
				board[row][col] = new Square(this, row, col);
				gridPanel.add(board[row][col]);
			}
		boardPanel = new JPanel();
		boardPanel.add(gridPanel);

		// Place the components:
		Container cp = getContentPane();
		cp.setLayout(new BorderLayout());
		cp.add("South", display);
		cp.add("North", id);
		cp.add("Center", boardPanel);

		// Create the menu.
		menuBar = new JMenuBar();
		setJMenuBar(menuBar);
		gameMenu = new JMenu("Game");
		gameMenu.setMnemonic(KeyEvent.VK_G);
		menuBar.add(gameMenu);
		newGameItem = new JMenuItem("New game", KeyEvent.VK_N);
		newGameItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F2, 0));
		gameMenu.add(newGameItem);
		quitItem = new JMenuItem("Quit", KeyEvent.VK_Q);
		quitItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F4, ActionEvent.ALT_MASK));
		gameMenu.add(quitItem);

		// Add listeners
		newGameItem.addActionListener(this);
		quitItem.addActionListener(this);
		// Add an anonymous WindowListener which calls quit() when the window is closing
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				quit();
			}
		});
                
                println("Waiting for other player...");

		// Place and show the window:
		setTitle("Tic Tac Toe: " + name);
		setSize(WIN_WIDTH, WIN_HEIGHT);
		setLocation(200, 200);
		setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		setVisible(true);
	}

	/**
	 * Called by the Square class when an empty square is clicked.
	 * @param row		The row of the square that was clicked.
	 * @param column	The column of the square that was clicked.
	 */
	public void squareClicked(int row, int column) {
		// This method must be modified!
                /*try {
                    this.board = remoteobj.getBoard();
                    repaint();
                } catch (RemoteException ex) {
                    System.err.println("error getting remote board!");
                    //Logger.getLogger(TicTacToeGui.class.getName()).log(Level.SEVERE, null, ex);
                }*/

                setMark(row, column, myMark);
                
                // TODO: call remote object and tell about the move that was made
                if(remoteobj != null) {
                    try {
                        System.err.println("Sending move x: "+row+" y: "+column +" char: "+myMark);
                        remoteobj.sendMove(row, column, myMark);
                        
                    } catch (RemoteException e) {
                        System.out.println("RemoteException: "+e.getMessage());
                        e.printStackTrace();
                    }
                } else {
                    System.out.println("FAIL: remoteobj was null for player " + this.myMark);
                }
        }

	/**
	 * Marks the specified square of the board with the specified mark.
	 * @param row		The row of the square to mark.
	 * @param column	The column of the square to mark.
	 * @param mark		The mark to use.
	 */
	public void setMark(int row, int column, char mark) {	
            if(validMove(row, column, mark)){
                board[row][column].setMark(mark);
		if(isWinner(row, column, mark)){
                   isDone = true;
                   if(mark == myMark) {
                       display.setText("You win!\nGame --> New game to start again.");
                   } else {
                       display.setText("You lose!\nGame --> New game to start again.");
                   }
                   
                   System.out.print("Player: " + mark + " wins!");
                }
                charTurn = charTurn=='X'? 'O': 'X';

                repaint();
            }
	}

        public boolean validMove(int row, int column, char mark){
            if(mark == charTurn && board[row][column].getMark() == ' ' && !isDone){
               return true;
            }
            return false;
        }

        public boolean isWinner(int row, int column, char mark){
            /*
             * Counters in the three directions
             * local variable for row and col
             */

            int counterX = 0;
            int counterY = 0;
            int counterZ = 0;
            int localRow = row;
            int localColumn = column;

            for(int i = 0;i<Constants.BOARD_SIZE;i++){
                //Horizontal check
                if(board[row][i].getMark() == mark){
                    counterX++;
                    if(counterX >= 5)return true;
                } else {
                    counterX = 0;
                }

                //Vertical check
                if(board[i][column].getMark() == mark){
                    counterY++;
                    if(counterY >= 5)return true;
                } else {
                    counterY = 0;
                }

            }
            //Diagonal check

            //find top left intersect with edge of board
            while(!(localRow == 0 || localColumn == 0)){
                localRow--;
                localColumn--;
                
                if(localRow < 0)localRow = 0;
                if(localColumn < 0)localColumn = 0;
            }
            //traverse the diagonal to see if theres 5 in a row
            while(localRow < Constants.BOARD_SIZE && localColumn < Constants.BOARD_SIZE){
                if(board[localRow][localColumn].getMark() == mark){
                    counterZ++;
                    if(counterZ >= Constants.WINNER_LENGTH)return true;
                } else {
                    counterZ = 0;
                }
                localRow++;
                localColumn++;
            }

            //reset locals
            localRow = row;
            localColumn = column;
            counterZ = 0;

            //find bottom left intersect with edge of board
            while(!(localRow == 0 || localColumn == Constants.BOARD_SIZE - 1)){
                localRow--;
                localColumn++;

                if(localRow < 0)localRow = 0;
                if(localColumn == Constants.BOARD_SIZE)localColumn--;
            }

            //traverse the diagonal to see if theres 5 in a row
            while(localRow < Constants.BOARD_SIZE && localColumn >= 0){
                if(board[localRow][localColumn].getMark() == mark){
                    counterZ++;
                    if(counterZ >= Constants.WINNER_LENGTH)return true;
                } else {
                    counterZ = 0;
                }
                localRow++;
                localColumn--;
            }
            return false;
        
    }
        public void setActionObject(Action o) {
            remoteobj = o;
            println("Player found!");
            //try {
            //    this.board = remoteobj.getBoard();
            //    repaint();
            //} catch (RemoteException ex) {
            //    System.err.println("error getting remote board!");
            //    //Logger.getLogger(TicTacToeGui.class.getName()).log(Level.SEVERE, null, ex);
            //}
        }
	/**
	 * Called when a menu item has been selected.
	 * @param e	The ActionEvent that occured.
	 */
	public void actionPerformed(ActionEvent e) {
		if(e.getSource() == newGameItem) {
			newGame();
                } else if(e.getSource() == quitItem)
			quit();
	}

	/**
	 * Starts a new game, if the user confirms it in a dialog box.
	 */
	public void newGame() {
		// This method must be modified!
		if(JOptionPane.showConfirmDialog(this, "Are you sure you want to start a new game?", "Start over?", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
			clearBoard();
                        try {
                            remoteobj.newGame();
                        } catch (RemoteException ex) {
                            ex.printStackTrace();
                        }
                        isDone = false;
		}
	}

        public Square[][] getBoard() {
            return this.board;
        }
	/**
	 * Removes all marks from the board.
	 */
	public void clearBoard() {
                for(int row = 0; row < board.length; row++)
			for(int col = 0; col < board[row].length; col++)
				board[row][col].setMark(' ');
		repaint();
	}

	/**
	 * Exits the game, if the user confirms it in a dialog box.
	 * Should notify the opponent that we left the game.
	 */
	public void quit() {
		// This method should be modified!
		if(JOptionPane.showConfirmDialog(this, "Are you sure you want to quit?", "Really quit?", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
			System.exit(0);
		}
	}

	/**
	 * Outputs a message to the user.
	 * @param s	The string to display. Adds a newline to the end of the string.
	 */
	public void println(String s) {
		display.append(s + "\n");
	}

	/**
	 * Outputs a message to the user.
	 * @param s	The string to display.
	 */
	public void print(String s) {
		display.append(s);
	}

    public String findHost() {
        return JOptionPane.showInputDialog("Enter IP of host or nothing to be the host: ");
        
    }

    void setPlayer(String player2, char c) {
        this.myMark = c;
        this.myName = player2;
    }

	/**
	 * Starts up a GUI without an associated player, in order
	 * to check the appearance of the GUI.
	 */
//	public static void main(String args[]) {
//		TicTacToeGui hisGui = new TicTacToeGui("Ottar", 'X');
//	}
}
