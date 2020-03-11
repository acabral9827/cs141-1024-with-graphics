//  prog5_1024.cpp
//     Text and graphical version of the game 1024, a variant of 2048 available online at:
//        http://gabrielecirulli.github.io/2048/
//     Object of game is to combine number tiles with the same value, accumulating
//     points as the game progresses, to try and create the tile with the value 1024.
//     Each moveNumber consists of sliding pieces to the left, up, right or down.
// 
// Instructions:
//    To run the graphical version, first select the "Build and Run" option at the top of the window. You
//    can ignore the following error messages that will appear:
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to initialize inotify, joystick connections and disconnections won't be notified
//    To see the graphical output then select the "Viewer" option at the top of the window.
//    
// For more information about SFML graphics, see: https://www.sfml-dev.org/tutorials
// Be sure to close the old window each time you rebuild and rerun, to ensure you
// are seeing the latest output.
//
//
/*  Running the program (for the textual output) looks like:

*/

#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // Since we are using multiple libraries, now use std::
// in front of every cin, cout, endl, setw, and string 
#include <iomanip>           // used for setting output field size using setw
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen()  
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)

const int WindowXSize = 400;
const int WindowYSize = 500;
const int MaxBoardSize = 12;  // Max number of squares per side
const int MaxTileStartValue = 1024;   // Max tile value to start out on a 4x4 board


//---------------------------------------------------------------------------------------
class Square {
public:
	// Default Constructor 
	Square()
	{
		size = 0;
		xPosition = 0;
		yPosition = 0;
		color = sf::Color::Black;
		isVisible = false;
		isCaptured = false;
		text = "";
	}

	// Fully-qualified constructor, used to set all fields
	Square(int theSize, int theXPosition, int theYPosition,
		const sf::Color &theColor, bool theVisibility, std::string theText)
	{
		// Set the class fields
		size = theSize;
		xPosition = theXPosition;
		yPosition = theYPosition;
		color = theColor;
		isVisible = theVisibility;
		isCaptured = false;   // By default squares have not been captured
		text = theText;
		// Use the values to set the display characteristics of theSquare
		theSquare.setSize(sf::Vector2f(theSize, theSize));
		theSquare.setPosition(theXPosition, theYPosition);   // Set the position of the square
		theSquare.setFillColor(theColor);
	}

	// Get (accessor) functions
	sf::RectangleShape getTheSquare() { return theSquare; }
	int getSize() { return size; }
	int getXPosition() { return xPosition; }
	int getYPosition() { return yPosition; }
	sf::Color& getColor() { return color; }
	bool getIsVisible() { return isVisible; }
	bool getIsCaptured() { return isCaptured; }
	std::string getText() { return text; }

	// Set (mutator) functions
	void setSize(int theSize) {
		size = theSize;
		theSquare.setSize(sf::Vector2f(theSize, theSize));
	}
	void setXPosition(int theXPosition) {
		xPosition = theXPosition;
		theSquare.setPosition(theXPosition, yPosition);   // Set the position of the square
	}
	void setYPosition(int theYPosition) {
		yPosition = theYPosition;
		theSquare.setPosition(xPosition, theYPosition);   // Set the position of the square
	}
	void setColor(sf::Color & theColor) {
		color = theColor;
		theSquare.setFillColor(theColor);    // Also update the color on the square itself
	}
	void setColor(int R, int G, int B) {
		sf::Color theNewColor(R, G, B);
		color = theNewColor;
		theSquare.setFillColor(theNewColor);
	}
	void setVisibility(bool theVisibility) { isVisible = theVisibility; }
	void setIsCaptured(bool isCaptured) { this->isCaptured = isCaptured; }
	void setText(std::string theText) { text = theText; }

	// Utility functions
	void displayText(sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);

private:
	int size;
	int xPosition;
	int yPosition;
	sf::Color color;
	bool isVisible;
	bool isCaptured;   // Indicates whether or not it is part of the captured area
	std::string text;
	sf::RectangleShape theSquare;

}; //end class Square


