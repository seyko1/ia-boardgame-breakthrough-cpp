package bk;

//Playing with Ludii
//Compile to obtain a .class
// $> javac -cp Ludii-XXX.jar FgPlayer.java
//As the class is in the ttt package, create a dir and place the class inside
// $> mkdir ttt
// $> mv FgPlayer.class bk/
//Make a  jar 
// $> jar cf FgPlayer.jar bk/FgPlayer.class
//Start Ludii, select the player and play with it
// $> 	

import java.util.concurrent.ThreadLocalRandom;

import game.Game;
import main.collections.FastArrayList;
import other.AI;
import other.context.Context;
import other.state.container.ContainerState;
import other.move.Move;
import other.action.others.ActionPass;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class FgPlayer extends AI {		
	public final static int EMPTY = 0;
	public final static int BLACK = 1;
	public final static int WHITE = 2;
	public final static boolean LOG_ACTIVE = false;
	public final static String local_player_str = "remote_cpp/rand_player_cmd";
	public final static int BOARDHEIGHT = 6;
  public final static int BOARDWIDTH = 10;

	protected int player = -1; // player_index
	
	public FgPlayer()	{
		this.friendlyName = "FgPlayer";
	}
	
	@Override
	public Move selectAction
	(
		final Game game, 
		final Context context, 
		final double maxSeconds,
		final int maxIterations,
		final int maxDepth
	)
	{
		FastArrayList<Move> legalMoves = game.moves(context).moves();    
		StringBuffer str_board = new StringBuffer();
		for (final ContainerState containerState : context.state().containerStates()) {
			for(int i = BOARDHEIGHT-1; i >= 0; i--) {
				StringBuffer str_line = new StringBuffer();
				for(int j = 0; j < BOARDWIDTH; j++) {
					int idx = i*BOARDWIDTH+j;
					if(containerState.isEmptyCell(idx)) { /*board[i] = EMPTY;*/ str_line.append("."); }
					else if(containerState.whoCell(idx) == 1) { /*board[i] = BLACK;*/ str_line.append("o"); }
					else if(containerState.whoCell(idx) == 2) { /*board[i] = WHITE;*/ str_line.append("@"); }
				}
				str_board.append(str_line);
			}
		}
		
		String turn = "o";
		if(player==2) turn = "@";
		String genmove_time = "1";
		String res = "";		
		int line_i = -1;
		int col_i = -1;
		int line_f = -1;
		int col_f = -1;
		try {
			//System.err.println("[cmd] "+white_str+" "+black_str);
				res = getProcessOutput(str_board.toString(), turn, genmove_time);
				if(res.length()==4) {
					if(LOG_ACTIVE) {
						System.out.println(local_player_str+" play "+res);
					}
  		  	line_i = res.charAt(0)-'1';
	  	  	col_i = res.charAt(1)-'a';
		    	line_f = res.charAt(2)-'1';
		    	col_f = res.charAt(3)-'a';
    			if(LOG_ACTIVE) System.err.println("= ("+line_i+" "+col_i+") ("+line_f+" "+col_f+")");
				} else {
					System.err.println("error res.length() "+res.length()+" |"+res+"|");
				}
    } catch(Exception e) { 
      e.printStackTrace();
    }
		if(line_i!=-1 && col_i !=-1 && line_f!=-1 && col_f!=-1) {
  		int pos_i = line_i*BOARDWIDTH+col_i;
	  	int pos_f = line_f*BOARDWIDTH+col_f;
    	if(LOG_ACTIVE) System.err.println("converted to "+pos_i+" "+pos_f);
		  for(int i = 0; i < legalMoves.size(); i++) {
				//System.err.print(legalMoves.get(i).from()+"-"+legalMoves.get(i).to()+" ");
			  if(legalMoves.get(i).from() == pos_i && legalMoves.get(i).to() == pos_f) { System.err.println(""); return legalMoves.get(i);}
		  }
			//System.err.println("");
		}	  
		System.out.println(local_player_str+" play random");
		return legalMoves.get(0);
	}
	
	@Override
	public void initAI(final Game game, final int playerID)
	{
		this.player = playerID;
	}	
	public String getProcessOutput(String _board, String _turn, String _time) throws IOException, InterruptedException
	{
		ProcessBuilder processBuilder = new ProcessBuilder(System.getProperty("user.dir")+"/"+local_player_str,_board,_turn,_time);	
		processBuilder.redirectErrorStream(false);
		Process process = processBuilder.start();
		StringBuilder processOutput = new StringBuilder();
		StringBuilder processErr = new StringBuilder();
		try (BufferedReader processOutputReader = new BufferedReader(
						new InputStreamReader(process.getInputStream()));) {
				String readLine;
				while ((readLine = processOutputReader.readLine()) != null) {
						processOutput.append(readLine + System.lineSeparator());
				}
				process.waitFor();
		}
		try (BufferedReader processOutputReader = new BufferedReader(
						new InputStreamReader(process.getErrorStream()));) {
				String readLine;
				while ((readLine = processOutputReader.readLine()) != null) {
						processErr.append(readLine + System.lineSeparator());
				}
				process.waitFor();
		}		
		System.err.println(processErr.toString().trim());
		return processOutput.toString().trim();
	}
}
