# Fantastic Four (Connect Four, 4 in a row)

## Statement

Fantastic Fours is a two-player connection game that is played on a 7x8 vertical grid.
The game starts with an empty grid. The players have their own colored discs and the
take turns in dropping their colored disc from the top into a column of the grid.
The pieces fall straight down, occupying the next available space within the column.
The objective of the game is to connect four of one's own discs of the same color next
to each other vertically, horizontally, or diagonally before your opponent.
The game ends when a player wins or all the columns of the grid are filled upto the top.

Your aim is to implement a bot which plays Fantastic Four with other's or default bot.
The program you submit will run for each move played by your player in the game. 

### Input 
The input will be a 7x8 matrix consisting of integers. Each integer represents the state
of a box in the grid.Then next line in the input contains your player id (1 or 2). The
difference between player 1 and 2 is that player 1 plays first in start of the game. 
In the given matrix, top-left is [0,0] and bottom-right is [6,7]. The coordinate of a
cell is represented by [row, column]. Rows increases from top to bottom and column
increases from left to right.
The cell marked 0 means it is empty. The cell marked 1 means it contains player 1's
piece which is red in color. The cell marked 2 means it contains player 2's piece
which is green in color. 

### Output 
Print the column number where you want to insert your piece into. It will move down vertically as far as it can. 

## Links
[Original competition](https://www.hackerearth.com/problem/multiplayer/fantastic-four/description/)