//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text 
// associated with this Square.
// 
// Assuming we display output in sf::RenderWindow window(...), then call this function using: 
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
void Square::displayText(
	sf::RenderWindow *pWindow,   // The window into which we draw everything
	sf::Font theFont,            // Font to be used in displaying text   
	sf::Color theColor,          // Color of the font
	int textSize)                // Size of the text to be displayed
{
	// Create a sf::Text object to draw the text, using a sf::Text constructor
	sf::Text theText(text,        // text is a class data member
		theFont,     // font from a font file, passed as a parameter
		textSize);   // this is the size of text to be displayed

// Text color is the designated one, unless the background is Yellow, in which case the text
// color gets changed to blue so we can see it, since we can't see white-on-yellow very well
	if (this->getColor() == sf::Color::Yellow) {
		theColor = sf::Color::Blue;
	}
	theText.setColor(theColor);

	// Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
	// For horizontal center, find the center of the square and subtract half the width of the text 
	int theXPosition = xPosition + (size / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
	// For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
	int theYPosition = yPosition + (size - theText.getCharacterSize()) / 2;
	// Use an additional offset to get it centered
	int offset = 5;
	theText.setPosition(theXPosition + offset, theYPosition - offset);

	// Finally draw the Text object in the RenderWindow
	pWindow->draw(theText);
}


//---------------------------------------------------------------------------------------
// Initialize the font
void initializeFont(sf::Font &theFont)
{
	// Create the global font object from the font file
	if (!theFont.loadFromFile("arial.ttf"))
	{
		std::cout << "Unable to load font. " << std::endl;
		exit(-1);
	}
}

//--------------------------------------------------------------------
//
class Node
{
public:
	int board[MaxBoardSize*MaxBoardSize];   // Game board
	int score;     // Current score of the game
	int moveNumber;  // # of moves until game is finished
	Node *pNext;
};

//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
	std::cout << "Welcome to 1024. \n"
		<< "  \n"
		<< "For each moveNumber enter a direction as a letter key, as follows: \n"
		<< "    W    \n"
		<< "  A S D  \n"
		<< "where A=left,W=up, D=right and S=down. \n"
		<< "  \n"
		<< "After a moveNumber, when two identical valued tiles come together they    \n"
		<< "join to become a new single tile with the value of the sum of the   \n"
		<< "two originals. This value gets added to the score.  On each moveNumber    \n"
		<< "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
		<< "square.  User input of x exits the game.                            \n";
	//<< "  \n";
}//end displayInstructions()

//--------------------------------------------------------------------
// Display the text-based Board
void displayAsciiBoard(int board[], int squaresPerSide, int score)
{
	std::cout << "\n"
		<< "        Score: " << score << std::endl;
	for (int row = 0; row < squaresPerSide; row++) {
		std::cout << "   ";
		for (int col = 0; col < squaresPerSide; col++) {
			int current = row * squaresPerSide + col;  // 1-d index corresponding to row & col
			std::cout << std::setw(6);    // set output field size to 6 (Requires #include <iomanip> )
			// display '.' if board value is 0
			if (board[current] == 0) {
				std::cout << '.';
			}
			else {
				std::cout << board[current];
			}
		}
		std::cout << "\n\n";
	}
}//end displayBoard()

//--------------------------------------------------------------------
// Place a randomly selected 2 or 4 into a random open square on
// the board.
void placeRandomPiece(int board[], int squaresPerSide)
{
	// Randomly choose a piece to be placed (2 or 4)
	int pieceToPlace = 2;
	if (rand() % 2 == 1) {
		pieceToPlace = 4;
	}

	// Find an unoccupied square that currently has a 0
	int index;
	do {
		index = rand() % (squaresPerSide*squaresPerSide);
	} while (board[index] != 0);

	// board at position index is blank, so place piece there
	board[index] = pieceToPlace;
}//end placeRandomPiece()

//-------------------------------------------------------------------------------------
// Initializes the board to 0
void initializeBoard(int board[], int squaresPerSide, int value)
{
	int i;
	for (i = 0; i < squaresPerSide * squaresPerSide; i++)
	{
		board[i] = value;
	}
}

//------------------------------------------------------------------------------------
// Creates a copy of the board
void copyBoard(int sourceBoard[], int board2[], int squaresPerSide)
{
	int i;
	for (i = 0; i < squaresPerSide * squaresPerSide; i++)
	{
		board2[i] = sourceBoard[i];
	}
}

//-------------------------------------------------------------------------------------
// moveNumbers all pieces to the left
// User input is: 'a'
void slideLeft(int board[], int squaresPerSide, int &score)
{
	// Slide the values to the left
	for (int current = 0; current < squaresPerSide * squaresPerSide; current++)
	{
		if (board[current] != 0)
		{
			while (current >= 0
				&& current % squaresPerSide != 0
				&& board[current - 1] == 0
				&& current <= squaresPerSide * squaresPerSide)
			{
				board[current - 1] = board[current];
				board[current] = 0;
				current -= (current + 1);
			}
		}
	}
	// If the adjacent values to the left are the same then merge
	for (int current = 0; current < squaresPerSide * squaresPerSide; current++)
	{
		if (board[current] == board[current - 1] && current % squaresPerSide != 0)
		{
			board[current - 1] += board[current];
			board[current] = 0;
			score += board[current - 1];
		}
	}
	// Shift values to the left again to avoid merging all values to the left if applicable
	for (int current = 0; current < squaresPerSide * squaresPerSide; current++)
	{
		if (board[current] != 0)
		{
			while (current % squaresPerSide != 0
				&& board[current - 1] == 0
				&& current <= squaresPerSide * squaresPerSide)
			{
				board[current - 1] = board[current];
				board[current] = 0;
				current -= (current + 1);
			}
		}
	}
}

//-------------------------------------------------------------------------------------
// moveNumbers all pieces to the right
// User input is: 'd'
void slideRight(int board[], int squaresPerSide, int &score)
{
	int i, j;
	// Iterate through entire board. Values of i in this loop are 3,7,11,15
	for (i = squaresPerSide - 1; i < squaresPerSide * squaresPerSide; i = i + squaresPerSide)
	{
		for (j = i - 1; (j + 1) % squaresPerSide > 0; j--)
		{
			// Current value: starting at one value to the left of limit; Continues until next limit
			int current = j;
			// Limit value: edge case values on the right side: 3,7,11,15
			int limit = i;
			while (current < limit && board[current + 1] == 0)
			{
				// swap with the piece to the right
				board[current + 1] = board[current];
				board[current] = 0;
				current++;
			}

			// Check if the piece and its right neighbor are the same value.
			// If they are then add both values to the new index
			if (board[current + 1] == board[current] && board[current + 1] != 0 && current < limit)
			{
				board[current + 1] = board[current + 1] + board[current];
				board[current] = 0;
				score += board[current + 1];
				continue;
			}
		}
	}

}

//----------------------------------------------------------------------------------------------------
// moveNumbers all pieces upward
// User input is: 'w'
void slideUp(int board[], int squaresPerSide, int &score)
{
	// Shift all values upward
	for (int current = 0; current < squaresPerSide * squaresPerSide; current++)
	{
		if (board[current] != 0)
		{
			while (current >= squaresPerSide
				&& board[current - squaresPerSide] == 0
				&& current <= squaresPerSide * squaresPerSide)
			{
				board[current - squaresPerSide] = board[current];
				board[current] = 0;
				current = current - (squaresPerSide + 1);
			}
		}
	}
	// If values upward are the same value then merge the values
	for (int current = 0; current < (squaresPerSide * squaresPerSide); current++)
	{
		if (board[current] == board[current - squaresPerSide] && current >= squaresPerSide)
		{
			board[current - squaresPerSide] += board[current];
			board[current] = 0;
			score += board[current - squaresPerSide];
		}
	}
	// Shift values upward again to avoid merging all values at once
	for (int current = 0; current < squaresPerSide * squaresPerSide; current++)
	{
		if (board[current] != 0)
		{
			while (current >= squaresPerSide
				&& board[current - squaresPerSide] == 0
				&& current <= squaresPerSide * squaresPerSide)
			{
				board[current - squaresPerSide] = board[current];
				board[current] = 0;
				current = current - (squaresPerSide + 1);
			}
		}
	}
}

//-------------------------------------------------------------------------------------
// moveNumbers all pieces downward
// User input is: 's'
void slideDown(int board[], int squaresPerSide, int &score)
{
	// Slide the values of the board downward
	for (int current = (squaresPerSide * squaresPerSide - 1); current >= 0; current--)
	{
		if (board[current] != 0)
		{
			while (current < (squaresPerSide * squaresPerSide - squaresPerSide)
				&& board[current + squaresPerSide] == 0
				&& current <= squaresPerSide * squaresPerSide)
			{
				board[current + squaresPerSide] = board[current];
				board[current] = 0;
				current = current + (squaresPerSide + 1);
			}
		}
	}
	// If any adjacent values when going down are the same, then merge
	for (int current = (squaresPerSide * squaresPerSide - 1); current >= 0; current--)
	{
		if (board[current] == board[current + squaresPerSide]
			&& current < (squaresPerSide * squaresPerSide - squaresPerSide))
		{
			board[current + squaresPerSide] += board[current];
			board[current] = 0;
			score += board[current + squaresPerSide];
		}
	}
	// Shift downward again to avoid merging all values at once
	for (int current = (squaresPerSide * squaresPerSide - 1); current >= 0; current--)
	{
		if (board[current] != 0) {
			while (current < (squaresPerSide * squaresPerSide - squaresPerSide)
				&& board[current + squaresPerSide] == 0
				&& current <= squaresPerSide * squaresPerSide)
			{
				board[current + squaresPerSide] = board[current];
				board[current] = 0;
				current = current + (squaresPerSide + 1);
			}
		}
	}
}

//--------------------------------------------------------------------------------------
// Sets the piece value where user wants
void setPiece(int board[], int index, int value)
{
	board[index] = value;
}

//----------------------------------------------------------------------------------------
// Return the exponent value
int raiseToThePowerOf(int square, int powerOf)
{
	int exponent = 1;
	for (int i = 1; i <= powerOf; i++)
	{
		exponent = exponent * square;
	}
	return exponent;
}

//-----------------------------------------------------------------------------------------
// Tests to see if the game has every piece filled
// If it does then the function will return true
bool gameNotFinished(int board[], int squaresPerSide)
{
	// Check if the board is full
	int notZero = 0;   // Used to count if there are any empty values
	for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
	{
		if (board[i] != 0)
		{
			++notZero;
		}

	}

	// If full then end the game
	if (notZero == squaresPerSide * squaresPerSide)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------------------
//
void prepend(Node *&pHead, int board[MaxBoardSize*MaxBoardSize], int &moveNumber, int &score, int squaresPerSide)
{
	int i;
	// Reference node to class
	Node *pTemp = new Node;

	// node of the board values
	pTemp->moveNumber = moveNumber;
	pTemp->score = score;
	for (i = 0; i < MaxBoardSize * MaxBoardSize; i++)
	{
		pTemp->board[i] = board[i];
	}

	pTemp->pNext = pHead;
	pHead = pTemp;
}

//--------------------------------------------------------------------------------------
//
void restoreBoard(Node *&pHead, int board[], int &moveNumber, int &score, int squaresPerSide)
{
	int i;
	moveNumber = pHead->moveNumber;
	score = pHead->score;
	for (i = 0; i < squaresPerSide * squaresPerSide; i++)
	{
		board[i] = pHead->board[i];
	}
}

//--------------------------------------------------------------------------------------
// Undo the move
void undoMove(Node *&pHead)
{
	// Check that user does NOT go past the start of the game
	if (pHead->pNext == NULL)
	{
		std::cout << "        *** You cannot undo past the beginning of the game.  Please retry. ***" << std::endl;
	}
	else
	{
		Node *pDelete = pHead;
		pHead = pHead->pNext;
		delete pDelete;
	}
}

void displayList(Node *pHead, int moveNumber)
{
	std::cout << "        List: ";
	while (pHead != NULL)
	{
		if (pHead->moveNumber > 1)
		{
			std::cout << pHead->moveNumber << "->";
			pHead = pHead->pNext;
		}
		if (pHead->moveNumber == 1)
		{
			std::cout << pHead->moveNumber;
			break;
		}
	}
	std::cout << std::endl << std::endl;
}

//---------------------------------------------------------------------------------------
int main()
{
	int moveNumber = 1;               // User moveNumber counter
	int score = 0;                    // Cummulative score, which is sum of combined tiles
	int squaresPerSide = 4;           // User will enter this value.  Set default to 4
	int board[MaxBoardSize * MaxBoardSize];          // space for largest possible board
	int previousBoard[MaxBoardSize * MaxBoardSize];  // space for copy of board, used to see 
													  //    if a moveNumber changed the board.
	// Create the graphical board, an array of Square objects set to be the max size it will ever be.
	Square squaresArray[MaxBoardSize * MaxBoardSize];
	int maxTileValue = 1024;  // 1024 for 4x4 board, 2048 for 5x5, 4096 for 6x6, etc.
	char userInput = ' ';     // Stores user input
	char aString[81];        // C-string to hold concatenated output of character literals
	int powerOf;  // Used to determine the the power maxTileValue will be raised to
	int userChoiceIndex;  // User's choice of index to be changed
	int userValue;  // User choice of value to be placed in the user's choice of index
	int listCounter = moveNumber;  // Used to display the list values

	// Create the graphics window
	sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 5: 1024");
	std::cout << std::endl;
	// Create and initialize the font, to be used in displaying text.
	sf::Font font;
	initializeFont(font);
	// Create the messages label at the bottom of the graphics screen, for displaying debugging information
	sf::Text messagesLabel("Welcome to 1024", font, 20);
	// Make a text object from the font
	messagesLabel.setColor(sf::Color(255, 255, 255));
	// Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
	messagesLabel.setPosition(0, WindowYSize - messagesLabel.getCharacterSize() - 5);

	// Display the instructions of the game
	displayInstructions();

	// Get the board size, create and initialize the board, and set the max tile value
	initializeBoard(board, squaresPerSide, 0);

	// Place initial starting random pieces
	placeRandomPiece(board, squaresPerSide);
	placeRandomPiece(board, squaresPerSide);

	// Display the board game max tile value
	std::cout << std::endl;
	std::cout << "Game ends when you reach " << maxTileValue << "." << std::endl;

	Node *pHead = NULL;
	// Declare a pointer for the head of the list.  Add a node onto the list.  
	//    The list may grow and shrink, but this first node should always be there.
	prepend(pHead, board, moveNumber, score, squaresPerSide);

	for (int i = 0; i < squaresPerSide; i++)
	{
		for (int j = 0; j < squaresPerSide; j++)
		{

			// 0    1    2    3
			// 4    5    6    7
			// 8    9    10   11
			// 12   13   14   15

			// Allocate each square individually.
			// Store a string in each square which contains its number, in order of creation
			char name[81];
			// Squares with a 0 value should not have a number displayed
			if (board[(i * 4) + j] == 0)
			{
				strcpy(name, "");   // "print" a blank text string
			}
			else
			{
				sprintf(name, "%d", board[(i * 4) + j]);   // "print" the square creation order number into a string 
														//      to be stored in the square
			}
			// Set each array pointer have the address of a new Square, created with a Square constructor
								  // Size, X pos + diff, Y pos + diff,  Color,    Visibility,  Text
			squaresArray[(i * 4) + j] = Square(90, 90 * j + j * 5, 90 * i + i * 5, sf::Color::White, true, name);
			// Draw the square
			window.draw(squaresArray[(i * 4) + j].getTheSquare());
			// Draw the text associated with the Square, in the window with the indicated color and text size
			int red = 0, green = 0, blue = 0;
			squaresArray[(i * 4) + j].displayText(&window, font, sf::Color(red, green, blue), 30);
		}
	}

	// Run the program as long as the window is open.  This is known as the "Event loop".
	while (window.isOpen())
	{
		// Display both the graphical and text boards.
		displayAsciiBoard(board, squaresPerSide, score);

		sprintf(aString, "Move %d", moveNumber);   // Print into aString
		messagesLabel.setString(aString);            // Store the string into the messagesLabel
		window.draw(messagesLabel);                  // Display the messagesLabel

		// Display the background frame buffer, replacing the previous RenderWindow frame contents.
		// This is known as "double-buffering", where you first draw into a background frame, and then
		// replace the currently displayed frame with this background frame.
		window.display();

		// Make a copy of the board.  After we then attempt a moveNumber, the copy will be used to 
		// verify that the board changed, which only then allows randomly placing an additional  
		// piece on the board and updating the moveNumber number.
		copyBoard(board, previousBoard, squaresPerSide);

		// Display the list
		displayList(pHead, moveNumber);

		// Prompt for and handle user input
		std::cout << moveNumber << ". Your move: ";
		std::cin >> userInput;
		switch (userInput) {
		case 'x':
			std::cout << "Thanks for playing. Exiting program... \n\n";
			window.close();
			exit(0);
			break;
			// Case for individually setting a value on the board
		case 'p':
			std::cin >> userChoiceIndex >> userValue;
			setPiece(board, userChoiceIndex, userValue);
			continue;
			break;
			// Case for resetting the board
		case 'r':
			std::cout << "Resetting board" << std::endl << std::endl;
			std::cout << "Enter the size board you want, between 4 and 12: ";

			// User choice of new squaresPerSide
			std::cin >> squaresPerSide;

			// Determine the difference first, then call the function to raise it
			powerOf = squaresPerSide - 4;

			// Output of new value of MaxTileStartValue
			std::cout << std::endl << "Game ends when you reach "
				<< raiseToThePowerOf(2, powerOf) * MaxTileStartValue << "." << std::endl;

			// Initialize the new board
			initializeBoard(board, squaresPerSide, 0);

			// Place new random value, and start over moveNumbers & score
			placeRandomPiece(board, squaresPerSide);
			score = 0;
			moveNumber = 0;
			break;

			// Left moveNumber
		case 'a':
			slideLeft(board, squaresPerSide, score);
			//prepend( pHead, board, moveNumber, score, squaresPerSide );
			break;
			// Upward moveNumber
		case 'w':
			slideUp(board, squaresPerSide, score);
			//prepend( pHead, board, moveNumber, score, squaresPerSide );
			break;
			// Right moveNumber
		case 'd':
			slideRight(board, squaresPerSide, score);
			//prepend( pHead, board, moveNumber, score, squaresPerSide );
			break;
			// Downward moveNumber
		case 's':
			slideDown(board, squaresPerSide, score);
			//prepend( pHead, board, moveNumber, score, squaresPerSide );
			break;
		case 'u':
			if (moveNumber > 1)
			{
				--moveNumber;
				std::cout << "        * Undoing move *" << std::endl;
			}

			undoMove(pHead);
			restoreBoard(pHead, board, moveNumber, score, squaresPerSide);

			// Clear the graphics window, erasing what is displayed
			window.clear();

			// Redraw all screen components to the background frame buffer
			for (int i = 0; i < squaresPerSide; i++)
			{
				for (int j = 0; j < squaresPerSide; j++)
				{
					// Allocate each square individually.
					// Store a string in each square which contains its number, in order of creation
					char name[81];
					// Squares with a 0 value should not have a number displayed
					if (board[(i * 4) + j] == 0)
					{
						strcpy(name, "");   // "print" a blank text string
					}
					else
					{
						sprintf(name, "%d", board[(i * 4) + j]);   // "print" the square creation order number into a 
																//     string to be stored in the square
					}
					// Set each array pointer have the address of a new Square, created with a Square constructor
											   // Size, X pos + diff, Y pos + diff,  Color,    Visibility,  Text
					squaresArray[(i * 4) + j] = Square(90, 90 * j + j * 5, 90 * i + i * 5, sf::Color::White, true, name);
					// Draw the square
					window.draw(squaresArray[(i * 4) + j].getTheSquare());
					// Draw the text associated with the Square, in the window with the indicated color and text size
					int red = 0, green = 0, blue = 0;
					squaresArray[(i * 4) + j].displayText(&window, font, sf::Color(red, green, blue), 30);
				}
			}
			continue;
			break;
		default:
			std::cout << "Invalid input, please retry.";
			continue;
			break;
		}//end switch( userInput)

		// If the moveNumber resulted in pieces changing position, then it was a valid moveNumber
		// so place a new random piece (2 or 4) in a random open square and update moveNumber number.
		// Add the new board, moveNumberNumber and score to a new list node at the front of the list.
		for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
		{
			if (board[i] != previousBoard[i])
			{
				placeRandomPiece(board, squaresPerSide);
				moveNumber++;
				prepend(pHead, board, moveNumber, score, squaresPerSide);
				break;
			}
		}
		// Clear the graphics window, erasing what is displayed
		window.clear();

		// Redraw all screen components to the background frame buffer
		for (int i = 0; i < squaresPerSide; i++)
		{
			for (int j = 0; j < squaresPerSide; j++)
			{

				// Allocate each square individually.
				// Store a string in each square which contains its number, in order of creation
				char name[81];
				// Squares with a 0 value should not have a number displayed
				if (board[(i * 4) + j] == 0) {
					strcpy(name, "");   // "print" a blank text string
				}
				else {
					sprintf(name, "%d", board[(i * 4) + j]);   // "print" the square creation order number into a 
															//     string to be stored in the square
				}
				// Set each array pointer have the address of a new Square, created with a Square constructor
									  // Size, X pos + diff, Y pos + diff,  Color,    Visibility,  Text
				squaresArray[(i * 4) + j] = Square(90, 90 * j + j * 5, 90 * i + i * 5, sf::Color::White, true, name);
				// Draw the square
				window.draw(squaresArray[(i * 4) + j].getTheSquare());
				// Draw the text associated with the Square, in the window with the indicated color and text size
				int red = 0, green = 0, blue = 0;
				squaresArray[(i * 4) + j].displayText(&window, font, sf::Color(red, green, blue), 30);
			}
		}

		// See if we're done.  If so, display the final board and break.
		for (int i = 0; i < squaresPerSide * squaresPerSide; i++)
		{
			if (board[i] == maxTileValue)
			{
				std::cout << "Congratulations!  You made it to " << maxTileValue << "!!!" << std::endl;
				displayAsciiBoard(board, squaresPerSide, score);
				exit(0);
			}
		}

		//system("clear");   // Clear the screen 

		// Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	}//end while( window.isOpen())

	// Display the final boards and messages
	displayAsciiBoard(board, squaresPerSide, score);
	std::cout << moveNumber << ". Your moveNumber: " << std::endl;
	std::cout << "No more available moveNumbers.  Game is over." << std::endl;
	displayAsciiBoard(board, squaresPerSide, score);

	return 0;
}//end main()